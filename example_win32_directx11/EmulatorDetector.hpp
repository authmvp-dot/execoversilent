#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <windows.h>
#include <tlhelp32.h>
#include <string>

enum class EmulatorType {
    None,
    BlueStacks,
    MSIAppPlayer
};

struct EmulatorInfo {
    EmulatorType type = EmulatorType::None;
    std::string name = "None";
    std::string adbExePath = "hd-adb.exe";
    int adbPort = 5555;
};

inline bool FileExistsA(const std::string& path) {
    DWORD dwAttrib = GetFileAttributesA(path.c_str());
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

inline EmulatorInfo DetectRunningEmulator() {
    EmulatorInfo info;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return info;
    }

    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(PROCESSENTRY32W);

    if (Process32FirstW(hSnapshot, &pe)) {
        do {
            std::wstring procName(pe.szExeFile);
            if (procName == L"HD-Player.exe" || procName == L"MSIAppPlayer.exe" || procName == L"MSI-Player.exe") {
                std::wstring exePathW = L"";
                HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pe.th32ProcessID);
                if (hProc) {
                    wchar_t pathBuf[MAX_PATH] = { 0 };
                    DWORD pathSize = MAX_PATH;
                    if (QueryFullProcessImageNameW(hProc, 0, pathBuf, &pathSize)) {
                        exePathW = pathBuf;
                    }
                    CloseHandle(hProc);
                }

                std::string exePathA = "";
                if (!exePathW.empty()) {
                    char charBuf[MAX_PATH] = { 0 };
                    WideCharToMultiByte(CP_ACP, 0, exePathW.c_str(), -1, charBuf, MAX_PATH, NULL, NULL);
                    exePathA = charBuf;
                }

                // Check path for MSI vs BlueStacks
                if (exePathW.find(L"BlueStacks_msi5") != std::wstring::npos || exePathW.find(L"MSI") != std::wstring::npos) {
                    info.type = EmulatorType::MSIAppPlayer;
                    info.name = "MSI App Player 5";
                    info.adbPort = 5555;
                } else {
                    info.type = EmulatorType::BlueStacks;
                    info.name = "BlueStacks 5";
                    info.adbPort = 5555;
                }

                // Locate HD-Adb.exe path in emulator directory
                std::string dirPath = "";
                size_t lastSlash = exePathA.find_last_of("\\/");
                if (lastSlash != std::string::npos) {
                    dirPath = exePathA.substr(0, lastSlash + 1);
                }

                if (!dirPath.empty() && FileExistsA(dirPath + "HD-Adb.exe")) {
                    info.adbExePath = dirPath + "HD-Adb.exe";
                } else if (!dirPath.empty() && FileExistsA(dirPath + "adb.exe")) {
                    info.adbExePath = dirPath + "adb.exe";
                } else if (FileExistsA("C:\\Program Files\\BlueStacks_nxt\\HD-Adb.exe")) {
                    info.adbExePath = "C:\\Program Files\\BlueStacks_nxt\\HD-Adb.exe";
                } else if (FileExistsA("C:\\Program Files\\BlueStacks_msi5\\HD-Adb.exe")) {
                    info.adbExePath = "C:\\Program Files\\BlueStacks_msi5\\HD-Adb.exe";
                } else if (FileExistsA("C:\\Program Files (x86)\\BlueStacks_nxt\\HD-Adb.exe")) {
                    info.adbExePath = "C:\\Program Files (x86)\\BlueStacks_nxt\\HD-Adb.exe";
                } else if (FileExistsA("C:\\Program Files (x86)\\BlueStacks_msi5\\HD-Adb.exe")) {
                    info.adbExePath = "C:\\Program Files (x86)\\BlueStacks_msi5\\HD-Adb.exe";
                } else {
                    info.adbExePath = "hd-adb.exe";
                }

                break;
            }
        } while (Process32NextW(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
    return info;
}
