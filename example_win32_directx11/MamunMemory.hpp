#pragma once
#include <windows.h>
#include <tlhelp32.h>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <thread>
#include <mutex>

class MamunMemory {
public:
    HANDLE hProcess = NULL;
    DWORD processId = 0;

    ~MamunMemory() {
        if (hProcess && hProcess != INVALID_HANDLE_VALUE) {
            CloseHandle(hProcess);
            hProcess = NULL;
        }
    }

    bool SetProcess(const std::vector<std::string>& processNames) {
        if (hProcess && processId > 0) {
            DWORD exitCode = 0;
            if (GetExitCodeProcess(hProcess, &exitCode) && exitCode == STILL_ACTIVE) {
                return true;
            }
            CloseHandle(hProcess);
            hProcess = NULL;
            processId = 0;
        }

        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot == INVALID_HANDLE_VALUE) return false;

        PROCESSENTRY32W pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32W);

        if (Process32FirstW(hSnapshot, &pe32)) {
            do {
                std::wstring wName(pe32.szExeFile);
                std::string exeName(wName.begin(), wName.end());
                for (const auto& target : processNames) {
                    if (_stricmp(exeName.c_str(), target.c_str()) == 0 ||
                        _stricmp(exeName.c_str(), (target + ".exe").c_str()) == 0) {
                        processId = pe32.th32ProcessID;
                        break;
                    }
                }
                if (processId > 0) break;
            } while (Process32NextW(hSnapshot, &pe32));
        }
        CloseHandle(hSnapshot);

        if (processId == 0) return false;

        hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
        if (!hProcess) {
            hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION, FALSE, processId);
        }
        return (hProcess != NULL);
    }

    struct PatternData {
        std::vector<BYTE> pattern;
        std::vector<BYTE> mask;
    };

    static PatternData ParsePattern(const std::string& patternStr) {
        PatternData pd;
        std::stringstream ss(patternStr);
        std::string token;
        while (ss >> token) {
            if (token == "??" || token == "?") {
                pd.pattern.push_back(0x00);
                pd.mask.push_back(0x00);
            } else {
                BYTE val = (BYTE)strtoul(token.c_str(), nullptr, 16);
                pd.pattern.push_back(val);
                pd.mask.push_back(0xFF);
            }
        }
        return pd;
    }

    std::vector<uintptr_t> AobScan(const std::string& patternStr) {
        std::vector<uintptr_t> results;
        if (!hProcess) return results;

        PatternData pd = ParsePattern(patternStr);
        if (pd.pattern.empty()) return results;

        MEMORY_BASIC_INFORMATION mbi;
        uintptr_t address = 0;

        std::vector<MEMORY_BASIC_INFORMATION> pages;
        while (VirtualQueryEx(hProcess, (LPCVOID)address, &mbi, sizeof(mbi)) == sizeof(mbi)) {
            bool isReadable = (mbi.State == MEM_COMMIT) &&
                              !(mbi.Protect & PAGE_NOACCESS) &&
                              !(mbi.Protect & PAGE_GUARD) &&
                              (mbi.Protect & (PAGE_READWRITE | PAGE_EXECUTE_READWRITE | PAGE_READONLY | PAGE_EXECUTE_READ));
            if (isReadable) {
                pages.push_back(mbi);
            }
            uintptr_t nextAddr = (uintptr_t)mbi.BaseAddress + mbi.RegionSize;
            if (nextAddr <= address) break;
            address = nextAddr;
        }

        std::mutex mtx;
        size_t patternLen = pd.pattern.size();

        auto worker = [&](size_t startIdx, size_t endIdx) {
            for (size_t i = startIdx; i < endIdx; ++i) {
                const auto& page = pages[i];
                std::vector<BYTE> buffer(page.RegionSize);
                SIZE_T bytesRead = 0;
                
                if (ReadProcessMemory(hProcess, page.BaseAddress, buffer.data(), page.RegionSize, &bytesRead) && bytesRead >= patternLen) {
                    for (size_t pos = 0; pos <= bytesRead - patternLen; ++pos) {
                        bool match = true;
                        for (size_t k = 0; k < patternLen; ++k) {
                            if ((buffer[pos + k] & pd.mask[k]) != (pd.pattern[k] & pd.mask[k])) {
                                match = false;
                                break;
                            }
                        }
                        if (match) {
                            std::lock_guard<std::mutex> lock(mtx);
                            results.push_back((uintptr_t)page.BaseAddress + pos);
                        }
                    }
                }
            }
        };

        unsigned int numThreads = std::thread::hardware_concurrency();
        if (numThreads == 0) numThreads = 4;
        std::vector<std::thread> threads;
        size_t totalPages = pages.size();
        size_t chunkSize = (totalPages + numThreads - 1) / numThreads;

        for (unsigned int t = 0; t < numThreads; ++t) {
            size_t startIdx = t * chunkSize;
            size_t endIdx = (std::min)(startIdx + chunkSize, totalPages);
            if (startIdx < endIdx) {
                threads.emplace_back(worker, startIdx, endIdx);
            }
        }

        for (auto& th : threads) {
            if (th.joinable()) th.join();
        }

        std::sort(results.begin(), results.end());
        return results;
    }

    bool AobReplace(uintptr_t address, const std::string& hexPattern) {
        if (!hProcess || address == 0) return false;
        std::stringstream ss(hexPattern);
        std::string token;
        std::vector<BYTE> bytes;
        while (ss >> token) {
            bytes.push_back((BYTE)strtoul(token.c_str(), nullptr, 16));
        }
        if (bytes.empty()) return false;

        SIZE_T written = 0;
        return WriteProcessMemory(hProcess, (LPVOID)address, bytes.data(), bytes.size(), &written) && written == bytes.size();
    }
};
