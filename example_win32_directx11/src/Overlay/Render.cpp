#include "Render.hpp"
#include <Windows.h>
#include <string>
#include <algorithm>
#include <cctype>
#include <vector>
#include <TlHelp32.h>
#include <iostream>

HWND RenderWindow = nullptr;

namespace Render
{
    std::vector<DWORD> GetProcessIdsByName(const std::wstring& ProcessName)
    {
        std::vector<DWORD> processIds;

        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot == INVALID_HANDLE_VALUE) {
            return processIds;
        }

        PROCESSENTRY32W processEntry;
        processEntry.dwSize = sizeof(PROCESSENTRY32W);

        if (Process32FirstW(hSnapshot, &processEntry))
        {
            do {
                if (ProcessName == processEntry.szExeFile)
                {
                    processIds.push_back(processEntry.th32ProcessID);
                }

            } while (Process32NextW(hSnapshot, &processEntry));
        }

        CloseHandle(hSnapshot);
        return processIds;
    }


    inline BOOL CALLBACK EnumChildWindowsProc(HWND hWnd, LPARAM lParam)
    {
        char windowName[256];
        GetWindowTextA(hWnd, windowName, sizeof(windowName));
        std::string name(windowName);

        char className[256];
        GetClassNameA(hWnd, className, sizeof(className));
        std::string class_str(className);

        if (name == "_ctl.Window" && class_str.find("BlueStacksApp") != std::wstring::npos)
        {
            RenderWindow = hWnd;
            return FALSE;
        }
        else if (name == "HD-Player" && class_str.find("Qt") != std::wstring::npos)
        {
            RenderWindow = hWnd;
            return FALSE;
        }

        return TRUE;
    }

    inline BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
    {
        DWORD processId = *reinterpret_cast<DWORD*>(lParam);

        DWORD windowProcessId;
        GetWindowThreadProcessId(hWnd, &windowProcessId);

        if (windowProcessId == processId)
        {
            EnumChildWindows(hWnd, EnumChildWindowsProc, lParam);
        }

        return TRUE;
    }

    HWND Render::FindRenderWindow()
    {
        RenderWindow = nullptr;

        static const wchar_t* kEmulatorProcesses[] = {
            L"HD-Player.exe",
            L"Bluestacks.exe",
            L"BlueStacks.exe",
            L"MSIAppPlayer.exe",
            L"BlueStacks_nxt.exe",
            L"dnplayer.exe",
            L"LdBoxHeadless.exe",
            L"Nox.exe",
            L"MEmu.exe",
            L"AndroidEmulator.exe",
            L"Gameloop.exe",
        };

        std::vector<DWORD> processIds;
        for (const wchar_t* exeName : kEmulatorProcesses) {
            const auto found = GetProcessIdsByName(exeName);
            processIds.insert(processIds.end(), found.begin(), found.end());
        }

        if (processIds.empty()) {
            std::cout << "[ERROR] No supported emulator process found!" << std::endl;
            return nullptr;
        }

        for (const DWORD processId : processIds) {
            const HANDLE processHandle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
            if (processHandle) {
                EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&processId));
                CloseHandle(processHandle);
                if (RenderWindow)
                    break;
            }
        }

        return RenderWindow;
    }
}