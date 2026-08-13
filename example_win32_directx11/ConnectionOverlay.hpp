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
inline int g_SelectedGameVersion = 0; // 0 = Free Fire Max, 1 = Free Fire

inline void DrawLoadingSpinner(ImDrawList* drawList, ImVec2 center, float radius, ImU32 color, ImU32 cloudColor, float thickness, float speed = 3.5f) {
    static float angle = 0.0f;
    angle += ImGui::GetIO().DeltaTime * speed;
    if (angle > 3.14159f * 2.0f) angle -= 3.14159f * 2.0f;

    int num_segments = 30;

    // Draw central cloud shape
    float cr = radius * 0.45f;
    ImVec2 c1 = center + ImVec2(-cr * 0.6f, cr * 0.3f);
    ImVec2 c2 = center + ImVec2(cr * 0.5f, cr * 0.1f);
    ImVec2 c3 = center + ImVec2(0.0f, -cr * 0.4f);
    drawList->AddCircleFilled(c1, cr * 0.6f, cloudColor, 20);
    drawList->AddCircleFilled(c2, cr * 0.7f, cloudColor, 20);
    drawList->AddCircleFilled(c3, cr * 0.8f, cloudColor, 20);
    drawList->AddRectFilled(c1 + ImVec2(0.f, -cr * 0.2f), c2 + ImVec2(0.f, cr * 0.7f), cloudColor, 0.f);

    // Draw two rotating arcs (sync effect)
    float start_angle1 = angle;
    float end_angle1 = angle + 3.14159f * 0.7f; 
    float start_angle2 = angle + 3.14159f;
    float end_angle2 = start_angle2 + 3.14159f * 0.7f;

    drawList->PathClear();
    drawList->PathArcTo(center, radius, start_angle1, end_angle1, num_segments);
    drawList->PathStroke(color, false, thickness);

    drawList->PathClear();
    drawList->PathArcTo(center, radius, start_angle2, end_angle2, num_segments);
    drawList->PathStroke(color, false, thickness);

    // Draw Arrow heads
    auto drawArrow = [&](float end_ang) {
        ImVec2 p1 = center + ImVec2(cosf(end_ang + 0.1f) * radius, sinf(end_ang + 0.1f) * radius);
        ImVec2 p2 = center + ImVec2(cosf(end_ang - 0.15f) * (radius - 5.f), sinf(end_ang - 0.15f) * (radius - 5.f));
        ImVec2 p3 = center + ImVec2(cosf(end_ang - 0.15f) * (radius + 5.f), sinf(end_ang - 0.15f) * (radius + 5.f));
        drawList->AddTriangleFilled(p1, p2, p3, color);
    };
    
    drawArrow(end_angle1);
    drawArrow(end_angle2);
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

    bool isInstalled = IsPackageInstalledInEmulator(adb, target, "com.mamun");
    if (isInstalled && !downloadedNewApk) {
        g_CurrentStepLog = "[3/6] APK already in emulator & up-to-date. Skipping install!";
    } else {
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
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // Step 4: Auto Launching APK & Selected Game Instantly
    g_ConnectionStep = 4;
    std::string gameName = (g_SelectedGameVersion == 0) ? "Free Fire Max" : "Free Fire";
    g_CurrentStepLog = "[4/6] Auto-launching APK & " + gameName + "...";
    RunSilentCommand(adb + target + "shell am start -n com.mamun/.MainActivity --ez LAUNCHED_FROM_EXE true");
    
    if (g_SelectedGameVersion == 0) {
        RunSilentCommand(adb + target + "shell am start -n com.dts.freefiremax/com.dts.freefireth.FFMainActivity");
    } else {
        RunSilentCommand(adb + target + "shell am start -n com.dts.freefireth/com.dts.freefireth.FFMainActivity");
    }

    // Step 5: Socket Bridge Setup (Port 8888)
    g_ConnectionStep = 5;
    g_CurrentStepLog = "[5/6] Establishing TCP Socket Bridge (Port 8888)...";
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
