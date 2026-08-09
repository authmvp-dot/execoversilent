#pragma once
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
            if (procName == L"HD-Player.exe") {
                info.type = EmulatorType::BlueStacks;
                info.name = "BlueStacks App Player";
                info.adbPort = 5555;
                break;
            } else if (procName == L"MSIAppPlayer.exe" || procName == L"MSI-Player.exe") {
                info.type = EmulatorType::MSIAppPlayer;
                info.name = "MSI App Player";
                info.adbPort = 5554;
                break;
            }
        } while (Process32NextW(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
    return info;
}
