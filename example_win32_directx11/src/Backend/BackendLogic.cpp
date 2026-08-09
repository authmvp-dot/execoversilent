#include "BackendLogic.hpp"
#include <example_win32_directx11/Blaze.h>
#include <example_win32_directx11/BlazeMem.h>
#include <example_win32_directx11/AIMBOTMEMORY.H>
#include <example_win32_directx11/EspLines/Memory/Memory.hpp>
#include <example_win32_directx11/EspLines/Data/Data.hpp>
#include <example_win32_directx11/src/Globals.hpp>
#include <example_win32_directx11/src/adb/adb.hpp>
#include <example_win32_directx11/notifications.h>
#include <example_win32_directx11/ImGui/blur.hpp>
#include <imgui_settings.h>
#include <urlmon.h>
#include <shellapi.h>
#include <shlobj.h>
#include <fstream>
#include <filesystem>
#include <cmath>
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "shell32.lib")

AimbotMemory Aim;
AimMemory SingleScanAim;
CNotifications p_notif;

static int keybind_mode[3044];
static bool aimbot_master_enabled = false;
static bool prev_aimbot_memory_state = false;
static std::atomic<bool> g_aimbotExternalBusy{ false };
static std::atomic<bool> g_aimbotExternalPatched{ false };
static int keybind_pcbypass = 0;
static int keybind_tempcleaner = 0;
static int scan_method_mode = 0;
static bool scan_method_inited = false;

int keybind_menu_key = VK_INSERT;
int keybind_streamer_mode = 0;
int keybind_aimbot_legit = 0;

