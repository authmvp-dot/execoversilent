#include "BackendBridge.hpp"
#include "SyncGlobals.hpp"
#include <src/adb/adb.hpp>
#include <src/Globals.hpp>
#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")
#include <BlazeMem.h>
#include <src/Overlay/Overlay.hpp>
#include <EspLines/Memory/Memory.hpp>
#include <AIMBOTMEMORY.H>
#include <notifications.h>
#include <Yorzen/Dudas/ui_settings.hpp>
#include <ImGui/font_defines.h>
#include <imgui_settings.h>

inline namespace YorzenKey {
    extern bool RefreshEspCheck;
    extern bool AimbotToggleCheck;
    extern bool AimbotLegitCheck;
    extern bool AimbotExtCheck;
    extern bool SilentAimCheck;
    extern bool EnemyPullCheck;
    extern bool StreamerModeCheck;
    extern bool WukongModeCheck;
    extern bool SniperScopeCheck;
    extern bool HideMenuCheck;
    extern bool ClosedCheck;
    extern int AimbotToggleKey;
    extern int AimbotLegitKey;
    extern int AimbotExtKey;
    extern int SilentAimKey;
    extern int EnemyPullKey;
    extern bool TeleportKillCheck;
    extern int TeleportKillKey;
    extern bool WallHack1Check;
    extern int WallHack1Key;
    extern bool WallHack2Check;
    extern int WallHack2Key;
    extern bool CamaraJipiCheck;
    extern int CamaraJipiKey;
    extern bool SniperMacroCheck;
    extern int SniperMacroKey;
    extern bool FakeLagCheck;
    extern int FakeLagKey;
    extern int HideMenuKey;
    extern int RefreshEspKey;
    extern int StreamerModeKey;
    extern int WukongModeKey;
    extern int SniperScopeKey;
    extern int ClosedKey;
}

#include <atomic>
#include <chrono>
#include <functional>
#include <string>
#include <thread>

extern std::string MemoryLogs;
extern AimbotMemory Aim;
extern AimMemory SingleScanAim;

static bool s_blaze_checkboxes[9999] = {};
static CNotifications p_notif;
static std::atomic<bool> s_utility_started{ false };

bool& Backend_BlazeCheckbox(int id) {
    if (id < 0 || id >= 9999) id = 0;
    return s_blaze_checkboxes[id];
}

static bool BlazeMemLogFailed() {
    const std::string& log = MemoryLogs;
    if (log.empty()) return true;
    auto has = [&](const char* s) { return log.find(s) != std::string::npos; };
    return has("Emulator Not Found") || has("Failed To Apply") || has("Failed To Remove") ||
        has("Failed to Enable") || has("Failed to Disable") || has("Failed") || has("failed") ||
        has("not found") || has("Not Found") || has("Pattern Not Found") ||
        has("Pattern search failed") || has("search failed") || has("attach failed") ||
        has("OpenProcess failed") || has("Address Not Found") || has("load pattern first") ||
        has("unexpected error");
}

static bool BlazeMemEnableOk() {
    if (BlazeMemLogFailed()) return false;
    const std::string& log = MemoryLogs;
    auto has = [&](const char* s) { return log.find(s) != std::string::npos; };
    if (has(": Applying") || has("Scan: Scanning") || has("Scanning...") || has("Scanning!"))
        return false;
    return has("Successfully Injected") || has("Successfully applied") ||
        has("Activated Successfully") || has(" - Enabled!") || has("Enabled!");
}

static bool BlazeMemDisableOk() {
    if (BlazeMemLogFailed()) return false;
    const std::string& log = MemoryLogs;
    auto has = [&](const char* s) { return log.find(s) != std::string::npos; };
    if (has(": Removing") || has("Reverting...")) return false;
    return has("Successfully Removed") || has("Deactivated Successfully") ||
        has("Successfully Removed..") || has(" - Disabled!") || has("Disabled Successfully");
}

static bool BlazeMemLoadOk() {
    if (BlazeMemLogFailed()) return false;
    const std::string& log = MemoryLogs;
    auto has = [&](const char* s) { return log.find(s) != std::string::npos; };
    if (has("Scan: Scanning") || has("Scanning...")) return false;
    return has("Loaded Successfully") || has("Scan Results Found");
}

static bool BlazeMemEnsureAttached() {
    Aim.SetEmulatorTargetWindow(FWork::Overlay::GetTargetWindow());
    return Aim.AttackProcess(Aim.GetEmulatorRunning()) != FALSE;
}

