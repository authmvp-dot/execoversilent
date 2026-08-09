#include <src/Backend/SyncGlobals.hpp>
#include <src/Backend/BackendBridge.hpp>
#include <src/Globals.hpp>
#include <Yorzen/Dudas/ui_settings.hpp>
#include <algorithm>
#include <d3d11.h>
#include <imgui.h>

std::string MemoryLogs;

int keybind_menu_key = VK_INSERT;
int keybind_streamer_mode = 0;
int keybind_wukong_mode = 0;
int keybind_aimbot_external = 0;
int keybind_sniper_scope = 0;
int keybind_aimbot_legit = 0;

namespace texture {
    ID3D11ShaderResourceView* custom_logo = nullptr;
}

static ImColor ToImColor(const float col[4]) {
    return ImColor(col[0], col[1], col[2], col[3]);
}

void SyncUIToGlobals() {
    Backend_SyncKeybindsFromYorzen();

    const ESPData& e = ui.esp;
    Globals& g = g_Globals;

    g.AimBot.Enabled = false;
    g.AimBot.Rage = false;
    g.AimBot.Ragev2 = false;
    aimbot_type = e.AimbotType;
    aimbot_master_enabled = e.AimbotEnabled;
    if (e.AimbotEnabled) {
        switch (e.AimbotType) {
        case 0: g.AimBot.Enabled = true; break;
        case 1: g.AimBot.Rage = true; break;
        case 2:
            if (keybind_aimbot_legit != 0 && (GetAsyncKeyState(keybind_aimbot_legit) & 0x8000)) {
                g.AimBot.Enabled = true;
                g.AimBot.KeyBind = keybind_aimbot_legit;
            }
            else {
                g.AimBot.Enabled = false;
                g.AimBot.KeyBind = VK_LBUTTON;
            }
            break;
        }
    }
    else {
        g.AimBot.KeyBind = VK_LBUTTON;
    }

    g.AimBot.IgnoreKnocked = e.IgnoreKnockedEntity;
    g.AimBot.IgnoreBots = e.IgnoreBots;
    g.AimBot.NoRecoil = e.NoRecoil;
    g.AimBot.DistanceAim = e.AimbotDistance;

    g.AimBot.SilentAimCheckbox = e.AimSilentEnabled;
    g.AimBot.SilentAimHitboxMode = e.AimSilentHitbox;
    if (e.AimSilentEnabled) {
        g.AimBot.SilentAimMax = (e.AimSilentHitbox == 0);
        g.AimBot.SilentAimBody = (e.AimSilentHitbox == 1);
        g.AimBot.SilentAimReworked = g.AimBot.SilentAimMax;
    }
    else {
        g.AimBot.SilentAimMax = false;
        g.AimBot.SilentAimBody = false;
        g.AimBot.SilentAimReworked = false;
    }

    g.AimBot.ExternalEnabled = e.AimExternalEnabled;
    if (ui.esp.AimExternalBone < 0 || ui.esp.AimExternalBone > 4)
        ui.esp.AimExternalBone = 0;
    g.AimBot.ExternalBone = ui.esp.AimExternalBone;
    g.AimBot.ExternalFov = (e.AimExternalFov > 0.f) ? e.AimExternalFov : 120.f;
    g.AimBot.ExternalDistance = (e.AimExternalDistance > 0) ? e.AimExternalDistance : 150;
    g.AimBot.ExternalKey = e.AimExternalKey;
    g.AimBot.ExternalBind = VK_LBUTTON;
    g.AimBot.ExternalIgnoreKnocked = e.IgnoreKnockedEntity;
    g.AimBot.IgnoreTrainingBots = e.IgnoreTrainingBots;

    g.Misc.ShowAimbotFov = e.AimFovEnabled;
    g.AimBot.Fov = e.AimFovValue;
    g.Misc.AimbotFovColor[0] = e.AimFovColor[0];
    g.Misc.AimbotFovColor[1] = e.AimFovColor[1];
    g.Misc.AimbotFovColor[2] = e.AimFovColor[2];
    g.Misc.AimbotFovColor[3] = e.AimFovColor[3];

    g.Misc.EnemyPullEnabled = e.PullEnemyEnabled;
    g.Misc.PullEnemy360Enabled = e.PullEnemyEnabled;
    g.Misc.PullEnemy360Mode = e.PullEnemyType;
    g.Misc.EnemyPullMaxDistance = e.PullEnemyDistance;
    g.Misc.PullEnemy360MaxDistance = e.PullEnemyDistance;
    g.Misc.AutoFire = e.AutoFireEnabled;

    g.Misc.FastReload = e.FastReload;
    g.Misc.FastFire = e.BurstFire;
    g.Misc.SpeedTimerEnabled = e.SpeedTimerEnabled;
    g.Misc.SniperScope = e.SniperScopeEnabled;
    g.Misc.SniperScopeMode = e.SniperScopeMode;
    g.Misc.VisionHackEnabled = e.VisionHackEnabled;
    g.Misc.DownPlayer = e.DownPlayerEnabled;
    g.Misc.SpinPlayer = e.SpinPlayerEnabled;
    g.Misc.SpinPlayerSpeed = e.SpinPlayerSpeed;

    static bool s_prevSpinPlayer = false;
    if (e.SpinPlayerEnabled != s_prevSpinPlayer) {
        if (e.SpinPlayerEnabled)
            SpinPlayer::Start();
        else
            SpinPlayer::Stop();
        s_prevSpinPlayer = e.SpinPlayerEnabled;
    }

    static bool s_prevNoGravityFly = false;
    g.Misc.NoGravityFlyEnabled = e.NoGravityFlyEnabled;
    if (e.NoGravityFlyEnabled != s_prevNoGravityFly) {
        if (e.NoGravityFlyEnabled) {
            g.Misc.FlyHackInternalEnabled = false;
            FlyHack_LocalPlayer::Stop();
            NoGravityFly::Start();
        }
        else {
            NoGravityFly::Stop();
        }
        s_prevNoGravityFly = e.NoGravityFlyEnabled;
    }
    else if (e.NoGravityFlyEnabled) {
        // Ensure thread is running (Start is no-op if already active)
        NoGravityFly::Start();
    }    g.Visuals.Enabled = e.ESPMasterEnabled;
    g.Visuals.Lines = e.ESPLineEnabled;
    g.Visuals.EspLines = e.ESPLineStartPos;
    g.Visuals.SnapLinesColor = ToImColor(e.ESPLineColor);
    g.Visuals.LineGlow = false;
    g.Visuals.LineGlowRadius = e.ESPLineGlowRadius;
    g.Visuals.ShowLogo = e.ShowLineLogo ? 1 : 0;

    g.Visuals.Box = e.ESPBoxEnabled;
    g.Visuals.players_box = e.ESPBoxMode;
    g.Visuals.BoxColor = ToImColor(e.ESPBoxColor);
    g.Visuals.BoxGlow = false;
    g.Visuals.BoxGlowRadius = e.ESPBoxGlowRadius;
    g.Visuals.fillBox = e.ESPBoxFill;
    g.Visuals.fillBoxColor = ToImColor(e.ESPBoxFillColor);

    g.Visuals.Skeleton = e.ESPBoneEnabled;
    g.Visuals.SkeletonColor = ToImColor(e.ESPBoneColor);
    g.Visuals.SkeletonGlow = false;
    g.Visuals.SkeletonGlowRadius = e.ESPBoneGlowRadius;

    g.Visuals.HealthBar = e.PlayerHealthBar;
    g.Visuals.Name = e.PlayerNameEnabled;
    g.Visuals.NameColor = ToImColor(e.PlayerNameColor);
    g.Visuals.Distance = e.PlayerDistanceEnabled;
    g.Visuals.DistanceColor = ToImColor(e.PlayerDistanceColor);
    g.Visuals.WeaponName = e.PlayerWeaponNameEnabled;
    g.Visuals.WeaponColor = ToImColor(e.PlayerWeaponNameColor);
    g.Visuals.WeaponIcon = e.PlayerWeaponIcon;
    g.Visuals.Rank = e.RankEnabled;
    g.Visuals.RankColor = ToImColor(e.RankColor);
    g.Visuals.SnapLines = e.SnapLinesEnabled;
    g.Visuals.OriginLineColor = ToImColor(e.SnapLinesColor);
    g.Visuals.RainbowESP = e.RainbowESP;
    g.Visuals.IgnoreTrainingBots = e.IgnoreTrainingBots;
    g.Visuals.RenderDistance = std::clamp(e.espmaxdis, 0, 200);
    g.Visuals.InvalidTimer = e.InvalidTimer;

    g.EspConfig.showOnlyVisible = e.WukongMode;
    g.Loot.Enabled = false;
    g.Loot.RenderDistance = e.LootRenderDistance;
    g.Loot.ShowPicker = e.LootShowPicker;

    g.General.Capture = e.EspStreamerMode;
    g.General.DisableAllEffects = e.DisableAllEffects;
}

void SyncGlobalsToUI() {
}
