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
#include <windows.h>
#include <mmsystem.h>
#include "SoundPLayer.h"

#pragma comment(lib, "winmm.lib")


#include <mutex>
#include <future>
#define WIN32_LEAN_AND_MEAN
bool beepsound1 = true;
static bool fastinject = true;

#pragma comment(lib, "ntdll.lib")
extern std::string MemoryLogs;

extern "C" NTSTATUS ZwReadVirtualMemory(HANDLE hProcess, LPVOID lpBaseAddress, void* lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead = NULL);
extern "C" NTSTATUS ZwWriteVirtualMemory(HANDLE hProcess, LPVOID lpBaseAddress, void* lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead = NULL);
extern "C" NTSTATUS ZwProtectVirtualMemory(HANDLE hProcess, LPVOID BaseAddress, size_t  NumberOfBytesToProtect, ULONG NewAccessProtection, PULONG OldAccessProtection);

class AimbotMemory {
public:

    const char* GetEmulatorRunning()
    {
        if (GetPid("HD-Player.exe") != 0)
            return "HD-Player.exe";

        else if
            (GetPid("HD-Player") != 0)
            return "HD-Player";

        else if
            (GetPid("MEmuHeadless.exe") != 0)
            return "MEmuHeadless.exe";

        else if
            (GetPid("LdVBoxHeadless.exe") != 0)
            return "LdVBoxHeadless.exe";

        else if
            (GetPid("AndroidProcess.exe") != 0)
            return "AndroidProcess.exe";

        else if
            (GetPid("Nox.exe") != 0)
            return "Nox.exe";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unordered_map<DWORD_PTR, int> originalValuesWrite;
    std::unordered_map<DWORD_PTR, int> originalValuesWrite2;
    std::unordered_map<DWORD_PTR, int> modifiedValuesWrite;
    std::unordered_map<DWORD_PTR, int> modifiedValuesWrite2;
    std::unordered_map<DWORD_PTR, int> modifiedAoBs;
    std::vector<DWORD_PTR> AddressScan;
    std::vector<BYTE> ScanAimbot = { 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA5, 0x43 };

    void EnableAimNeck() {
        originalValuesWrite.clear();
        modifiedValuesWrite.clear();
        originalValuesWrite2.clear();
        modifiedValuesWrite2.clear();
        MemoryLogs.clear();
        // Activating();
       // notificationSystem.Notification("1", "Notification", "Applying Aimbot Neck Pro ! ", main_color);
        if (!AttackProcess(GetEmulatorRunning())) {

            return;
        }

        SYSTEM_INFO si;
        GetSystemInfo(&si);
        DWORD_PTR SAddress = (DWORD_PTR)si.lpMinimumApplicationAddress;
        DWORD_PTR EAddress = (DWORD_PTR)si.lpMaximumApplicationAddress;
        AddressScan.clear();
        if (fastinject) {
            FastFindPattern(SAddress, EAddress, ScanAimbot.data(), AddressScan);
        }
        else {
            for (int retry = 0; retry < 3 && AddressScan.empty(); retry++) {
                SlowFindPattern(SAddress, EAddress, ScanAimbot.data(), AddressScan);
                Sleep(50);
            }
        }
        if (AddressScan.empty()) {

            notificationSystem.AddNotification("Notification", "Failed applied!", ImGui::GetColorU32(c::accent));
            desactivar();
        }
        else {
            for (size_t i = 0; i < AddressScan.size(); i++) {
                DWORD_PTR addressscan = AddressScan[i] + 0xAA;
                DWORD_PTR addressrep = AddressScan[i] + 0xA6;

                int bufferRead, bufferWrite;

                ReadProcessMemory(ProcessHandle, (LPVOID)(addressrep), &bufferWrite, sizeof(bufferWrite), NULL);
                originalValuesWrite[addressrep] = bufferWrite;
                ReadProcessMemory(ProcessHandle, (LPVOID)(addressscan), &bufferRead, sizeof(bufferRead), NULL);
                originalValuesWrite2[addressscan] = bufferRead;
                WriteProcessMemory(ProcessHandle, (LPVOID)(addressrep), &bufferRead, sizeof(bufferRead), 0);
                modifiedValuesWrite[addressrep] = bufferRead;
                WriteProcessMemory(ProcessHandle, (LPVOID)(addressscan), &bufferWrite, sizeof(bufferWrite), 0);
                modifiedValuesWrite2[addressscan] = bufferWrite;
            }
            notificationSystem.AddNotification("Notification", "Successfully applied", ImGui::GetColorU32(c::accent));
            Activado();

        }

        CloseHandle(ProcessHandle);
    }



    void EnableAimDRAG() {
        originalValuesWrite.clear();
        modifiedValuesWrite.clear();
        originalValuesWrite2.clear();
        modifiedValuesWrite2.clear();
        MemoryLogs.clear();
        // Activating();
       // notificationSystem.Notification("1", "Notification", "Applying Aimbot Neck Pro ! ", main_color);
        if (!AttackProcess(GetEmulatorRunning())) {

            return;
        }

        SYSTEM_INFO si;
        GetSystemInfo(&si);
        DWORD_PTR SAddress = (DWORD_PTR)si.lpMinimumApplicationAddress;
        DWORD_PTR EAddress = (DWORD_PTR)si.lpMaximumApplicationAddress;
        AddressScan.clear();
        if (fastinject) {
            FastFindPattern(SAddress, EAddress, ScanAimbot.data(), AddressScan);
        }
        else {
            for (int retry = 0; retry < 3 && AddressScan.empty(); retry++) {
                SlowFindPattern(SAddress, EAddress, ScanAimbot.data(), AddressScan);
                Sleep(50);
            }
        }
        if (AddressScan.empty()) {

            notificationSystem.AddNotification("Notification", "Failed applied!", ImGui::GetColorU32(c::accent));
            desactivar();
        }
        else {
            for (size_t i = 0; i < AddressScan.size(); i++) {
                DWORD_PTR addressscan = AddressScan[i] + 0xA4;
                DWORD_PTR addressrep = AddressScan[i] + 0xA6;

                int bufferRead, bufferWrite;

                ReadProcessMemory(ProcessHandle, (LPVOID)(addressrep), &bufferWrite, sizeof(bufferWrite), NULL);
                originalValuesWrite[addressrep] = bufferWrite;
                ReadProcessMemory(ProcessHandle, (LPVOID)(addressscan), &bufferRead, sizeof(bufferRead), NULL);
                originalValuesWrite2[addressscan] = bufferRead;
                WriteProcessMemory(ProcessHandle, (LPVOID)(addressrep), &bufferRead, sizeof(bufferRead), 0);
                modifiedValuesWrite[addressrep] = bufferRead;
                WriteProcessMemory(ProcessHandle, (LPVOID)(addressscan), &bufferWrite, sizeof(bufferWrite), 0);
                modifiedValuesWrite2[addressscan] = bufferWrite;
            }
            notificationSystem.AddNotification("Notification", "Successfully applied", ImGui::GetColorU32(c::accent));
            Activado();

        }

        CloseHandle(ProcessHandle);
    }


    void SniperSwitch()
    {
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        DWORD_PTR startAddress = (DWORD_PTR)si.lpMinimumApplicationAddress;
        DWORD_PTR endAddress = (DWORD_PTR)si.lpMaximumApplicationAddress;

        if (!AttackProcess(GetEmulatorRunning()))
        {
            notificationSystem.AddNotification("Notification", "Emulator Not Found!", ImGui::GetColorU32(c::accent));
            MemoryLogs = "Emulator Not Found!";
            return;
        }

        MemoryLogs = "SniperScope : Applying";



        std::vector<BYTE> scan = { 0x3F, 0x00, 0x00, 0x80, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x20, 0x41, 0x00, 0x00, 0x34, 0x42, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F };
        std::vector<BYTE> replace = { 0x01, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x20, 0x41, 0x00, 0x00, 0x34, 0x42, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F };
        bool st = ReplacePattern(startAddress, endAddress, scan.data(), replace.data());

        if (st)
        {

            notificationSystem.AddNotification("Notification", "Successfully applied", ImGui::GetColorU32(c::accent));
            Activado();
            MemoryLogs = "SniperScope : Successfully Injected!";
        }
        else
        {
            MemoryLogs = "SniperScope : Failed To Apply!";
            notificationSystem.AddNotification("Notification", "Failed applied!", ImGui::GetColorU32(c::accent));
            desactivar();
        }

        CloseHandle(ProcessHandle);
    }

    void SniperSCOPE()
    {
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        DWORD_PTR startAddress = (DWORD_PTR)si.lpMinimumApplicationAddress;
        DWORD_PTR endAddress = (DWORD_PTR)si.lpMaximumApplicationAddress;

        if (!AttackProcess(GetEmulatorRunning()))
        {
            notificationSystem.AddNotification("Notification", "Emulator Not Found!", ImGui::GetColorU32(c::accent));
            MemoryLogs = "Emulator Not Found!";
            return;
        }

        MemoryLogs = "SniperScope : Applying";



        std::vector<BYTE> scan = { 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };
        std::vector<BYTE> replace = { 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        bool st = ReplacePattern(startAddress, endAddress, scan.data(), replace.data());

        if (st)
        {

            notificationSystem.AddNotification("Notification", "Successfully applied", ImGui::GetColorU32(c::accent));
            Activado();
            MemoryLogs = "SniperScope : Successfully Injected!";
        }
        else
        {
            MemoryLogs = "SniperScope : Failed To Apply!";
            notificationSystem.AddNotification("Notification", "Failed applied!", ImGui::GetColorU32(c::accent));
            desactivar();
        }

        CloseHandle(ProcessHandle);
    }

    void SCOPE2X()
    {
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        DWORD_PTR startAddress = (DWORD_PTR)si.lpMinimumApplicationAddress;
        DWORD_PTR endAddress = (DWORD_PTR)si.lpMaximumApplicationAddress;

        if (!AttackProcess(GetEmulatorRunning()))
        {
            notificationSystem.AddNotification("Notification", "Emulator Not Found!", ImGui::GetColorU32(c::accent));
            MemoryLogs = "Emulator Not Found!";
            return;
        }

        MemoryLogs = "SniperScope : Applying";



        std::vector<BYTE> scan = { 0x33, 0x33, 0x93, 0x3F, 0x8F, 0xC2, 0xF5, 0x3C, 0xCD, 0xCC, 0xCC, 0x3D, 0x02, 0x00, 0x00, 0x00, 0xEC, 0x51, 0xB8, 0x3D, 0xCD, 0xCC, 0x4C, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA0, 0x42, 0x00, 0x00, 0xC0, 0x3F, 0x33, 0x33, 0x13, 0x40, 0x00, 0x00, 0xF0, 0x3F, 0x00, 0x00, 0x80, 0x3F, 0x01, 0x00 };
        std::vector<BYTE> replace = { 0x33, 0x33, 0x93, 0x3F, 0x8F, 0xC2, 0xF5, 0x3C, 0xCD, 0xCC, 0xCC, 0x3D, 0x02, 0x00, 0x00, 0x00, 0xEC, 0x51, 0xB8, 0x3D, 0xCD, 0xCC, 0x4C, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA0, 0x42, 0x00, 0x00, 0xC0, 0x3F, 0x33, 0x33, 0x13, 0x40, 0x00, 0x00, 0xF0, 0x3F, 0x00, 0x00, 0x29, 0x5C, 0x01, 0x00 };
        bool st = ReplacePattern(startAddress, endAddress, scan.data(), replace.data());

        if (st)
        {

            notificationSystem.AddNotification("Notification", "Successfully applied", ImGui::GetColorU32(c::accent));
            Activado();
            MemoryLogs = "SniperScope : Successfully Injected!";
        }
        else
        {
            MemoryLogs = "SniperScope : Failed To Apply!";
            notificationSystem.AddNotification("Notification", "Failed applied!", ImGui::GetColorU32(c::accent));
            desactivar();
        }

        CloseHandle(ProcessHandle);
    }

    void SCOPE4X()
    {
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        DWORD_PTR startAddress = (DWORD_PTR)si.lpMinimumApplicationAddress;
        DWORD_PTR endAddress = (DWORD_PTR)si.lpMaximumApplicationAddress;

        if (!AttackProcess(GetEmulatorRunning()))
        {
            notificationSystem.AddNotification("Notification", "Emulator Not Found!", ImGui::GetColorU32(c::accent));
            MemoryLogs = "Emulator Not Found!";
            return;
        }

        MemoryLogs = "SniperScope : Applying";



        std::vector<BYTE> scan = { 0x20, 0x40, 0xCD, 0xCC, 0x8C, 0x3F, 0x8F, 0xC2, 0xF5, 0x3C, 0xCD, 0xCC, 0xCC, 0x3D, '?', 0x00, 0x00, 0x00, 0x29, 0x5C, 0x8F, 0x3D, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0xF0, 0x41, 0x00, 0x00, 0x48, 0x42, 0x00, 0x00, 0x00, 0x3F, 0x33, 0x33, 0x13, 0x40, 0x00, 0x00, 0xD0, 0x3F, 0x00, 0x00, 0x80, 0x3F, 0x01 };
        std::vector<BYTE> replace = { 0x20, 0x40, 0xCD, 0xCC, 0x8C, 0x3F, 0x8F, 0xC2, 0xF5, 0x3C, 0xCD, 0xCC, 0xCC, 0x3D, '?', 0x00, 0x00, 0x00, 0x29, 0x5C, 0x8F, 0x3D, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0xF0, 0x41, 0x00, 0x00, 0x48, 0x42, 0x00, 0x00, 0x00, 0x3F, 0x33, 0x33, 0x13, 0x40, 0x00, 0x00, 0xD0, 0x3F, 0x00, 0x00, 0x80, 0x5C, 0x01 };
        bool st = ReplacePattern(startAddress, endAddress, scan.data(), replace.data());

        if (st)
        {

            notificationSystem.AddNotification("Notification", "Successfully applied", ImGui::GetColorU32(c::accent));
            Activado();
            MemoryLogs = "SniperScope : Successfully Injected!";
        }
        else
        {
            MemoryLogs = "SniperScope : Failed To Apply!";
            notificationSystem.AddNotification("Notification", "Failed applied!", ImGui::GetColorU32(c::accent));
            desactivar();
        }

        CloseHandle(ProcessHandle);
    }

    void AWMLOCATION()
    {
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        DWORD_PTR startAddress = (DWORD_PTR)si.lpMinimumApplicationAddress;
        DWORD_PTR endAddress = (DWORD_PTR)si.lpMaximumApplicationAddress;

        if (!AttackProcess(GetEmulatorRunning()))
        {
            notificationSystem.AddNotification("Notification", "Emulator Not Found!", ImGui::GetColorU32(c::accent));
            MemoryLogs = "Emulator Not Found!";
            return;
        }

        MemoryLogs = "SniperScope : Applying";



        std::vector<BYTE> scan = { 0x20, 0x00, 0x00, 0x00, 0x69, 0x00, 0x6E, 0x00, 0x67, 0x00, 0x61, 0x00, 0x6D, 0x00, 0x65, 0x00, 0x2F, 0x00, 0x70, 0x00, 0x69, 0x00, 0x63, 0x00, 0x6B, 0x00, 0x75, 0x00, 0x70, 0x00, 0x2F, 0x00, 0x70, 0x00, 0x69, 0x00, 0x63, 0x00, 0x6B, 0x00, 0x75, 0x00, 0x70, 0x00, 0x5F, 0x00, 0x61, 0x00, 0x77, 0x00, 0x6D, 0x00, 0x5F, 0x00, 0x67, 0x00, 0x6F, 0x00, 0x6C, 0x00, 0x64, 0x00, 0xFF };
        std::vector<BYTE> replace = { 0x1D, 0x00, 0x00, 0x00, 0x65, 0x00, 0x66, 0x00, 0x66, 0x00, 0x65, 0x00, 0x63, 0x00, 0x74, 0x00, 0x73, 0x00, 0x2F, 0x00, 0x76, 0x00, 0x66, 0x00, 0x78, 0x00, 0x5F, 0x00, 0x69, 0x00, 0x6E, 0x00, 0x61, 0x00, 0x67, 0x00, 0x6D, 0x00, 0x65, 0x00, 0x5F, 0x00, 0x6C, 0x00, 0x61, 0x00, 0x73, 0x00, 0x65, 0x00, 0x72, 0x00, 0x5F, 0x00, 0x73, 0x00, 0x68, 0x00, 0x6F, 0x00, 0x70, 0x00 };
        bool st = ReplacePattern(startAddress, endAddress, scan.data(), replace.data());

        if (st)
        {

            notificationSystem.AddNotification("Notification", "Successfully applied", ImGui::GetColorU32(c::accent));
            Activado();
            MemoryLogs = "SniperScope : Successfully Injected!";
        }
        else
        {
            MemoryLogs = "SniperScope : Failed To Apply!";
            notificationSystem.AddNotification("Notification", "Failed applied!", ImGui::GetColorU32(c::accent));
            desactivar();
        }

        CloseHandle(ProcessHandle);
    }

    void NORECOIL()
    {
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        DWORD_PTR startAddress = (DWORD_PTR)si.lpMinimumApplicationAddress;
        DWORD_PTR endAddress = (DWORD_PTR)si.lpMaximumApplicationAddress;

        if (!AttackProcess(GetEmulatorRunning()))
        {
            notificationSystem.AddNotification("Notification", "Emulator Not Found!", ImGui::GetColorU32(c::accent));
            MemoryLogs = "Emulator Not Found!";
            return;
        }

        MemoryLogs = "SniperScope : Applying";



        std::vector<BYTE> scan = { 0x03, 0x0A, 0x9F, 0xED, 0x10, 0x0A, 0x01, 0xEE, 0x00, 0x0A, 0x81, 0xEE, 0x10, 0x0A, 0x10, 0xEE, 0x10, 0x8C, 0xBD, 0xE8, 0x00, 0x00, 0x7A, 0x44, 0xF0 };
        std::vector<BYTE> replace = { 0x03, 0x0A, 0x9F, 0xED, 0x10, 0x0A, 0x01, 0xEE, 0x00, 0x0A, 0x81, 0xEE, 0x10, 0x0A, 0x10, 0xEE, 0x10, 0x8C, 0xBD, 0xE8, 0x00, 0x00, 0x00, 0x00, 0xF0 };
        bool st = ReplacePattern(startAddress, endAddress, scan.data(), replace.data());

        if (st)
        {

            notificationSystem.AddNotification("Notification", "Successfully applied", ImGui::GetColorU32(c::accent));
            Activado();
            MemoryLogs = "SniperScope : Successfully Injected!";
        }
        else
        {
            MemoryLogs = "SniperScope : Failed To Apply!";
            notificationSystem.AddNotification("Notification", "Failed applied!", ImGui::GetColorU32(c::accent));
            desactivar();
        }

        CloseHandle(ProcessHandle);
    }

    void GLITCH_FIRE()
    {
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        DWORD_PTR startAddress = (DWORD_PTR)si.lpMinimumApplicationAddress;
        DWORD_PTR endAddress = (DWORD_PTR)si.lpMaximumApplicationAddress;

        if (!AttackProcess(GetEmulatorRunning()))
        {
            notificationSystem.AddNotification("Notification", "Emulator Not Found!", ImGui::GetColorU32(c::accent));
            MemoryLogs = "Emulator Not Found!";
            return;
        }

        MemoryLogs = "SniperScope : Applying";



        std::vector<BYTE> scan = { 0xC0, 0x3F, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x80, 0x3F, 0x00 };
        std::vector<BYTE> replace = { 0x72, 0xA0, 0xE3, 0x1E, 0xFF, 0xE1 };
        bool st = ReplacePattern(startAddress, endAddress, scan.data(), replace.data());

        if (st)
        {

            notificationSystem.AddNotification("Notification", "Successfully applied", ImGui::GetColorU32(c::accent));
            Activado();
            MemoryLogs = "SniperScope : Successfully Injected!";
        }
        else
        {
            MemoryLogs = "SniperScope : Failed To Apply!";
            notificationSystem.AddNotification("Notification", "Failed applied!", ImGui::GetColorU32(c::accent));
            desactivar();
        }

        CloseHandle(ProcessHandle);
    }

    void restgoost()
    {
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        DWORD_PTR startAddress = (DWORD_PTR)si.lpMinimumApplicationAddress;
        DWORD_PTR endAddress = (DWORD_PTR)si.lpMaximumApplicationAddress;

        if (!AttackProcess(GetEmulatorRunning()))
        {
            notificationSystem.AddNotification("Notification", "Emulator Not Found!", ImGui::GetColorU32(c::accent));
            MemoryLogs = "Emulator Not Found!";
            return;
        }

        MemoryLogs = "SniperScope : Applying";



        std::vector<BYTE> scan = { 0x10, 0x4C, 0x2D, 0xE9, 0x08, 0xB0, 0x8D, 0xE2, 0x0C, 0x01, 0x9F, 0xE5, 0x00, 0x00, 0x8F, 0xE0 };
        std::vector<BYTE> replace = { 0x01, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1, 0x0C, 0x01, 0x9F, 0xE5, 0x00, 0x00, 0x8F, 0xE0 };
        bool st = ReplacePattern(startAddress, endAddress, scan.data(), replace.data());

        if (st)
        {

            notificationSystem.AddNotification("Notification", "Successfully applied", ImGui::GetColorU32(c::accent));
            Activado();
            MemoryLogs = "SniperScope : Successfully Injected!";
        }
        else
        {
            MemoryLogs = "SniperScope : Failed To Apply!";
            notificationSystem.AddNotification("Notification", "Failed applied!", ImGui::GetColorU32(c::accent));
            desactivar();
        }

        CloseHandle(ProcessHandle);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // instanly truns off the aimbot
    void Restore() {
        if (!AttackProcess(GetEmulatorRunning())) {
            return;
        }
        for (const auto& entry : originalValuesWrite) {
            DWORD_PTR address = entry.first;
            int originalValue = entry.second;
            WriteProcessMemory(ProcessHandle, (LPVOID)address, &originalValue, sizeof(originalValue), NULL);
        }
        for (const auto& entry : originalValuesWrite2) {
            DWORD_PTR address = entry.first;
            int originalValue = entry.second;
            WriteProcessMemory(ProcessHandle, (LPVOID)address, &originalValue, sizeof(originalValue), NULL);
        }
        // notificationSystem.Notification("1", "Notification", "Aimbot Deactivated ! ", main_color);
        CloseHandle(ProcessHandle);
    }
    // instantly turns on the aimbot
    void Reapply() {
        if (!AttackProcess(GetEmulatorRunning())) {
            return;
        }
        for (const auto& entry : modifiedValuesWrite) {
            DWORD_PTR address = entry.first;
            int valueToReapply = entry.second;
            WriteProcessMemory(ProcessHandle, (LPVOID)address, &valueToReapply, sizeof(valueToReapply), NULL);
        }
        for (const auto& entry : modifiedValuesWrite2) {
            DWORD_PTR address = entry.first;
            int valueToReapply = entry.second;
            WriteProcessMemory(ProcessHandle, (LPVOID)address, &valueToReapply, sizeof(valueToReapply), NULL);
        }
        // notificationSystem.Notification("1", "Notification", "Aimbot Activated ! ", ImColor(0, 255, 0));
        CloseHandle(ProcessHandle);

    }





    void ReWrite(std::string type, DWORD_PTR dwStartRange, DWORD_PTR dwEndRange, BYTE* Search, BYTE* Replace)
    {
        if (!AttackProcess(GetEmulatorRunning()))
            MemoryLogs = "Panel : Error!";

        bool Status = ReplacePattern(dwStartRange, dwEndRange, Search, Replace, true);
        if (Status)

            MemoryLogs = "Panel : Error!";

        else

            MemoryLogs = "Panel : Error!";

        CloseHandle(ProcessHandle);
    }





    void deWrite(std::string type, DWORD_PTR dwStartRange, DWORD_PTR dwEndRange, BYTE* Search, BYTE* Replace)
    {
        if (!AttackProcess(GetEmulatorRunning()))
            MemoryLogs = "Panel : Error";

        bool Status = ReplacePattern(dwStartRange, dwEndRange, Search, Replace, true);
        if (Status)
            MemoryLogs = "Panel : Error";
        else
            MemoryLogs = "Panel : Error";

        CloseHandle(ProcessHandle);
    }

    DWORD ProcessId = 0;
    HANDLE ProcessHandle;

    typedef struct _MEMORY_REGION
    {
        DWORD_PTR dwBaseAddr;
        DWORD_PTR dwMemorySize;
    }MEMORY_REGION;

    int GetPid(const char* procname)
    {

        if (procname == NULL)
            return 0;
        DWORD pid = 0;
        DWORD threadCount = 0;

        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32 pe;

        pe.dwSize = sizeof(PROCESSENTRY32);
        Process32First(hSnap, &pe);
        while (Process32Next(hSnap, &pe)) {
            if (_tcsicmp(pe.szExeFile, procname) == 0)
            {
                if ((int)pe.cntThreads > threadCount)
                {
                    threadCount = pe.cntThreads;

                    pid = pe.th32ProcessID;

                }
            }
        }
        return pid;
    }
    BOOL AttackProcess(const char* procname)
    {
        DWORD ProcId = GetPid(procname);
        if (ProcId == 0)
            return false;

        ProcessId = ProcId;
        ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, ProcessId);
        return ProcessHandle != nullptr;
    }

    bool ChangeProtection(ULONG Address, size_t size, DWORD NewProtect, DWORD& OldProtect)
    {
        return VirtualProtectEx(ProcessHandle, (LPVOID)Address, size, NewProtect, &OldProtect);;
    }

    bool ReplacePattern(DWORD_PTR dwStartRange, DWORD_PTR dwEndRange, BYTE* SearchAob, BYTE* ReplaceAob, bool ForceWrite = false)
    {
        int RepByteSize = _msize(ReplaceAob);
        if (RepByteSize <= 0) return false;
        std::vector<DWORD_PTR> foundedAddress;
        FindPattern(dwStartRange, dwEndRange, SearchAob, foundedAddress);
        if (foundedAddress.empty())
            return false;

        OutputDebugStringA(std::to_string(foundedAddress.size()).c_str());

        DWORD OldProtect;
        for (int i = 0; i < foundedAddress.size(); i++)
        {
            ChangeProtection(foundedAddress[i], RepByteSize, PAGE_EXECUTE_READWRITE, OldProtect);
            WriteProcessMemory(ProcessHandle, (LPVOID)foundedAddress[i], ReplaceAob, RepByteSize, 0);
        }

        return true;
    }


    bool FindPattern(DWORD_PTR StartRange, DWORD_PTR EndRange, BYTE* SearchBytes, std::vector<DWORD_PTR>& AddressRet) {
        MEMORY_BASIC_INFORMATION mbi;
        DWORD_PTR dwAddress = StartRange;
        DWORD_PTR nSearchSize = _msize(SearchBytes);

        std::vector<MEMORY_REGION> memoryRegions;

        // Collect memory regions
        while (VirtualQueryEx(ProcessHandle, (LPCVOID)dwAddress, &mbi, sizeof(mbi)) && dwAddress < EndRange) {
            if ((mbi.State == MEM_COMMIT) && !(mbi.Protect & PAGE_GUARD) && mbi.Protect != PAGE_NOACCESS) {
                memoryRegions.push_back({ (DWORD_PTR)mbi.BaseAddress, mbi.RegionSize });
            }
            dwAddress = (DWORD_PTR)mbi.BaseAddress + mbi.RegionSize;
        }

        std::mutex mtx;
        auto processRegion = [&](MEMORY_REGION region) {
            std::unique_ptr<BYTE[]> memoryData(new BYTE[region.dwMemorySize]);
            SIZE_T bytesRead = 0;

            if (ReadProcessMemory(ProcessHandle, (LPCVOID)region.dwBaseAddr, memoryData.get(), region.dwMemorySize, &bytesRead) && bytesRead > 0) {
                DWORD_PTR offset = 0;
                int matchOffset;
                while ((matchOffset = Memfind(memoryData.get() + offset, bytesRead - offset, SearchBytes, nSearchSize)) != -1) {
                    std::lock_guard<std::mutex> lock(mtx);
                    AddressRet.push_back(region.dwBaseAddr + offset + matchOffset);
                    offset += matchOffset + nSearchSize;
                }
            }
            };

        // Launch threads for each memory region
        std::vector<std::future<void>> futures;
        for (const auto& region : memoryRegions) {
            futures.push_back(std::async(std::launch::async, processRegion, region));
        }

        // Wait for all threads to finish
        for (auto& fut : futures) {
            fut.get();
        }

        return true;
    }

    bool SlowFindPattern(DWORD_PTR StartRange, DWORD_PTR EndRange, BYTE* SearchBytes, std::vector<DWORD_PTR>& AddressRet)
    {

        BYTE* pCurrMemoryData = NULL;
        MEMORY_BASIC_INFORMATION	mbi;
        std::vector<MEMORY_REGION> m_vMemoryRegion;
        mbi.RegionSize = 0x1000;



        DWORD_PTR dwAddress = StartRange;
        DWORD_PTR nSearchSize = _msize(SearchBytes);


        while (VirtualQueryEx(ProcessHandle, (LPCVOID)dwAddress, &mbi, sizeof(mbi)) && (dwAddress < EndRange) && ((dwAddress + mbi.RegionSize) > dwAddress))
        {

            if ((mbi.State == MEM_COMMIT) && ((mbi.Protect & PAGE_GUARD) == 0) && (mbi.Protect != PAGE_NOACCESS) && ((mbi.AllocationProtect & PAGE_NOCACHE) != PAGE_NOCACHE))
            {

                MEMORY_REGION mData = { 0 };
                mData.dwBaseAddr = (DWORD_PTR)mbi.BaseAddress;
                mData.dwMemorySize = mbi.RegionSize;
                m_vMemoryRegion.push_back(mData);

            }
            dwAddress = (DWORD_PTR)mbi.BaseAddress + mbi.RegionSize;

        }

        std::vector<MEMORY_REGION>::iterator it;
        for (it = m_vMemoryRegion.begin(); it != m_vMemoryRegion.end(); it++)
        {
            MEMORY_REGION mData = *it;


            DWORD_PTR dwNumberOfBytesRead = 0;
            pCurrMemoryData = new BYTE[mData.dwMemorySize];
            ZeroMemory(pCurrMemoryData, mData.dwMemorySize);
            ZwReadVirtualMemory(ProcessHandle, (LPVOID)mData.dwBaseAddr, pCurrMemoryData, mData.dwMemorySize, &dwNumberOfBytesRead);
            if ((int)dwNumberOfBytesRead <= 0)
            {
                delete[] pCurrMemoryData;
                continue;
            }
            DWORD_PTR dwOffset = 0;
            int iOffset = Memfind(pCurrMemoryData, dwNumberOfBytesRead, SearchBytes, nSearchSize);
            while (iOffset != -1)
            {
                dwOffset += iOffset;
                AddressRet.push_back(dwOffset + mData.dwBaseAddr);
                dwOffset += nSearchSize;
                iOffset = Memfind(pCurrMemoryData + dwOffset, dwNumberOfBytesRead - dwOffset - nSearchSize, SearchBytes, nSearchSize);
            }

            if (pCurrMemoryData != NULL)
            {
                delete[] pCurrMemoryData;
                pCurrMemoryData = NULL;
            }

        }
        return TRUE;
    }

    bool FastFindPattern(DWORD_PTR StartRange, DWORD_PTR EndRange, BYTE* SearchBytes, std::vector<DWORD_PTR>& AddressRet) {
        MEMORY_BASIC_INFORMATION mbi;
        mbi.RegionSize = 0x1000;
        DWORD_PTR dwAddress = StartRange;
        DWORD_PTR nSearchSize = _msize(SearchBytes);

        std::vector<MEMORY_REGION> m_vMemoryRegion;

        // Collect all memory regions
        while (VirtualQueryEx(ProcessHandle, (LPCVOID)dwAddress, &mbi, sizeof(mbi)) && (dwAddress < EndRange) && ((dwAddress + mbi.RegionSize) > dwAddress)) {
            if ((mbi.State == MEM_COMMIT) && ((mbi.Protect & PAGE_GUARD) == 0) && (mbi.Protect != PAGE_NOACCESS) && ((mbi.AllocationProtect & PAGE_NOCACHE) != PAGE_NOCACHE)) {
                MEMORY_REGION mData = { (DWORD_PTR)mbi.BaseAddress, mbi.RegionSize };
                m_vMemoryRegion.push_back(mData);
            }
            dwAddress = (DWORD_PTR)mbi.BaseAddress + mbi.RegionSize;
        }

        // Mutex for synchronizing access to AddressRet and modifiedAoBs
        std::mutex mtx;

        // Lambda to process each memory region in a separate thread
        auto processRegion = [&](MEMORY_REGION mData) {
            BYTE* pCurrMemoryData = new BYTE[mData.dwMemorySize];
            ZeroMemory(pCurrMemoryData, mData.dwMemorySize);
            DWORD_PTR dwNumberOfBytesRead = 0;
            ZwReadVirtualMemory(ProcessHandle, (LPVOID)mData.dwBaseAddr, pCurrMemoryData, mData.dwMemorySize, &dwNumberOfBytesRead);

            if ((int)dwNumberOfBytesRead > 0) {
                DWORD_PTR dwOffset = 0;
                int iOffset = Memfind(pCurrMemoryData, dwNumberOfBytesRead, SearchBytes, nSearchSize);
                while (iOffset != -1) {
                    dwOffset += iOffset;
                    DWORD_PTR firstByteAddress = dwOffset + mData.dwBaseAddr;

                    std::lock_guard<std::mutex> lock(mtx);
                    // Check if the address has already been modified
                    if (modifiedAoBs.find(firstByteAddress) == modifiedAoBs.end()) {
                        // Address has not been modified, add it to the list and the map
                        AddressRet.push_back(firstByteAddress);
                        modifiedAoBs[firstByteAddress] = 1;  // Mark it as modified
                    }

                    dwOffset += nSearchSize;
                    iOffset = Memfind(pCurrMemoryData + dwOffset, dwNumberOfBytesRead - dwOffset - nSearchSize, SearchBytes, nSearchSize);
                }
            }
            delete[] pCurrMemoryData;
            };

        // Launch threads to process memory regions concurrently
        std::vector<std::future<void>> futures;
        for (auto& region : m_vMemoryRegion) {
            futures.push_back(std::async(std::launch::async, processRegion, region));
        }

        // Wait for all threads to complete
        for (auto& fut : futures) {
            fut.get();
        }

        return true;
    }





    int Memfind(BYTE* buffer, DWORD_PTR dwBufferSize, BYTE* bstr, DWORD_PTR dwStrLen)
    {
        if (dwBufferSize < 0)
        {
            return -1;
        }
        DWORD_PTR  i, j;
        for (i = 0; i < dwBufferSize; i++)
        {
            for (j = 0; j < dwStrLen; j++)
            {
                if (buffer[i + j] != bstr[j] && bstr[j] != '?')
                    break;

            }
            if (j == dwStrLen)
                return i;
        }
        return -1;
    }


    static int findMyProc(const char* procname) {
        if (procname == NULL)
            return 0;
        DWORD pid = 0;
        DWORD threadCount = 0;

        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32 pe;

        pe.dwSize = sizeof(PROCESSENTRY32);
        Process32First(hSnap, &pe);
        while (Process32Next(hSnap, &pe)) {
            if (_tcsicmp(pe.szExeFile, procname) == 0) {
                if ((int)pe.cntThreads > threadCount) {
                    threadCount = pe.cntThreads;

                    pid = pe.th32ProcessID;

                }
            }
        }
        return pid;
    }
};