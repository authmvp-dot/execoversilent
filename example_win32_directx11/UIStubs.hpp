#pragma once
#include <functional>
#include <atomic>
#include <d3d11.h>
#include <AIMBOTMEMORY.H>
#include "TCPClientBridge.hpp"

inline std::atomic<bool> g_AdbReady{ false };
inline std::atomic<bool> g_AdbFailed{ false };

inline void Backend_StartUtilityThread() {}
inline void Backend_SyncKeybindsFromYorzen() {}
inline void Backend_SyncEspPreview() {}
inline void Backend_RenderNotifications() {}
inline void Backend_Notify(const char*, bool = true) {}
inline void Backend_RunTempCleaner() {}
inline void Backend_ExitPanel() {}

inline void Backend_RunAdbInit() {
    g_AdbFailed = false;
    
    // Connect ADB and set up port forwarding
    RunSilentCommand("hd-adb.exe connect 127.0.0.1:5555");
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    RunSilentCommand("hd-adb.exe forward tcp:8888 tcp:8888");
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Start background socket connection thread if not already running
    static bool socketThreadStarted = false;
    if (!socketThreadStarted) {
        std::thread(ConnectSocketThread).detach();
        socketThreadStarted = true;
    }

    // Wait for connection to succeed (up to 5 seconds)
    int retries = 0;
    while (!g_BridgeConnected.load() && retries < 10) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        retries++;
    }

    if (g_BridgeConnected.load()) {
        g_AdbReady = true;
        g_AdbFailed = false;
    } else {
        // Fallback or report fail
        g_AdbReady = false;
        g_AdbFailed = true;
    }
}

inline void BlazeMemOnCheckboxToggled(int, const char*, const std::function<void()>&, const std::function<void()>&) {}
inline void BlazeMemRunLoad(const char*, const std::function<void()>&) {}

inline bool& Backend_BlazeCheckbox(int) {
    static bool dummy = false;
    return dummy;
}

inline void SyncUIToGlobals() {}
inline void Backend_LoadConfig() {}
inline void Backend_SaveConfig() {}
inline void Backend_ResetConfig() {}

// Global AimbotMemory instance
inline AimbotMemory Aim;

// keybind_menu_key
inline int keybind_menu_key = 0;

// texture::custom_logo
namespace texture {
    inline ID3D11ShaderResourceView* custom_logo = nullptr;
}

// FlyHack_LocalPlayer stubs
namespace FlyHack_LocalPlayer {
    inline void Start() {}
    inline void Stop() {}
    inline void ApplyFly() {}
}

// NoGravityFly stubs
namespace NoGravityFly {
    inline void Start() {}
    inline void Stop() {}
}

// SpinPlayer stubs
namespace SpinPlayer {
    inline void Start() {}
    inline void Stop() {}
}
