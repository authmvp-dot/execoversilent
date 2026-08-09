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
    int adbPort = 5555;
};

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
                std::wstring exePath = L"";
                HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pe.th32ProcessID);
                if (hProc) {
                    wchar_t pathBuf[MAX_PATH] = { 0 };
                    DWORD pathSize = MAX_PATH;
                    if (QueryFullProcessImageNameW(hProc, 0, pathBuf, &pathSize)) {
                        exePath = pathBuf;
                    }
                    CloseHandle(hProc);
                }

                // Check path for MSI vs BlueStacks
                if (exePath.find(L"BlueStacks_msi5") != std::wstring::npos || exePath.find(L"MSI") != std::wstring::npos) {
                    info.type = EmulatorType::MSIAppPlayer;
                    info.name = "MSI App Player 5";
                    info.adbPort = 5555; // ADB default port for MSI 5
                    break;
                } else {
                    info.type = EmulatorType::BlueStacks;
                    info.name = "BlueStacks 5";
                    info.adbPort = 5555; // ADB default port for BlueStacks 5
                    break;
                }
            }
        } while (Process32NextW(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
    return info;
}
