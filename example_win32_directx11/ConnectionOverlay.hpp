#pragma once
#include <imgui.h>
#include <imgui_internal.h>
#include <atomic>
#include <string>
#include <chrono>
#include <thread>
#include "EmulatorDetector.hpp"
#include "GitHubDownloader.hpp"
#include "TCPClientBridge.hpp"

inline std::atomic<int> g_ConnectionStep{ 0 };
inline std::atomic<bool> g_ConnectionStarted{ false };
inline std::atomic<bool> g_ConnectionDone{ false };
inline std::atomic<bool> g_TransitionToMenu{ false };
inline std::string g_CurrentStepLog = "Click CONNECT to initialize bridge system";
inline std::string g_DetectedEmulatorName = "None";

inline void DrawLoadingSpinner(ImDrawList* drawList, ImVec2 center, float radius, ImU32 color, float thickness, float speed = 3.5f) {
    static float angle = 0.0f;
    angle += ImGui::GetIO().DeltaTime * speed;
    if (angle > 3.14159f * 2.0f) angle -= 3.14159f * 2.0f;

    int num_segments = 30;
    float start_angle = angle;
    float end_angle = angle + 3.14159f * 1.3f; // Capsule arc length

    drawList->PathClear();
    drawList->PathArcTo(center, radius, start_angle, end_angle, num_segments);
    drawList->PathStroke(color, false, thickness);
}

inline void ExecuteBridgeConnectSequence() {
    g_ConnectionStarted = true;

    // Step 1: Smart Downloading APK with Version Check
    g_ConnectionStep = 1;
    g_CurrentStepLog = "[1/6] Checking for APK updates on GitHub...";
    std::string currentVer = "1.0";
    bool downloadedNewApk = SmartDownloadApkFromGitHub(currentVer);
    
    if (downloadedNewApk) {
        g_CurrentStepLog = "[1/6] Downloaded new APK (v" + currentVer + ") from GitHub!";
    } else {
        g_CurrentStepLog = "[1/6] APK is up-to-date (v" + currentVer + "). Skipping download!";
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(800));

    // Step 2: Emulator Detection
    g_ConnectionStep = 2;
    g_CurrentStepLog = "[2/6] Detecting running emulator (BlueStacks / MSI)...";
    EmulatorInfo emu = DetectRunningEmulator();
    g_DetectedEmulatorName = emu.name;
    std::string adb = "\"" + emu.adbExePath + "\"";
    std::string target = " -s 127.0.0.1:" + std::to_string(emu.adbPort) + " ";
    g_ActiveAdbCmd = adb;
    g_ActiveAdbTarget = target;
    std::this_thread::sleep_for(std::chrono::milliseconds(800));

    // Step 3: ADB Installing & Pulling APK
    g_ConnectionStep = 3;
    g_CurrentStepLog = "[3/6] Connecting ADB to " + emu.name + "...";
    std::string connectCmd = adb + " connect 127.0.0.1:" + std::to_string(emu.adbPort);
    RunSilentCommand(connectCmd, 10000);
    RunSilentCommand("hd-adb.exe connect 127.0.0.1:5555", 10000);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    g_CurrentStepLog = "[3/6] Installing APK into " + emu.name + "...";
    std::string tempApk = GetTempApkFilePath();

    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(tempApk.c_str(), &findData);
    bool tempApkExists = (hFind != INVALID_HANDLE_VALUE);
    if (hFind != INVALID_HANDLE_VALUE) FindClose(hFind);

    if (!tempApkExists) {
        g_CurrentStepLog = "[3/6] Temp APK missing, downloading fresh copy...";
        DownloadApkFromGitHub();
    }

    std::string installCmd = adb + target + "install -r -g \"" + tempApk + "\"";
    bool installed = RunSilentCommand(installCmd, 45000);
    if (!installed) {
        installed = RunSilentCommand(adb + " install -r -g \"" + tempApk + "\"", 45000);
    }
    if (!installed) {
        installed = RunSilentCommand("hd-adb.exe install -r -g \"" + tempApk + "\"", 45000);
    }
    if (!installed) {
        RunSilentCommand("adb.exe install -r -g \"" + tempApk + "\"", 45000);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // Step 4: Auto Launching FF & APK
    g_ConnectionStep = 4;
    g_CurrentStepLog = "[4/6] Auto-launching APK...";
    RunSilentCommand(adb + target + "shell am start -n com.mamun/.MainActivity");
    
    // Wait for 5 seconds to let APK initialize and show its UI/Permissions
    for (int i = 5; i >= 1; i--) {
        g_CurrentStepLog = "[4/6] Waiting for APK " + std::to_string(i) + "s...";
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    g_CurrentStepLog = "[4/6] Auto-launching Free Fire Game...";
    RunSilentCommand(adb + target + "shell am start -n com.dts.freefireth/com.dts.freefireth.FFMainActivity");
    RunSilentCommand(adb + target + "shell am start -n com.dts.freefiremax/com.dts.freefireth.FFMainActivity");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // Step 5: 5s Safety Delay
    g_ConnectionStep = 5;
    for (int i = 5; i >= 1; i--) {
        g_CurrentStepLog = "[5/6] System initialized. Delay " + std::to_string(i) + "s for memory setup...";
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    // Step 6: Socket Bridge Connection Handshake
    g_ConnectionStep = 6;
    g_CurrentStepLog = "[6/6] Establishing TCP Socket Bridge (Port 8888)...";
    RunSilentCommand(adb + target + "forward tcp:8888 tcp:8888");
    RunSilentCommand(adb + " forward tcp:8888 tcp:8888");
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    static bool socketThreadStarted = false;
    if (!socketThreadStarted) {
        std::thread(ConnectSocketThread).detach();
        socketThreadStarted = true;
    }

    int retries = 0;
    while (!g_BridgeConnected.load() && retries < 30) {
        g_CurrentStepLog = "[6/6] Waiting for APK to inject (Retry " + std::to_string(retries) + "/30)...";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        retries++;
    }

    if (g_BridgeConnected.load()) {
        g_ConnectionDone = true;
        g_CurrentStepLog = "CONNECTED DONE!";
        
        // Wait exact 1.0 second before auto-opening Main Menu
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        g_TransitionToMenu = true;
    } else {
        g_ConnectionDone = false;
        g_ConnectionStarted = false;
        g_CurrentStepLog = "[FAILED] Socket bridge connection timeout! Retry CONNECT.";
    }
}