static void BlazeMemRunToggle(int checkboxId, bool enabling, const char* label,
    const std::function<void()>& onEnable, const std::function<void()>& onDisable)
{
    std::thread([checkboxId, enabling, label, onEnable, onDisable]() {
        if (enabling) {
            s_blaze_checkboxes[checkboxId] = true;
            p_notif.AddMessage((std::string(label) + " Enabled").c_str(), ICON_BOMB_FILL, c::main_color);
        }
        else {
            s_blaze_checkboxes[checkboxId] = false;
            p_notif.AddMessage((std::string(label) + " Disabled").c_str(), ICON_BOMB_FILL, ImColor(255, 80, 80, 255));
        }
    }).detach();
}

void BlazeMemOnCheckboxToggled(int checkboxId, const char* label,
    const std::function<void()>& onEnable, const std::function<void()>& onDisable)
{
    BlazeMemRunToggle(checkboxId, s_blaze_checkboxes[checkboxId], label, onEnable, onDisable);
}

void BlazeMemRunLoad(const char* label, const std::function<void()>& onLoad)
{
    std::thread([label, onLoad]() {
        p_notif.AddMessage((std::string(label) + " Loaded").c_str(), ICON_BOMB_FILL, c::main_color);
    }).detach();
}

static void BlazeMemHotkeyToggle(int checkboxId, const char* label,
    const std::function<void()>& onEnable, const std::function<void()>& onDisable)
{
    const bool wantOn = !s_blaze_checkboxes[checkboxId];
    s_blaze_checkboxes[checkboxId] = wantOn;
    BlazeMemRunToggle(checkboxId, wantOn, label, onEnable, onDisable);
}

static void ToggleAimbotMaster(bool enabled)
{
    ui.esp.AimbotEnabled = enabled;
    g_Globals.AimBot.Enabled = false;
    g_Globals.AimBot.Rage = false;
    g_Globals.AimBot.Ragev2 = false;
    if (enabled) {
        switch (ui.esp.AimbotType) {
        case 0: g_Globals.AimBot.Enabled = true; break;
        case 1: g_Globals.AimBot.Rage = true; break;
        case 2: break;
        }
    }
    else {
        g_Globals.AimBot.KeyBind = VK_LBUTTON;
    }
}

static void ApplyAimbotLegitHold()
{
    if (ui.esp.AimbotType != 2 || !ui.esp.AimbotEnabled || keybind_aimbot_legit == 0)
        return;

    if (GetAsyncKeyState(keybind_aimbot_legit) & 0x8000) {
        g_Globals.AimBot.Enabled = true;
        g_Globals.AimBot.KeyBind = keybind_aimbot_legit;
    }
    else {
        g_Globals.AimBot.Enabled = false;
        g_Globals.AimBot.KeyBind = VK_LBUTTON;
    }
}