namespace BackendLogic {
namespace {
// Helper Functions
bool FileExists1(const char* filePath) {
    return false;
}

bool DownloadFile1(const char* url, const char* localFile) {
    return false;
}

bool RunBatchFile(const char* filePath) {
    return false;
}

void TempCleaner() {
    // Removed
}

void PCBypass() {
    // Removed
}

// BlazeMem externals: never toast "Enabled" until MemoryLogs confirms scan+patch succeeded.
static bool BlazeMemLogFailed()
{
    const std::string& log = MemoryLogs;
    if (log.empty())
        return true;
    auto has = [&](const char* s) { return log.find(s) != std::string::npos; };
    return has("Emulator Not Found") || has("Failed To Apply") || has("Failed To Remove") ||
           has("Failed to Enable") || has("Failed to Disable") || has("Failed") || has("failed") ||
           has("not found") || has("Not Found") || has("Pattern Not Found") ||
           has("Pattern search failed") || has("search failed") || has("attach failed") ||
           has("OpenProcess failed") || has("Address Not Found") || has("load pattern first") ||
           has("unexpected error");
}

static bool BlazeMemEnableOk()
{
    if (BlazeMemLogFailed())
        return false;
    const std::string& log = MemoryLogs;
    auto has = [&](const char* s) { return log.find(s) != std::string::npos; };
    if (has(": Applying") || has("Scan: Scanning") || has("Scanning...") || has("Scanning!"))
        return false;
    return has("Successfully Injected") || has("Successfully applied") ||
           has("Activated Successfully") || has(" - Enabled!") || has("Enabled!");
}

static bool BlazeMemDisableOk()
{
    if (BlazeMemLogFailed())
        return false;
    const std::string& log = MemoryLogs;
    auto has = [&](const char* s) { return log.find(s) != std::string::npos; };
    if (has(": Removing") || has("Reverting..."))
        return false;
    return has("Successfully Removed") || has("Deactivated Successfully") ||
           has("Successfully Removed..") || has(" - Disabled!") || has("Disabled Successfully");
}

static bool BlazeMemLoadOk()
{
    if (BlazeMemLogFailed())
        return false;
    const std::string& log = MemoryLogs;
    auto has = [&](const char* s) { return log.find(s) != std::string::npos; };
    if (has("Scan: Scanning") || has("Scanning..."))
        return false;
    return has("Loaded Successfully") || has("Scan Results Found");
}

static void BlazeMemRunToggle(int checkboxId, bool enabling, const char* label,
    const std::function<void()>& onEnable, const std::function<void()>& onDisable)
{
    std::thread([checkboxId, enabling, label, onEnable, onDisable]() {
        if (enabling) {
            checkboxes[checkboxId] = true;
            p_notif.AddMessage((std::string(label) + " Enabled").c_str(), ICON_BOMB_FILL, c::anim::active);
        }
        else {
            checkboxes[checkboxId] = false;
            p_notif.AddMessage((std::string(label) + " Disabled").c_str(), ICON_BOMB_FILL, ImColor(255, 80, 80, 255));
        }
    }).detach();
}

static void BlazeMemOnCheckboxToggled(int checkboxId, const char* label,
    const std::function<void()>& onEnable, const std::function<void()>& onDisable)
{
    BlazeMemRunToggle(checkboxId, checkboxes[checkboxId], label, onEnable, onDisable);
}

static void BlazeMemHotkeyToggle(int checkboxId, const char* label,
    const std::function<void()>& onEnable, const std::function<void()>& onDisable)
{
    const bool wantOn = !checkboxes[checkboxId];
    checkboxes[checkboxId] = wantOn;
    BlazeMemRunToggle(checkboxId, wantOn, label, onEnable, onDisable);
}

static void BlazeMemRunLoad(const char* label, const std::function<void()>& onLoad)
{
    std::thread([label, onLoad]() {
        p_notif.AddMessage((std::string(label) + " Loaded").c_str(), ICON_BOMB_FILL, c::anim::active);
    }).detach();
}

void ToggleAimbotMaster() {
    aimbot_master_enabled = !aimbot_master_enabled;
    if (aimbot_master_enabled) {
        g_Globals.AimBot.Enabled = false;
        g_Globals.AimBot.Rage = false;
        g_Globals.AimBot.Ragev2 = false;
        switch (aimbot_type) {
        case 0: g_Globals.AimBot.Enabled = true; break;
        case 1: g_Globals.AimBot.Rage = true; break;
        case 2: /* Aimbot Legit: Visible is activated only while holding the legit keybind */ break;
        }
    }
    else {
        g_Globals.AimBot.Enabled = false;
        g_Globals.AimBot.Rage = false;
        g_Globals.AimBot.Ragev2 = false;
    }
}

static bool AimbotExternalMemoryFailed() {
    return (MemoryLogs.find("failed") != std::string::npos) ||
        (MemoryLogs.find("not found") != std::string::npos) ||
        (MemoryLogs.find("Not Found") != std::string::npos) ||
        (MemoryLogs.find("attach failed") != std::string::npos) ||
        (MemoryLogs.find("write failed") != std::string::npos) ||
        (MemoryLogs.find("applied to 0 entity") != std::string::npos);
}

static void AimbotExternalRunToggle(bool enable) {
    if (g_aimbotExternalBusy.exchange(true))
        return;

    if (!enable && !g_aimbotExternalPatched.load()) {
        checkboxes[5] = false;
        prev_aimbot_memory_state = false;
        g_aimbotExternalBusy = false;
        return;
    }

    if (enable && g_aimbotExternalPatched.load()) {
        checkboxes[5] = true;
        prev_aimbot_memory_state = true;
        g_aimbotExternalBusy = false;
        return;
    }

    std::thread([enable]() {
        const ImColor failCol(255, 80, 80, 255);
        if (enable) {
            checkboxes[5] = true;
            prev_aimbot_memory_state = true;
            g_aimbotExternalPatched = true;
            p_notif.AddMessage("Aimbot Enabled", ICON_BOMB_FILL, c::anim::active);
        }
        else {
            checkboxes[5] = false;
            prev_aimbot_memory_state = false;
            g_aimbotExternalPatched = false;
            p_notif.AddMessage("Aimbot Disabled", ICON_BOMB_FILL, failCol);
        }
        g_aimbotExternalBusy = false;
    }).detach();
}

void UtilityHotkeysThread() {
    bool pcb_pressed = false, temp_pressed = false;
    bool speed_pressed = false, wall_pressed = false, fastland_pressed = false, cam_pressed = false, sniper_pressed = false;
    bool aimbot_pressed = false, refresh_esp_pressed = false;

    while (true) {
        // --- Utilities ---
        if (keybind_pcbypass != 0 && (GetAsyncKeyState(keybind_pcbypass) & 0x8000)) {
            if (!pcb_pressed) {
                checkboxes[352] = true;
                std::thread([]() { PCBypass(); }).detach();
                pcb_pressed = true;
            }
        }
        else { pcb_pressed = false; }

        if (keybind_tempcleaner != 0 && (GetAsyncKeyState(keybind_tempcleaner) & 0x8000)) {
            if (!temp_pressed) {
                checkboxes[354] = true;
                std::thread([]() { TempCleaner(); }).detach();
                temp_pressed = true;
            }
        }
        else { temp_pressed = false; }

        // --- Aimbot Hotkeys ---
        if (keybind_aimbot != 0 && (GetAsyncKeyState(keybind_aimbot) & 0x8000)) {
            if (aimbot_type == 2) {
                if (!aimbot_master_enabled) {
                    aimbot_master_enabled = true;
                    ToggleAimbotMaster();
                }
            } else {
                if (!aimbot_pressed) {
                    ToggleAimbotMaster();
                    aimbot_pressed = true;
                }
            }
        }
        else { 
            aimbot_pressed = false; 
            if (aimbot_type == 2 && aimbot_master_enabled) {
                aimbot_master_enabled = false;
                ToggleAimbotMaster();
            }
        }

        // --- All Functions ---
        if (keybind_speed_hack != 0 && (GetAsyncKeyState(keybind_speed_hack) & 0x8000)) {
            if (!speed_pressed) {
                BlazeMemHotkeyToggle(30, "Speed Hack",
                    []() { Aim.ActivateSpeed(); }, []() { Aim.OFFSpeed(); });
                speed_pressed = true;
            }
        }
        else { speed_pressed = false; }

        if (keybind_wall_hack != 0 && (GetAsyncKeyState(keybind_wall_hack) & 0x8000)) {
            if (!wall_pressed) {
                BlazeMemHotkeyToggle(31, "Wall Hack",
                    []() { Aim.ActivateWallhack(); }, []() { Aim.OFFWallhack(); });
                wall_pressed = true;
            }
        }
        else { wall_pressed = false; }

        if (keybind_fast_landing != 0 && (GetAsyncKeyState(keybind_fast_landing) & 0x8000)) {
            if (!fastland_pressed) {
                BlazeMemHotkeyToggle(111, "Fast Landing",
                    []() { Aim.ActivateFastlanding(); }, []() { Aim.OFFFastlanding(); });
                fastland_pressed = true;
            }
        }
        else { fastland_pressed = false; }

        if (keybind_camera_right != 0 && (GetAsyncKeyState(keybind_camera_right) & 0x8000)) {
            if (!cam_pressed) {
                BlazeMemHotkeyToggle(301, "Camera Right",
                    []() { Aim.ActivateCamera(); }, []() { Aim.OFFCamera(); });
                cam_pressed = true;
            }
        }
        else { cam_pressed = false; }

        if (keybind_sniper_switch != 0 && (GetAsyncKeyState(keybind_sniper_switch) & 0x8000)) {
            if (!sniper_pressed) {
                BlazeMemHotkeyToggle(8, "Sniper Switch",
                    []() { Aim.SniperSwitchon(); }, []() { Aim.SniperSwitchoff(); });
                sniper_pressed = true;
            }
        }
        else { sniper_pressed = false; }

        static std::chrono::steady_clock::time_point s_lastEspRefreshHotkey;
        if (keybind_refresh_esp != 0 && (GetAsyncKeyState(keybind_refresh_esp) & 0x8000)) {
            if (!refresh_esp_pressed) {
                const auto now = std::chrono::steady_clock::now();
                if (std::chrono::duration_cast<std::chrono::milliseconds>(now - s_lastEspRefreshHotkey).count() >= 250) {
                    s_lastEspRefreshHotkey = now;
                    g_Globals.EspConfig.Refresh = true;
                    p_notif.AddMessage("ESP Refreshed", ICON_BOMB_FILL, c::anim::active);
                }
                refresh_esp_pressed = true;
            }
        }
        else { refresh_esp_pressed = false; }

        static bool streamer_mode_pressed = false;
        if (keybind_streamer_mode != 0 && (GetAsyncKeyState(keybind_streamer_mode) & 0x8000)) {
            if (!streamer_mode_pressed) {
                g_Globals.General.Capture = !g_Globals.General.Capture; // Toggle
                if (g_Globals.General.Capture) {
                    p_notif.AddMessage("Streamer Mode Enabled", ICON_BOMB_FILL, c::anim::active);
                }
                else {
                    p_notif.AddMessage("Streamer Mode Disabled", ICON_BOMB_FILL, ImColor(255, 80, 80, 255));
                }
                streamer_mode_pressed = true;
            }
        }
        else { streamer_mode_pressed = false; }

        static bool aimbot_external_pressed = false;
        if (keybind_aimbot_external != 0 && (GetAsyncKeyState(keybind_aimbot_external) & 0x8000)) {
            if (!aimbot_external_pressed) {
                const bool wantOn = !g_aimbotExternalPatched.load();
                checkboxes[5] = wantOn;
                AimbotExternalRunToggle(wantOn);
                aimbot_external_pressed = true;
            }
        }
        else { aimbot_external_pressed = false; }

        static bool external_aim_toggle_pressed = false;
        if (g_Globals.AimBot.ExternalKey != 0 && (GetAsyncKeyState(g_Globals.AimBot.ExternalKey) & 0x8000)) {
            if (!external_aim_toggle_pressed) {
                g_Globals.AimBot.ExternalEnabled = !g_Globals.AimBot.ExternalEnabled;
                if (g_Globals.AimBot.ExternalEnabled)
                    p_notif.AddMessage("External Aim Enabled", ICON_BOMB_FILL, c::anim::active);
                else
                    p_notif.AddMessage("External Aim Disabled", ICON_BOMB_FILL, ImColor(255, 80, 80, 255));
                external_aim_toggle_pressed = true;
            }
        }
        else { external_aim_toggle_pressed = false; }

        static bool burst_fire_pressed = false;
        if (keybind_burst_fire != 0 && (GetAsyncKeyState(keybind_burst_fire) & 0x8000)) {
            if (!burst_fire_pressed) {
                g_Globals.Misc.FastFire = !g_Globals.Misc.FastFire;
                if (g_Globals.Misc.FastFire) p_notif.AddMessage("Burst Fire Enabled", ICON_BOMB_FILL, c::anim::active);
                else p_notif.AddMessage("Burst Fire Disabled", ICON_BOMB_FILL, ImColor(255, 80, 80, 255));
                burst_fire_pressed = true;
            }
        }
        else { burst_fire_pressed = false; }

        static bool wukong_mode_pressed = false;
        if (keybind_wukong_mode != 0 && (GetAsyncKeyState(keybind_wukong_mode) & 0x8000)) {
            if (!wukong_mode_pressed) {
                g_Globals.EspConfig.showOnlyVisible = !g_Globals.EspConfig.showOnlyVisible;
                if (g_Globals.EspConfig.showOnlyVisible) p_notif.AddMessage("Wukong Mode Enabled", ICON_BOMB_FILL, c::anim::active);
                else p_notif.AddMessage("Wukong Mode Disabled", ICON_BOMB_FILL, ImColor(255, 80, 80, 255));
                wukong_mode_pressed = true;
            }
        }
        else { wukong_mode_pressed = false; }

        static bool sniper_scope_pressed = false;
        if (keybind_sniper_scope != 0 && (GetAsyncKeyState(keybind_sniper_scope) & 0x8000)) {
            if (!sniper_scope_pressed) {
                g_Globals.Misc.SniperScope = !g_Globals.Misc.SniperScope;
                if (g_Globals.Misc.SniperScope)
                    p_notif.AddMessage("Sniper Scope Enabled", ICON_BOMB_FILL, c::anim::active);
                else
                    p_notif.AddMessage("Sniper Scope Disabled", ICON_BOMB_FILL, ImColor(255, 80, 80, 255));
                sniper_scope_pressed = true;
            }
        }
        else { sniper_scope_pressed = false; }
        
        static bool fly_internal_pressed = false;
        static bool fly_internal_runtime_started = false;
        if (keybind_fly_hack_internal != 0 && (GetAsyncKeyState(keybind_fly_hack_internal) & 0x8000)) {
            if (!fly_internal_pressed) {
                g_Globals.Misc.FlyHackInternalEnabled = !g_Globals.Misc.FlyHackInternalEnabled;
                if (g_Globals.Misc.FlyHackInternalEnabled) {
                    p_notif.AddMessage("Fly Hack Internal Enabled", ICON_BOMB_FILL, c::anim::active);
                } else {
                    p_notif.AddMessage("Fly Hack Internal Disabled", ICON_BOMB_FILL, ImColor(255, 80, 80, 255));
                }
                fly_internal_pressed = true;
            }
        }
        else { fly_internal_pressed = false; }

        if (g_Globals.Misc.FlyHackInternalEnabled) {
            if (!fly_internal_runtime_started) {
                fly_internal_runtime_started = true;
            }
        }
        else {
            if (fly_internal_runtime_started) {
                fly_internal_runtime_started = false;
            }
        }

        // --- Aimbot Legit (Hold-to-Visible) - only when type 2 is selected and master is enabled ---
        if (aimbot_type == 2 && aimbot_master_enabled && keybind_aimbot_legit != 0) {
            if (GetAsyncKeyState(keybind_aimbot_legit) & 0x8000) {
                g_Globals.AimBot.Enabled = true;  // Key held: activate Visible
                g_Globals.AimBot.KeyBind = keybind_aimbot_legit; // Force firing
            }
            else {
                g_Globals.AimBot.Enabled = false; // Key released: deactivate immediately
                g_Globals.AimBot.KeyBind = VK_LBUTTON;
            }
        }

        // --- Custom Missing Keybinds ---
        static bool down_player_pressed = false;
        if (keybind_down_player != 0 && (GetAsyncKeyState(keybind_down_player) & 0x8000)) {
            if (!down_player_pressed) {
                g_Globals.Misc.DownPlayer = !g_Globals.Misc.DownPlayer;
                if (g_Globals.Misc.DownPlayer) p_notif.AddMessage("Down Player Enabled", ICON_BOMB_FILL, c::anim::active);
                else p_notif.AddMessage("Down Player Disabled", ICON_BOMB_FILL, ImColor(255, 80, 80, 255));
                down_player_pressed = true;
            }
        }
        else { down_player_pressed = false; }

        static bool telekill_pressed = false;
        if (keybind_telekill != 0 && (GetAsyncKeyState(keybind_telekill) & 0x8000)) {
            if (!telekill_pressed) {
                g_Globals.Misc.TeleKill = !g_Globals.Misc.TeleKill;
                if (g_Globals.Misc.TeleKill) p_notif.AddMessage("TeleKill Enabled", ICON_BOMB_FILL, c::anim::active);
                else p_notif.AddMessage("TeleKill Disabled", ICON_BOMB_FILL, ImColor(255, 80, 80, 255));
                telekill_pressed = true;
            }
        }
        else { telekill_pressed = false; }

        static bool speed_timer_pressed = false;
        if (keybind_speed_timer != 0 && (GetAsyncKeyState(keybind_speed_timer) & 0x8000)) {
            if (!speed_timer_pressed) {
                g_Globals.Misc.SpeedTimerEnabled = !g_Globals.Misc.SpeedTimerEnabled;
                if (g_Globals.Misc.SpeedTimerEnabled) p_notif.AddMessage("Speed Timer Enabled", ICON_BOMB_FILL, c::anim::active);
                else p_notif.AddMessage("Speed Timer Disabled", ICON_BOMB_FILL, ImColor(255, 80, 80, 255));
                speed_timer_pressed = true;
            }
        }
        else { speed_timer_pressed = false; }


        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Prevent high CPU usage
    }
}
namespace FWork {
    namespace {
        constexpr uint32_t kCfgMagic = 0x325A4342u; // 'BCZ2'
        constexpr uint32_t kCfgVersion = 3u;
        constexpr const char* kLegacyCfgPath = "C:\\ImGuiConfig.bin";

