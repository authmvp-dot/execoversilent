#pragma once
#include <functional>
#include <atomic>

inline std::atomic<bool> g_AdbReady{ true };
inline std::atomic<bool> g_AdbFailed{ false };

inline void Backend_StartUtilityThread() {}
inline void Backend_SyncKeybindsFromYorzen() {}
inline void Backend_SyncEspPreview() {}
inline void Backend_RenderNotifications() {}
inline void Backend_Notify(const char*, bool = true) {}
inline void Backend_RunTempCleaner() {}
inline void Backend_ExitPanel() {}
inline void Backend_RunAdbInit() {}

inline void BlazeMemOnCheckboxToggled(int, const char*, const std::function<void()>&, const std::function<void()>&) {}
inline void BlazeMemRunLoad(const char*, const std::function<void()>&) {}

inline bool& Backend_BlazeCheckbox(int) {
    static bool dummy = false;
    return dummy;
}

inline void SyncUIToGlobals() {}
inline void Backend_LoadConfig() {}
inline void Backend_SaveConfig() {}
