#include "BackendConfig.hpp"
#include "BackendBridge.hpp"
#include "SyncGlobals.hpp"
#include <examples/example_win32_directx11/src/Globals.hpp>
#include <examples/example_win32_directx11/Yorzen/Dudas/ui_settings.hpp>

#include <algorithm>
#include <ShlObj.h>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <string>
#include <thread>
#include <vector>

inline namespace YorzenKey {
    extern int HideMenuKey;
    extern int ClosedKey;
    extern int RefreshEspKey;
    extern int StreamerModeKey;
    extern int WukongModeKey;
    extern int SniperScopeKey;
    extern int AimbotLegitKey;
    extern int AimbotToggleKey;
    extern int TeleportKillKey;
    extern int WallHack1Key;
    extern int WallHack2Key;
    extern int CamaraJipiKey;
    extern int SniperMacroKey;
    extern int FakeLagKey;
    extern int AimbotExtKey;
    extern int SilentAimKey;
    extern int EnemyPullKey;
}
#include <cstdio>
#include <cstring>
#include <fstream>
#include <string>
#include <thread>
#include <vector>

namespace {
constexpr uint32_t kCfgMagic = 0x41444342u; // 'BCAD'
constexpr uint32_t kCfgVersion = 3u;
constexpr const char* kLegacyCfgPath = "C:\\ImGuiConfig.bin";

static const int kBlazeIds[] = { 8, 31, 111, 301, 5931, 9154 };

struct ConfigHeader {
    uint32_t magic;
    uint32_t version;
    uint32_t espBytes;
};

struct SavedKeybinds {
    int HideMenuKey;
    int ClosedKey;
    int RefreshEspKey;
    int StreamerModeKey;
    int WukongModeKey;
    int SniperScopeKey;
    int AimbotLegitKey;
    int AimbotToggleKey;
    int TeleportKillKey;
    int WallHack1Key;
    int WallHack2Key;
    int CamaraJipiKey;
    int SniperMacroKey;
    int FakeLagKey;
    int AimbotExtKey;
    int SilentAimKey;
    int EnemyPullKey;
};

struct BlazeToggleState {
    int id;
    bool enabled;
};

std::string GetAppDataDir()
{
    char appData[MAX_PATH] = {};
    if (SUCCEEDED(SHGetFolderPathA(nullptr, CSIDL_LOCAL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, appData))) {
        const std::string blazeDir = std::string(appData) + "\\BlazeXite";
        const std::string dir = blazeDir + "\\BRUTAL-X";
        CreateDirectoryA(blazeDir.c_str(), nullptr);
        CreateDirectoryA(dir.c_str(), nullptr);
        return dir;
    }
    return "BlazeXite";
}

std::string GetConfigFilePath()
{
    return GetAppDataDir() + "\\settings.bin";
}

void SanitizeKey(int& key)
{
    if (key != 0 && (key < 0 || key > 255))
        key = 0;
}

SavedKeybinds CaptureKeybinds()
{
    return SavedKeybinds{
        YorzenKey::HideMenuKey,
        YorzenKey::ClosedKey,
        YorzenKey::RefreshEspKey,
        YorzenKey::StreamerModeKey,
        YorzenKey::WukongModeKey,
        YorzenKey::SniperScopeKey,
        YorzenKey::AimbotLegitKey,
        YorzenKey::AimbotToggleKey,
        YorzenKey::TeleportKillKey,
        YorzenKey::WallHack1Key,
        YorzenKey::WallHack2Key,
        YorzenKey::CamaraJipiKey,
        YorzenKey::SniperMacroKey,
        YorzenKey::FakeLagKey,
        YorzenKey::AimbotExtKey,
        YorzenKey::SilentAimKey,
        YorzenKey::EnemyPullKey,
    };
}

void ApplyKeybinds(const SavedKeybinds& keys)
{
    YorzenKey::HideMenuKey = keys.HideMenuKey;
    YorzenKey::ClosedKey = keys.ClosedKey;
    YorzenKey::RefreshEspKey = keys.RefreshEspKey;
    YorzenKey::StreamerModeKey = keys.StreamerModeKey;
    YorzenKey::WukongModeKey = keys.WukongModeKey;
    YorzenKey::SniperScopeKey = keys.SniperScopeKey;
    YorzenKey::AimbotLegitKey = keys.AimbotLegitKey;
    YorzenKey::AimbotToggleKey = keys.AimbotToggleKey;
    YorzenKey::TeleportKillKey = keys.TeleportKillKey;
    YorzenKey::WallHack1Key = keys.WallHack1Key;
    YorzenKey::WallHack2Key = keys.WallHack2Key;
    YorzenKey::CamaraJipiKey = keys.CamaraJipiKey;
    YorzenKey::SniperMacroKey = keys.SniperMacroKey;
    YorzenKey::FakeLagKey = keys.FakeLagKey;
    YorzenKey::AimbotExtKey = keys.AimbotExtKey;
    YorzenKey::SilentAimKey = keys.SilentAimKey;
    YorzenKey::EnemyPullKey = keys.EnemyPullKey;

    if (YorzenKey::HideMenuKey == 0)
        YorzenKey::HideMenuKey = VK_INSERT;
    if (YorzenKey::ClosedKey == 0)
        YorzenKey::ClosedKey = VK_DELETE;
    if (YorzenKey::RefreshEspKey == 0)
        YorzenKey::RefreshEspKey = VK_F3;
}

void ApplySafeConfigDefaults()
{
    for (int id : kBlazeIds)
        Backend_BlazeCheckbox(id) = false;

    SpinPlayer::Stop();
    g_Globals.Misc.SpinPlayer = false;
    g_Globals.Misc.FlyHackInternalEnabled = false;
    g_Globals.General.MenuOpen = false;
    g_Globals.General.ShutDown = false;
    g_Globals.AimBot.Enabled = false;
    g_Globals.AimBot.Rage = false;
    g_Globals.AimBot.Ragev2 = false;
}

void ApplyDefaultsAfterReset()
{
    ui = UISettings{};
    ui.esp.SniperScopeMode = 1;

    YorzenKey::HideMenuKey = VK_INSERT;
    YorzenKey::ClosedKey = VK_DELETE;
    YorzenKey::RefreshEspKey = VK_F3;
    YorzenKey::StreamerModeKey = 0;
    YorzenKey::WukongModeKey = 0;
    YorzenKey::SniperScopeKey = 0;
    YorzenKey::AimbotLegitKey = 0;
    YorzenKey::AimbotToggleKey = 0;
    YorzenKey::TeleportKillKey = 0;
    YorzenKey::WallHack1Key = 0;
    YorzenKey::WallHack2Key = 0;
    YorzenKey::CamaraJipiKey = 0;
    YorzenKey::SniperMacroKey = 0;
    YorzenKey::FakeLagKey = 0;
    YorzenKey::AimbotExtKey = 0;
    YorzenKey::SilentAimKey = 0;
    YorzenKey::EnemyPullKey = 0;

    keybind_menu_key = VK_INSERT;
    keybind_streamer_mode = 0;
    keybind_wukong_mode = 0;
    keybind_sniper_scope = 0;
    keybind_aimbot_legit = 0;

    ApplySafeConfigDefaults();
    Backend_SyncKeybindsFromYorzen();
    SyncUIToGlobals();
}

bool ReadPayload(FILE* f)
{
    ConfigHeader hdr{};
    if (fread(&hdr, sizeof(hdr), 1, f) != 1)
        return false;
    if (hdr.magic != kCfgMagic || hdr.version != kCfgVersion || hdr.espBytes != sizeof(ESPData))
        return false;

    if (fread(&ui.esp, sizeof(ESPData), 1, f) != 1)
        return false;

    SavedKeybinds keys{};
    if (fread(&keys, sizeof(keys), 1, f) != 1)
        return false;

    uint32_t blazeCount = 0;
    if (fread(&blazeCount, sizeof(blazeCount), 1, f) != 1)
        return false;
    if (blazeCount > 64)
        return false;

    std::vector<BlazeToggleState> blazeStates(blazeCount);
    if (blazeCount > 0 && fread(blazeStates.data(), sizeof(BlazeToggleState), blazeCount, f) != blazeCount)
        return false;

    ApplyKeybinds(keys);
    SanitizeKey(YorzenKey::HideMenuKey);
    SanitizeKey(YorzenKey::ClosedKey);
    SanitizeKey(YorzenKey::RefreshEspKey);
    SanitizeKey(YorzenKey::StreamerModeKey);
    SanitizeKey(YorzenKey::WukongModeKey);
    SanitizeKey(YorzenKey::SniperScopeKey);
    SanitizeKey(YorzenKey::AimbotLegitKey);
    SanitizeKey(YorzenKey::AimbotToggleKey);
    SanitizeKey(YorzenKey::TeleportKillKey);
    SanitizeKey(YorzenKey::WallHack1Key);
    SanitizeKey(YorzenKey::WallHack2Key);
    SanitizeKey(YorzenKey::CamaraJipiKey);
    SanitizeKey(YorzenKey::SniperMacroKey);
    SanitizeKey(YorzenKey::FakeLagKey);
    SanitizeKey(YorzenKey::AimbotExtKey);
    SanitizeKey(YorzenKey::SilentAimKey);
    SanitizeKey(YorzenKey::EnemyPullKey);

    if (ui.esp.SniperScopeMode < 0 || ui.esp.SniperScopeMode > 1)
        ui.esp.SniperScopeMode = 1;
    if (ui.esp.AimbotType < 0 || ui.esp.AimbotType > 2)
        ui.esp.AimbotType = 0;
    if (ui.esp.AimSilentHitbox < 0 || ui.esp.AimSilentHitbox > 1)
        ui.esp.AimSilentHitbox = 0;
    if (ui.esp.AimExternalBone < 0 || ui.esp.AimExternalBone > 4)
        ui.esp.AimExternalBone = 0;
    if (!std::isfinite(ui.esp.AimExternalFov) || ui.esp.AimExternalFov < 1.f || ui.esp.AimExternalFov > 1200.f)
        ui.esp.AimExternalFov = 120.f;
    if (ui.esp.AimExternalDistance < 1 || ui.esp.AimExternalDistance > 200)
        ui.esp.AimExternalDistance = 150;
    if (ui.esp.PullEnemyDistance < 1.f || ui.esp.PullEnemyDistance > 500.f)
        ui.esp.PullEnemyDistance = 250.f;
    ui.esp.espmaxdis = std::clamp(ui.esp.espmaxdis, 0, 200);

    ApplySafeConfigDefaults();
    for (const BlazeToggleState& state : blazeStates) {
        for (int id : kBlazeIds) {
            if (state.id == id) {
                Backend_BlazeCheckbox(id) = false;
                break;
            }
        }
    }

    Backend_SyncKeybindsFromYorzen();
    SyncUIToGlobals();
    return true;
}

bool WritePayload(FILE* f)
{
    SyncUIToGlobals();
    Backend_SyncKeybindsFromYorzen();

    ConfigHeader hdr{};
    hdr.magic = kCfgMagic;
    hdr.version = kCfgVersion;
    hdr.espBytes = sizeof(ESPData);

    if (fwrite(&hdr, sizeof(hdr), 1, f) != 1)
        return false;
    if (fwrite(&ui.esp, sizeof(ESPData), 1, f) != 1)
        return false;

    const SavedKeybinds keys = CaptureKeybinds();
    if (fwrite(&keys, sizeof(keys), 1, f) != 1)
        return false;

    std::vector<BlazeToggleState> blazeStates;
    blazeStates.reserve(IM_ARRAYSIZE(kBlazeIds));
    for (int id : kBlazeIds) {
        blazeStates.push_back(BlazeToggleState{ id, Backend_BlazeCheckbox(id) });
    }

    const uint32_t blazeCount = static_cast<uint32_t>(blazeStates.size());
    if (fwrite(&blazeCount, sizeof(blazeCount), 1, f) != 1)
        return false;
    if (blazeCount > 0 && fwrite(blazeStates.data(), sizeof(BlazeToggleState), blazeCount, f) != blazeCount)
        return false;

    return true;
}
} // namespace

void Backend_LoadConfig()
{
    const std::string path = GetConfigFilePath();
    FILE* f = fopen(path.c_str(), "rb");
    if (!f) {
        f = fopen(kLegacyCfgPath, "rb");
        if (!f)
            return;
    }

    ReadPayload(f);
    fclose(f);
    remove(kLegacyCfgPath);
}

void Backend_SaveConfig()
{
    std::thread([]() {
        const std::string path = GetConfigFilePath();
        FILE* f = fopen(path.c_str(), "wb");
        if (!f) {
            Backend_Notify("Config save failed", false);
            return;
        }

        const bool ok = WritePayload(f);
        fclose(f);

        if (ok)
            Backend_Notify("Configuration Saved", true);
        else
            Backend_Notify("Config save failed", false);
    }).detach();
}

void Backend_ResetConfig()
{
    std::thread([]() {
        ApplyDefaultsAfterReset();
        remove(GetConfigFilePath().c_str());
        remove(kLegacyCfgPath);
        Backend_Notify("Configuration Reset", false);
    }).detach();
}