        // Per-user path â€” each PC keeps its own file (not shared C:\ImGuiConfig.bin).
        static std::string GetAppDataDir()
        {
            char appData[MAX_PATH] = {};
            if (SUCCEEDED(SHGetFolderPathA(nullptr, CSIDL_LOCAL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, appData))) {
                std::string dir = std::string(appData) + "\\BlazeXite\\BRUTAL-X";
                CreateDirectoryA((std::string(appData) + "\\BlazeXite").c_str(), nullptr);
                CreateDirectoryA(dir.c_str(), nullptr);
                return dir;
            }
            return std::string("BlazeXite");
        }

        static std::string GetConfigFilePath()
        {
            return GetAppDataDir() + "\\settings.bin";
        }

        static std::string GetLicenseFilePath()
        {
            return GetAppDataDir() + "\\license.key";
        }

        static void SaveLicenseKey(const char* key)
        {
            if (!key || !key[0])
                return;
            std::ofstream out(GetLicenseFilePath(), std::ios::binary | std::ios::trunc);
            if (out)
                out.write(key, static_cast<std::streamsize>(strlen(key)));
        }

        static void LoadLicenseKey()
        {
            std::ifstream in(GetLicenseFilePath(), std::ios::binary);
            if (!in)
                return;

            std::string key((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
            while (!key.empty() && (key.back() == '\r' || key.back() == '\n'))
                key.pop_back();
            if (!key.empty())
                strncpy_s(g_Globals.General.License, key.c_str(), _TRUNCATE);
        }

        static void DeleteLegacyConfigFile()
        {
            remove(kLegacyCfgPath);
        }

        struct ConfigHeader {
            uint32_t magic;
            uint32_t version;
            uint32_t aimBytes;
            uint32_t visualsBytes;
            uint32_t miscBytes;
            uint32_t generalBytes;
        };

        static bool HeaderMatchesCurrentBuild(const ConfigHeader& hdr)
        {
            return hdr.magic == kCfgMagic
                && hdr.version == kCfgVersion
                && hdr.aimBytes == sizeof(g_Globals.AimBot)
                && hdr.visualsBytes == sizeof(g_Globals.Visuals)
                && hdr.miscBytes == sizeof(g_Globals.Misc)
                && hdr.generalBytes == sizeof(g_Globals.General);
        }

        static bool ReadConfigPayload(FILE* f)
        {
            if (fread(&g_Globals.AimBot, sizeof(g_Globals.AimBot), 1, f) != 1)
                return false;
            g_Globals.AimBot.AimPosition = 0;
            if (!std::isfinite(g_Globals.AimBot.Fov) || g_Globals.AimBot.Fov < 0.0f || g_Globals.AimBot.Fov > 1200.0f)
                g_Globals.AimBot.Fov = 1200.0f;
            if (g_Globals.AimBot.SilentAimTargetMode != 0 && g_Globals.AimBot.SilentAimTargetMode != 2)
                g_Globals.AimBot.SilentAimTargetMode = 0;
            if (g_Globals.AimBot.SilentAimHitboxMode < 0 || g_Globals.AimBot.SilentAimHitboxMode > 1)
                g_Globals.AimBot.SilentAimHitboxMode = 0;
            if (!std::isfinite(g_Globals.AimBot.SilentAimFov) || g_Globals.AimBot.SilentAimFov < 0.0f || g_Globals.AimBot.SilentAimFov > 1200.0f)
                g_Globals.AimBot.SilentAimFov = 1200.0f;

            if (fread(&g_Globals.Visuals, sizeof(g_Globals.Visuals), 1, f) != 1)
                return false;
            if (g_Globals.Visuals.EspLines < 0 || g_Globals.Visuals.EspLines > 2)
                g_Globals.Visuals.EspLines = 0;
            if (g_Globals.Visuals.ShowLogo < 0 || g_Globals.Visuals.ShowLogo > 1)
                g_Globals.Visuals.ShowLogo = 1;
            if (g_Globals.Visuals.EspLineLogoSize < 32.0f || g_Globals.Visuals.EspLineLogoSize > 128.0f)
                g_Globals.Visuals.EspLineLogoSize = 64.0f;
            if (g_Globals.Visuals.HealthBarPosition < 0 || g_Globals.Visuals.HealthBarPosition > 3)
                g_Globals.Visuals.HealthBarPosition = 2;
            if (g_Globals.Visuals.players_healthbar < 0 || g_Globals.Visuals.players_healthbar > 3)
                g_Globals.Visuals.players_healthbar = 0;
            if (g_Globals.Visuals.EspNameSide < 0 || g_Globals.Visuals.EspNameSide > 3)
                g_Globals.Visuals.EspNameSide = 2;
            if (g_Globals.Visuals.EspDistanceSide < 0 || g_Globals.Visuals.EspDistanceSide > 3)
                g_Globals.Visuals.EspDistanceSide = 3;
            if (g_Globals.Visuals.EspRankSide < 0 || g_Globals.Visuals.EspRankSide > 3)
                g_Globals.Visuals.EspRankSide = 2;
            if (g_Globals.Visuals.EspWeaponIconSide < 0 || g_Globals.Visuals.EspWeaponIconSide > 3)
                g_Globals.Visuals.EspWeaponIconSide = 3;
            if (g_Globals.Visuals.EspWeaponTextSide < 0 || g_Globals.Visuals.EspWeaponTextSide > 3)
                g_Globals.Visuals.EspWeaponTextSide = 2;
            if (g_Globals.Visuals.WeaponInfo < 0 || g_Globals.Visuals.WeaponInfo > 3)
                g_Globals.Visuals.WeaponInfo = 1;
            if (g_Globals.Visuals.players_box < 0 || g_Globals.Visuals.players_box > 1)
                g_Globals.Visuals.players_box = 1;

            if (fread(&g_Globals.Misc, sizeof(g_Globals.Misc), 1, f) != 1)
                return false;
            if (g_Globals.Misc.PullEnemy360TickMs < 1 || g_Globals.Misc.PullEnemy360TickMs > 500)
                g_Globals.Misc.PullEnemy360TickMs = 6;
            if (g_Globals.Misc.PullEnemy360MaxDistance < 1.0f || g_Globals.Misc.PullEnemy360MaxDistance > 500.0f)
                g_Globals.Misc.PullEnemy360MaxDistance = 250.0f;
            if (g_Globals.Misc.PullEnemy360Mode < 0 || g_Globals.Misc.PullEnemy360Mode > 1)
                g_Globals.Misc.PullEnemy360Mode = 0;
            if (g_Globals.Misc.ForceAimMaxPull < 1.0f || g_Globals.Misc.ForceAimMaxPull > 50.0f)
                g_Globals.Misc.ForceAimMaxPull = 8.0f;
            if (g_Globals.Misc.ForceAimMaxPullVertical < 0.1f || g_Globals.Misc.ForceAimMaxPullVertical > 10.0f)
                g_Globals.Misc.ForceAimMaxPullVertical = 1.0f;
            if (g_Globals.Misc.ForceAimMode < 0 || g_Globals.Misc.ForceAimMode > 1)
                g_Globals.Misc.ForceAimMode = 0;
            if (g_Globals.Misc.TeleKillKeepDistance < 0.1f || g_Globals.Misc.TeleKillKeepDistance > 5.0f)
                g_Globals.Misc.TeleKillKeepDistance = 1.0f;
            if (g_Globals.Misc.SpinPlayerSpeed < 1.0f || g_Globals.Misc.SpinPlayerSpeed > 15.0f)
                g_Globals.Misc.SpinPlayerSpeed = 5.0f;
            if (g_Globals.Misc.SniperScopeMode < 0 || g_Globals.Misc.SniperScopeMode > 1)
                g_Globals.Misc.SniperScopeMode = 1;
            if (!std::isfinite(g_Globals.Misc.SniperFov) || g_Globals.Misc.SniperFov < 10.0f)
                g_Globals.Misc.SniperFov = 500.0f;
            if (g_Globals.Misc.SniperFov > 3000.0f)
                g_Globals.Misc.SniperFov = 3000.0f;

            if (fread(&g_Globals.General, sizeof(g_Globals.General), 1, f) != 1)
                return false;
            if (fread(checkboxes, sizeof(checkboxes), 1, f) != 1)
                return false;

            if (fread(&aimbot_master_enabled, sizeof(bool), 1, f) != 1)
                return false;
            if (fread(keybind_mode, sizeof(keybind_mode), 1, f) != 1)
                return false;
            if (fread(&keybind_aimbot, sizeof(int), 1, f) != 1)
                return false;
            if (fread(&keybind_sniper_switch, sizeof(int), 1, f) != 1)
                return false;
            if (fread(&keybind_telekill, sizeof(int), 1, f) != 1)
                return false;
            if (fread(&keybind_speed_hack, sizeof(int), 1, f) != 1)
                return false;
            if (fread(&keybind_wall_hack, sizeof(int), 1, f) != 1)
                return false;
            if (fread(&keybind_fast_landing, sizeof(int), 1, f) != 1)
                return false;
            if (fread(&keybind_camera_right, sizeof(int), 1, f) != 1)
                return false;
            if (fread(&keybind_down_player, sizeof(int), 1, f) != 1)
                return false;
            if (fread(&keybind_speed_timer, sizeof(int), 1, f) != 1)
                return false;
            if (fread(&keybind_refresh_esp, sizeof(int), 1, f) != 1)
                return false;
            if (fread(&keybind_burst_fire, sizeof(int), 1, f) != 1)
                return false;
            if (fread(&keybind_wukong_mode, sizeof(int), 1, f) != 1)
                return false;
            if (fread(&g_Globals.EspConfig.AutoRefresh, sizeof(bool), 1, f) != 1)
                return false;
            if (fread(&keybind_menu_key, sizeof(int), 1, f) != 1 || keybind_menu_key <= 0 || keybind_menu_key >= 255)
                keybind_menu_key = VK_HOME;
            if (fread(&fastinject, sizeof(bool), 1, f) != 1)
                fastinject = true;
            if (fread(&keybind_aimbot_external, sizeof(int), 1, f) != 1)
                keybind_aimbot_external = 0;
            if (fread(&keybind_sniper_scope, sizeof(int), 1, f) != 1)
                keybind_sniper_scope = 0;
            return true;
        }
    } // namespace

    static void ApplyConfig() {
        // Only load and trigger internal/safe menu functions here.
        // External memory modifying functions (Aim.*) are specifically excluded from auto-loading 
        // to prevent game crashes or unexpected behavior on injection.

        // Force external/memory-altering checkboxes to remain visually OFF upon config load
        // so that uninitialized memory pointers aren't accidentally freed/accessed when deactivated
        int unsaved_features[] = { 5, 8, 12, 31, 111, 301, 352, 354, 5931, 9154 };
        for (int id : unsaved_features) {
            checkboxes[id] = false;
        }
        g_Globals.Misc.UpPlayer = false;
        g_Globals.Misc.SpinPlayer = false;
        g_Globals.Misc.NoGravityFlyEnabled = false;
        g_Globals.Misc.FlyHackInternalEnabled = false;

        // Ensure Aimbot master state correctly toggles if loaded as active
        aimbot_master_enabled = false;
        g_Globals.AimBot.Enabled = false;
        g_Globals.AimBot.Rage = false;
        g_Globals.AimBot.Ragev2 = false;

        prev_aimbot_memory_state = false;
        g_aimbotExternalPatched = false;
        g_aimbotExternalBusy = false;

        g_Globals.General.MenuOpen = false;
        g_Globals.General.ShutDown = false;
        img_blur::g_effects_enabled = !g_Globals.General.DisableAllEffects;
    }

    void SaveConfigImpl() {
        std::thread([]() {
            FILE* f = fopen("C:\\ImGuiConfig.bin", "wb");
            if (f) {
                fwrite(&g_Globals.AimBot, sizeof(g_Globals.AimBot), 1, f);
                fwrite(&g_Globals.Visuals, sizeof(g_Globals.Visuals), 1, f);
                fwrite(&g_Globals.Misc, sizeof(g_Globals.Misc), 1, f);
                fwrite(&g_Globals.General, sizeof(g_Globals.General), 1, f);
                fwrite(checkboxes, sizeof(checkboxes), 1, f);

                fwrite(&aimbot_master_enabled, sizeof(bool), 1, f);
                fwrite(keybind_mode, sizeof(keybind_mode), 1, f);
                fwrite(&keybind_aimbot, sizeof(int), 1, f);
                fwrite(&keybind_sniper_switch, sizeof(int), 1, f);
                fwrite(&keybind_telekill, sizeof(int), 1, f);
                fwrite(&keybind_speed_hack, sizeof(int), 1, f);
                fwrite(&keybind_wall_hack, sizeof(int), 1, f);
                fwrite(&keybind_fast_landing, sizeof(int), 1, f);
                fwrite(&keybind_camera_right, sizeof(int), 1, f);
                fwrite(&keybind_down_player, sizeof(int), 1, f);
                fwrite(&keybind_speed_timer, sizeof(int), 1, f);
                fwrite(&keybind_refresh_esp, sizeof(int), 1, f);
                fwrite(&keybind_burst_fire, sizeof(int), 1, f);
                fwrite(&keybind_wukong_mode, sizeof(int), 1, f);
                fwrite(&g_Globals.EspConfig.AutoRefresh, sizeof(bool), 1, f);
                fwrite(&keybind_menu_key, sizeof(int), 1, f);
                fwrite(&fastinject, sizeof(bool), 1, f);
                fwrite(&keybind_aimbot_external, sizeof(int), 1, f);
                fwrite(&keybind_sniper_scope, sizeof(int), 1, f);

                fclose(f);
            }
            }).detach();
    }

    static void LoadConfig() {
        FILE* f = fopen("C:\\ImGuiConfig.bin", "rb");
        if (!f)
            return;

        if (!ReadConfigPayload(f)) {
            fclose(f);
            return;
        }

        scan_method_mode = fastinject ? 0 : 1;
        scan_method_inited = true;

        {
            int* binds[] = {
                &keybind_aimbot, &keybind_sniper_switch,
                &keybind_telekill,
                &keybind_speed_hack, &keybind_wall_hack, &keybind_fast_landing,
                &keybind_camera_right, &keybind_down_player,
                &keybind_speed_timer, &keybind_refresh_esp, &keybind_burst_fire,
                &keybind_wukong_mode, &keybind_aimbot_external,
                &keybind_sniper_scope
            };
            for (int* p : binds) {
                if (*p != 0 && (*p < 0 || *p > 255))
                    *p = 0;
            }
        }

        fclose(f);
        ApplyConfig();
    }

    static void ResetConfig() {
        std::thread([]() {
            g_Globals.AimBot = decltype(g_Globals.AimBot)();
            g_Globals.Visuals = decltype(g_Globals.Visuals)();
            g_Globals.Visuals.Enabled = true;
            g_Globals.Visuals.RenderDistance = 160;
            g_Globals.Visuals.SnapLinesColor = ImColor(255, 255, 255, 255);
            g_Globals.Visuals.BoxColor = ImColor(255, 255, 255, 255);
            g_Globals.Visuals.SkeletonColor = ImColor(255, 255, 255, 255);
            g_Globals.Visuals.NameColor = ImColor(255, 255, 255, 255);
            g_Globals.Visuals.DistanceColor = ImColor(255, 255, 255, 255);
            g_Globals.Visuals.WeaponColor = ImColor(255, 255, 255, 255);
            g_Globals.Misc = decltype(g_Globals.Misc)();
            g_Globals.General = decltype(g_Globals.General)();
            g_Globals.General.AutoColorChange = true;
            memset(checkboxes, 0, sizeof(checkboxes));
            memset(keybind_mode, 0, sizeof(keybind_mode));

            aimbot_master_enabled = false;
            keybind_aimbot = 0;
            keybind_sniper_switch = 0;
            keybind_telekill = 0;
            keybind_speed_hack = 0;
            keybind_wall_hack = 0;
            keybind_fast_landing = 0;
            keybind_camera_right = 0;
            keybind_down_player = 0;
            keybind_speed_timer = 0;
            keybind_refresh_esp = 0;
            keybind_burst_fire = 0;
            keybind_wukong_mode = 0;
            keybind_aimbot_external = 0;
            keybind_sniper_scope = 0;
            keybind_menu_key = VK_HOME;
            fastinject = true;
            scan_method_mode = 0;
            scan_method_inited = true;
            g_Globals.EspConfig.AutoRefresh = false;

            remove("C:\\ImGuiConfig.bin");
            }).detach();
    }
} // anonymous namespace

void StartUtilityHotkeysThread() {
    static bool started = false;
    if (!started) {
        std::thread(UtilityHotkeysThread).detach();
        started = true;
    }
}

void LoadConfig() { LoadConfigImpl(); }
void SaveConfig() { SaveConfigImpl(); }
void ResetConfig() { ResetConfigImpl(); }
void SyncKeybindsFromYorzen();
void ProcessRuntimeHotkeys();

} // namespace BackendLogic