static void UtilityHotkeysThread()
{
    static bool wall_pressed = false;
    static bool fastland_pressed = false;
    static bool cam_pressed = false;
    static bool sniper_pressed = false;
    static bool refresh_esp_pressed = false;
    static bool streamer_pressed = false;
    static bool wukong_pressed = false;
    static bool sniper_scope_pressed = false;
    static bool telekill_pressed = false;
    static bool speed_timer_pressed = false;
    static bool down_player_pressed = false;
    static bool aimbot_toggle_pressed = false;
    static bool external_aim_pressed = false;
    static bool silent_aim_pressed = false;
    static bool enemy_pull_pressed = false;
    static bool close_pressed = false;
    static std::chrono::steady_clock::time_point s_lastEspRefreshHotkey;

    while (true) {
        Backend_SyncKeybindsFromYorzen();

        if (keybind_wall_hack != 0 && (GetAsyncKeyState(keybind_wall_hack) & 0x8000)) {
            if (!wall_pressed) {
                BlazeMemHotkeyToggle(31, "Wall Hack",
                    []() { Aim.ActivateWallhack(); }, []() { Aim.OFFWallhack(); });
                wall_pressed = true;
            }
        }
        else wall_pressed = false;

        if (keybind_fast_landing != 0 && (GetAsyncKeyState(keybind_fast_landing) & 0x8000)) {
            if (!fastland_pressed) {
                BlazeMemHotkeyToggle(111, "Fast Landing",
                    []() { Aim.ActivateFastlanding(); }, []() { Aim.OFFFastlanding(); });
                fastland_pressed = true;
            }
        }
        else fastland_pressed = false;

        if (keybind_camera_right != 0 && (GetAsyncKeyState(keybind_camera_right) & 0x8000)) {
            if (!cam_pressed) {
                BlazeMemHotkeyToggle(301, "Camera Right",
                    []() { Aim.ActivateCamera(); }, []() { Aim.OFFCamera(); });
                cam_pressed = true;
            }
        }
        else cam_pressed = false;

        if (keybind_sniper_switch != 0 && (GetAsyncKeyState(keybind_sniper_switch) & 0x8000)) {
            if (!sniper_pressed) {
                BlazeMemHotkeyToggle(8, "Sniper Switch",
                    []() { Aim.SniperSwitchon(); }, []() { Aim.SniperSwitchoff(); });
                sniper_pressed = true;
            }
        }
        else sniper_pressed = false;

        if (keybind_refresh_esp != 0 && (GetAsyncKeyState(keybind_refresh_esp) & 0x8000)) {
            if (!refresh_esp_pressed) {
                const auto now = std::chrono::steady_clock::now();
                if (std::chrono::duration_cast<std::chrono::milliseconds>(now - s_lastEspRefreshHotkey).count() >= 250) {
                    s_lastEspRefreshHotkey = now;
                    g_Globals.EspConfig.Refresh = true;
                    p_notif.AddMessage("ESP Refreshed", ICON_BOMB_FILL, c::main_color);
                }
                refresh_esp_pressed = true;
            }
        }
        else refresh_esp_pressed = false;

        if (keybind_streamer_mode != 0 && (GetAsyncKeyState(keybind_streamer_mode) & 0x8000)) {
            if (!streamer_pressed) {
                g_Globals.General.Capture = !g_Globals.General.Capture;
                ui.esp.EspStreamerMode = g_Globals.General.Capture;
                p_notif.AddMessage(g_Globals.General.Capture ? "Streamer Mode Enabled" : "Streamer Mode Disabled",
                    ICON_BOMB_FILL, g_Globals.General.Capture ? c::main_color : ImColor(255, 80, 80, 255));
                streamer_pressed = true;
            }
        }
        else streamer_pressed = false;

        if (keybind_wukong_mode != 0 && (GetAsyncKeyState(keybind_wukong_mode) & 0x8000)) {
            if (!wukong_pressed) {
                g_Globals.EspConfig.showOnlyVisible = !g_Globals.EspConfig.showOnlyVisible;
                ui.esp.WukongMode = g_Globals.EspConfig.showOnlyVisible;
                p_notif.AddMessage(g_Globals.EspConfig.showOnlyVisible ? "Wukong Mode Enabled" : "Wukong Mode Disabled",
                    ICON_BOMB_FILL, g_Globals.EspConfig.showOnlyVisible ? c::main_color : ImColor(255, 80, 80, 255));
                wukong_pressed = true;
            }
        }
        else wukong_pressed = false;

        if (keybind_sniper_scope != 0 && (GetAsyncKeyState(keybind_sniper_scope) & 0x8000)) {
            if (!sniper_scope_pressed) {
                g_Globals.Misc.SniperScope = !g_Globals.Misc.SniperScope;
                ui.esp.SniperScopeEnabled = g_Globals.Misc.SniperScope;
                p_notif.AddMessage(g_Globals.Misc.SniperScope ? "Sniper Scope Enabled" : "Sniper Scope Disabled",
                    ICON_BOMB_FILL, g_Globals.Misc.SniperScope ? c::main_color : ImColor(255, 80, 80, 255));
                sniper_scope_pressed = true;
            }
        }
        else sniper_scope_pressed = false;

        if (keybind_telekill != 0 && (GetAsyncKeyState(keybind_telekill) & 0x8000)) {
            if (!telekill_pressed) {
                g_Globals.Misc.TeleKill = !g_Globals.Misc.TeleKill;
                p_notif.AddMessage(g_Globals.Misc.TeleKill ? "TeleKill Enabled" : "TeleKill Disabled",
                    ICON_BOMB_FILL, g_Globals.Misc.TeleKill ? c::main_color : ImColor(255, 80, 80, 255));
                telekill_pressed = true;
            }
        }
        else telekill_pressed = false;

        if (keybind_speed_timer != 0 && (GetAsyncKeyState(keybind_speed_timer) & 0x8000)) {
            if (!speed_timer_pressed) {
                g_Globals.Misc.SpeedTimerEnabled = !g_Globals.Misc.SpeedTimerEnabled;
                ui.esp.SpeedTimerEnabled = g_Globals.Misc.SpeedTimerEnabled;
                p_notif.AddMessage(g_Globals.Misc.SpeedTimerEnabled ? "Speed Timer Enabled" : "Speed Timer Disabled",
                    ICON_BOMB_FILL, g_Globals.Misc.SpeedTimerEnabled ? c::main_color : ImColor(255, 80, 80, 255));
                speed_timer_pressed = true;
            }
        }
        else speed_timer_pressed = false;

        if (keybind_down_player != 0 && (GetAsyncKeyState(keybind_down_player) & 0x8000)) {
            if (!down_player_pressed) {
                g_Globals.Misc.DownPlayer = !g_Globals.Misc.DownPlayer;
                ui.esp.DownPlayerEnabled = g_Globals.Misc.DownPlayer;
                p_notif.AddMessage(g_Globals.Misc.DownPlayer ? "Down Player Enabled" : "Down Player Disabled",
                    ICON_BOMB_FILL, g_Globals.Misc.DownPlayer ? c::main_color : ImColor(255, 80, 80, 255));
                down_player_pressed = true;
            }
        }
        else down_player_pressed = false;

        if (YorzenKey::AimbotToggleKey != 0 && (GetAsyncKeyState(YorzenKey::AimbotToggleKey) & 0x8000)) {
            if (!aimbot_toggle_pressed) {
                ToggleAimbotMaster(!ui.esp.AimbotEnabled);
                p_notif.AddMessage(ui.esp.AimbotEnabled ? "Aimbot Enabled" : "Aimbot Disabled",
                    ICON_BOMB_FILL, ui.esp.AimbotEnabled ? c::main_color : ImColor(255, 80, 80, 255));
                aimbot_toggle_pressed = true;
            }
        }
        else aimbot_toggle_pressed = false;

        if (g_Globals.AimBot.ExternalKey != 0 && (GetAsyncKeyState(g_Globals.AimBot.ExternalKey) & 0x8000)) {
            if (!external_aim_pressed) {
                ui.esp.AimExternalEnabled = !ui.esp.AimExternalEnabled;
                g_Globals.AimBot.ExternalEnabled = ui.esp.AimExternalEnabled;
                p_notif.AddMessage(ui.esp.AimExternalEnabled ? "External Aim Enabled" : "External Aim Disabled",
                    ICON_BOMB_FILL, ui.esp.AimExternalEnabled ? c::main_color : ImColor(255, 80, 80, 255));
                external_aim_pressed = true;
            }
        }
        else external_aim_pressed = false;

        if (YorzenKey::SilentAimKey != 0 && (GetAsyncKeyState(YorzenKey::SilentAimKey) & 0x8000)) {
            if (!silent_aim_pressed) {
                ui.esp.AimSilentEnabled = !ui.esp.AimSilentEnabled;
                g_Globals.AimBot.SilentAimCheckbox = ui.esp.AimSilentEnabled;
                if (ui.esp.AimSilentEnabled) {
                    g_Globals.AimBot.SilentAimMax = (ui.esp.AimSilentHitbox == 0);
                    g_Globals.AimBot.SilentAimBody = (ui.esp.AimSilentHitbox == 1);
                    g_Globals.AimBot.SilentAimReworked = g_Globals.AimBot.SilentAimMax;
                }
                else {
                    g_Globals.AimBot.SilentAimMax = false;
                    g_Globals.AimBot.SilentAimBody = false;
                    g_Globals.AimBot.SilentAimReworked = false;
                }
                p_notif.AddMessage(ui.esp.AimSilentEnabled ? "Silent Aim Enabled" : "Silent Aim Disabled",
                    ICON_BOMB_FILL, ui.esp.AimSilentEnabled ? c::main_color : ImColor(255, 80, 80, 255));
                silent_aim_pressed = true;
            }
        }
        else silent_aim_pressed = false;

        if (YorzenKey::EnemyPullKey != 0 && (GetAsyncKeyState(YorzenKey::EnemyPullKey) & 0x8000)) {
            if (!enemy_pull_pressed) {
                ui.esp.PullEnemyEnabled = !ui.esp.PullEnemyEnabled;
                g_Globals.Misc.EnemyPullEnabled = ui.esp.PullEnemyEnabled;
                g_Globals.Misc.PullEnemy360Enabled = ui.esp.PullEnemyEnabled;
                p_notif.AddMessage(ui.esp.PullEnemyEnabled ? "Enemy Pull Enabled" : "Enemy Pull Disabled",
                    ICON_BOMB_FILL, ui.esp.PullEnemyEnabled ? c::main_color : ImColor(255, 80, 80, 255));
                enemy_pull_pressed = true;
            }
        }
        else enemy_pull_pressed = false;

        ApplyAimbotLegitHold();

        if (YorzenKey::ClosedKey != 0 && (GetAsyncKeyState(YorzenKey::ClosedKey) & 0x8000)) {
            if (!close_pressed) {
                p_notif.AddMessage("Closing...", ICON_BOMB_FILL, ImColor(255, 80, 80, 255));
                close_pressed = true;
                g_Globals.General.ShutDown = true;
            }
        }
        else close_pressed = false;

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void Backend_Notify(const char* message, bool success)
{
    p_notif.AddMessage(message, ICON_BOMB_FILL, success ? c::main_color : ImColor(255, 80, 80, 255));
}

void Backend_RenderNotifications()
{
    p_notif.Render();
}

namespace {
bool FileExistsForCleaner(const char* filePath) {
    return false;
}

bool DownloadCleanerFile(const char* url, const char* localFile) {
    return false;
}

bool RunCleanerBatch(const char* filePath) {
    return false;
}

void RunTempCleanerImpl() {
    // Removed
}
} // namespace

void Backend_RunTempCleaner()
{
    std::thread([]() { RunTempCleanerImpl(); }).detach();
}

void Backend_RunAdbInit()
{
    extern void adbInit();
    g_AdbFailed = false;
    g_AdbReady = false;
    adbInit();
}

void Backend_ExitPanel()
{
    std::thread([]() {
        adb::KillEmulatorAndAdbOnExit();
        g_Globals.General.ShutDown = true;
    }).detach();
}

void Backend_SyncKeybindsFromYorzen()
{
    auto markActive = [](bool& check, int key) {
        if (key != 0)
            check = true;
    };

    markActive(YorzenKey::WallHack1Check, YorzenKey::WallHack1Key);
    markActive(YorzenKey::WallHack2Check, YorzenKey::WallHack2Key);
    markActive(YorzenKey::CamaraJipiCheck, YorzenKey::CamaraJipiKey);
    markActive(YorzenKey::SniperMacroCheck, YorzenKey::SniperMacroKey);
    markActive(YorzenKey::FakeLagCheck, YorzenKey::FakeLagKey);
    markActive(YorzenKey::TeleportKillCheck, YorzenKey::TeleportKillKey);
    markActive(YorzenKey::AimbotToggleCheck, YorzenKey::AimbotToggleKey);
    markActive(YorzenKey::RefreshEspCheck, YorzenKey::RefreshEspKey);
    markActive(YorzenKey::StreamerModeCheck, YorzenKey::StreamerModeKey);
    markActive(YorzenKey::WukongModeCheck, YorzenKey::WukongModeKey);
    markActive(YorzenKey::SniperScopeCheck, YorzenKey::SniperScopeKey);
    markActive(YorzenKey::AimbotLegitCheck, YorzenKey::AimbotLegitKey);
    markActive(YorzenKey::AimbotExtCheck, YorzenKey::AimbotExtKey);
    markActive(YorzenKey::SilentAimCheck, YorzenKey::SilentAimKey);
    markActive(YorzenKey::EnemyPullCheck, YorzenKey::EnemyPullKey);
    markActive(YorzenKey::HideMenuCheck, YorzenKey::HideMenuKey);
    markActive(YorzenKey::ClosedCheck, YorzenKey::ClosedKey);

    keybind_telekill = YorzenKey::TeleportKillKey;
    keybind_wall_hack = YorzenKey::WallHack1Key;
    keybind_fast_landing = YorzenKey::WallHack2Key;
    keybind_camera_right = YorzenKey::CamaraJipiKey;
    keybind_sniper_switch = YorzenKey::SniperMacroKey;
    keybind_speed_timer = YorzenKey::FakeLagKey;
    keybind_refresh_esp = YorzenKey::RefreshEspKey;
    keybind_menu_key = YorzenKey::HideMenuKey;
    keybind_streamer_mode = YorzenKey::StreamerModeKey;
    keybind_wukong_mode = YorzenKey::WukongModeKey;
    keybind_sniper_scope = YorzenKey::SniperScopeKey;
    keybind_aimbot_legit = YorzenKey::AimbotLegitKey;
    keybind_aimbot_external = YorzenKey::AimbotExtKey;
    g_Globals.AimBot.ExternalKey = YorzenKey::AimbotExtKey;
    ui.esp.AimExternalKey = YorzenKey::AimbotExtKey;
}

void Backend_StartUtilityThread()
{
    if (s_utility_started.exchange(true))
        return;
    std::thread(UtilityHotkeysThread).detach();
}
