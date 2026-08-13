#pragma once
#include <Windows.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <TlHelp32.h>
#include <tchar.h>
#define WIN32_LEAN_AND_MEAN
#include <winternl.h>
#include <sstream>
#include <mmsystem.h>
#include <mutex>
#include <future>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "ntdll.lib")

extern "C" NTSTATUS ZwReadVirtualMemory(HANDLE hProcess, LPVOID lpBaseAddress, void* lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead = NULL);
extern "C" NTSTATUS ZwWriteVirtualMemory(HANDLE hProcess, LPVOID lpBaseAddress, void* lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead = NULL);
extern "C" NTSTATUS ZwProtectVirtualMemory(HANDLE hProcess, LPVOID BaseAddress, size_t NumberOfBytesToProtect, ULONG NewAccessProtection, PULONG OldAccessProtection);

class MamunMemoryEngine {
public:
    DWORD ProcessId = 0;
    HANDLE ProcessHandle = nullptr;

    typedef struct _MEMORY_REGION
    {
        DWORD_PTR dwBaseAddr;
        DWORD_PTR dwMemorySize;
    } MEMORY_REGION;

    std::unordered_map<DWORD_PTR, int> modifiedAoBs;

    const char* GetEmulatorRunning()
    {
        if (GetPid("HD-Player.exe") != 0) return "HD-Player.exe";
        else if (GetPid("HD-Player") != 0) return "HD-Player";
        else if (GetPid("HD-Player64.exe") != 0) return "HD-Player64.exe";
        else if (GetPid("HD-Player64") != 0) return "HD-Player64";
        else if (GetPid("MEmuHeadless.exe") != 0) return "MEmuHeadless.exe";
        else if (GetPid("LdVBoxHeadless.exe") != 0) return "LdVBoxHeadless.exe";
        else if (GetPid("AndroidProcess.exe") != 0) return "AndroidProcess.exe";
        else if (GetPid("Nox.exe") != 0) return "Nox.exe";
        return nullptr;
    }

    int GetPid(const char* procname)
    {
        if (procname == NULL) return 0;
        DWORD pid = 0;
        DWORD threadCount = 0;

        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnap == INVALID_HANDLE_VALUE) return 0;

        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hSnap, &pe)) {
            do {
                if (_tcsicmp(pe.szExeFile, procname) == 0) {
                    if ((int)pe.cntThreads > threadCount) {
                        threadCount = pe.cntThreads;
                        pid = pe.th32ProcessID;
                    }
                }
            } while (Process32Next(hSnap, &pe));
        }
        CloseHandle(hSnap);
        return pid;
    }

    BOOL AttackProcess(const char* procname)
    {
        if (!procname) return false;
        DWORD ProcId = GetPid(procname);
        if (ProcId == 0) return false;

        ProcessId = ProcId;
        ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, ProcessId);
        if (!ProcessHandle) {
            ProcessHandle = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION, 0, ProcessId);
        }
        return ProcessHandle != nullptr;
    }

    bool FastFindPattern(DWORD_PTR StartRange, DWORD_PTR EndRange, const std::vector<BYTE>& searchBytes, std::vector<DWORD_PTR>& AddressRet) {
        if (!ProcessHandle || searchBytes.empty()) return false;
        MEMORY_BASIC_INFORMATION mbi;
        mbi.RegionSize = 0x1000;
        DWORD_PTR dwAddress = StartRange;
        DWORD_PTR nSearchSize = searchBytes.size();

        std::vector<MEMORY_REGION> m_vMemoryRegion;

        while (VirtualQueryEx(ProcessHandle, (LPCVOID)dwAddress, &mbi, sizeof(mbi)) && (dwAddress < EndRange) && ((dwAddress + mbi.RegionSize) > dwAddress)) {
            bool isCommit = (mbi.State == MEM_COMMIT);
            bool isReadable = !(mbi.Protect & PAGE_GUARD) && (mbi.Protect != PAGE_NOACCESS) && ((mbi.AllocationProtect & PAGE_NOCACHE) != PAGE_NOCACHE);
            if (isCommit && isReadable) {
                MEMORY_REGION mData = { (DWORD_PTR)mbi.BaseAddress, mbi.RegionSize };
                m_vMemoryRegion.push_back(mData);
            }
            uintptr_t nextAddr = (uintptr_t)mbi.BaseAddress + mbi.RegionSize;
            if (nextAddr <= dwAddress) break;
            dwAddress = nextAddr;
        }

        std::mutex mtx;
        auto processRegion = [&](MEMORY_REGION mData) {
            std::vector<BYTE> buffer(mData.dwMemorySize);
            SIZE_T bytesRead = 0;
            if (ReadProcessMemory(ProcessHandle, (LPCVOID)mData.dwBaseAddr, buffer.data(), mData.dwMemorySize, &bytesRead) && bytesRead >= nSearchSize) {
                DWORD_PTR dwOffset = 0;
                int iOffset = Memfind(buffer.data(), bytesRead, (BYTE*)searchBytes.data(), nSearchSize);
                while (iOffset != -1) {
                    dwOffset += iOffset;
                    DWORD_PTR firstByteAddress = dwOffset + mData.dwBaseAddr;

                    std::lock_guard<std::mutex> lock(mtx);
                    if (modifiedAoBs.find(firstByteAddress) == modifiedAoBs.end()) {
                        AddressRet.push_back(firstByteAddress);
                        modifiedAoBs[firstByteAddress] = 1;
                    }

                    dwOffset += nSearchSize;
                    if (dwOffset + nSearchSize > bytesRead) break;
                    iOffset = Memfind(buffer.data() + dwOffset, bytesRead - dwOffset, (BYTE*)searchBytes.data(), nSearchSize);
                }
            }
        };

        std::vector<std::future<void>> futures;
        for (auto& region : m_vMemoryRegion) {
            futures.push_back(std::async(std::launch::async, processRegion, region));
        }

        for (auto& fut : futures) {
            fut.get();
        }

        return true;
    }

    int Memfind(BYTE* buffer, DWORD_PTR dwBufferSize, BYTE* bstr, DWORD_PTR dwStrLen)
    {
        if (dwBufferSize < 0) return -1;
        DWORD_PTR i, j;
        for (i = 0; i < dwBufferSize; i++) {
            for (j = 0; j < dwStrLen; j++) {
                if (buffer[i + j] != bstr[j] && bstr[j] != '?')
                    break;
            }
            if (j == dwStrLen) return i;
        }
        return -1;
    }

    bool WriteBytes(DWORD_PTR address, const std::vector<BYTE>& bytes) {
        if (!ProcessHandle || address == 0 || bytes.empty()) return false;
        DWORD oldProtect;
        VirtualProtectEx(ProcessHandle, (LPVOID)address, bytes.size(), PAGE_EXECUTE_READWRITE, &oldProtect);
        SIZE_T written = 0;
        bool result = WriteProcessMemory(ProcessHandle, (LPVOID)address, bytes.data(), bytes.size(), &written);
        VirtualProtectEx(ProcessHandle, (LPVOID)address, bytes.size(), oldProtect, &oldProtect);
        return result;
    }
};
