#pragma once
#include <Windows.h>
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui.h>
#include <EspLines/Player.h>
#include <EspLines/Math/Matrix4v4.hpp>
#include <EspLines/Loot/LootTypes.hpp>
#include <unordered_map>
#include <string>
#include <vector>

static int auth_tab = 0;
static bool aimbot_master_enabled = false;
static bool previous_aimbot_state = false;
static int aimbot_type = 0; // 0 = Visible, 1 = Rage, 2 = Legit
static const char* aimbot_items[] = { "Aimbot Visible", "Aimbot Rage", "Aimbot Legit" };
static const char* forceAimHitboxModes[] = { "Head", "Body" };
static int keybind_aimbot = 0;
static int master_aimbot_state = 0;
static int menu_state = 0;
static int keybind_hyper_movement = 0;
static int keybind_camera_horizontal = 0;
static int keybind_camera_vertical = 0;
static int keybind_shake_kill = 0;
static int keybind_up_player = 0;
static char loginUser[64] = "";
static char loginPass[64] = "";
static bool showLoginError = false;
// Bright Red for a warning/aggressive "glowing" look
const float BUTTON_WIDTH = 200.0f;
const float BUTTON_HEIGHT = 33.0f;

static int Aimbotype = 0;
static float aimlegit = 0.05f;
static int entity_visible = 0;
static int keybind_visible = 0;
static int entity_rage = 0;
static int keybind_rage = 0;
static int keybind_telekill = 0;
static int entity_ultra = 0;
static int keybind_ultra = 0;
static int entity_body = 0;
static int keybind_body = 0;
static int entity_pure = 0;
static int keybind_pure = 0;
static int keybind_down_player = 0;
static int keybind_speed_timer = 0;

// Added universally-requested hotkey components
static int keybind_speed_hack = 0;
static int keybind_wall_hack = 0;
static int keybind_fast_landing = 0;
static int keybind_camera_right = 0;
static int keybind_sniper_switch = 0;
static int keybind_refresh_esp = VK_F3; // Mirror AotForms default
static int keybind_fly_hack_internal = 0;
static int keybind_burst_fire = 0;
extern int keybind_wukong_mode;
extern int keybind_aimbot_external;
extern int keybind_sniper_scope;

extern int keybind_menu_key;
extern int keybind_streamer_mode;
extern int keybind_aimbot_legit;

namespace FlyHack_LocalPlayer {
	void Start();
	void Stop();
	void ApplyFly();
}

namespace SpinPlayer {
	void Start();
	void Stop();
}

namespace NoGravityFly {
	void Start();
	void Stop();
}


class Globals {
public:
	struct AimBot {
		bool NoRecoil;
		bool Ultra;
		bool LegitAimbot;
		bool Enabled;
		bool NeckA;
		bool RightShoulderA;
		bool IgnoreKnocked;
		bool IgnoreBots;
		bool VisibleOnly;
		int DistanceAim = 100;
		float Fov = 1200.0f;
		int KeyBind = VK_LBUTTON;
		int AimPosition = 0;
		bool SilentAimCheckbox = false;
		bool SilentAimReworked = false; // legacy alias Ã¢â‚¬â€ prefer SilentAimMax
		bool SilentAimMax = false;      // Max (Head)
		bool SilentAimBody = false;     // Body (Hip)
		int SilentAimTargetMode = 0; // unused (kept for config layout)
		int SilentAimHitboxMode = 0; // 0 = Head, 1 = Body
		float SilentAimFov = 1200.0f;
		float Speed = 1.0f;
		bool Rage;
		bool Ragev2;
		int AimbotModes;
		float SmoothingFactor;
		float Smoothness;


		bool ExternalEnabled = false;
		bool ExternalAlwaysOn = false;
		bool ExternalUseAimAssist = false; // Ã°Å¸â€Â¥ new (aimassist mode toggle)
		int ExternalWriteDelayMs = 0; // 0 Ã¢â€ â€™ 3000 ms

		int ExternalKey = 0;
		int ExternalBind = VK_LBUTTON;
		int ExternalBone = 1;
		int ExternalDelayMin = 5;
		int ExternalDelayMax = 20;
		int ExternalDistance = 150;

		float ExternalFov = 120.f;


		bool ExternalIgnoreKnocked = false;
		bool IgnoreTrainingBots = false;

	} AimBot;

	struct Visuals
	{
		bool Enabled = true;
		bool Lines = false;
		bool SnapLines = false;
		int EspLines = 0;
		int ShowLogo = 1; // 0 = hide line logo, 1 = show at snapline anchor
		ImColor SnapLinesColor = ImColor(255, 255, 255, 255);
		ImColor OriginLineColor = ImColor(255, 255, 255, 255);

		bool Box = false;
		int players_box = 1;
		ImColor BoxColor = ImColor(255, 255, 255, 255);
		bool fillBox = false;
		ImColor fillBoxColor = ImColor(0, 0, 0, 0);

		bool Name = false;
		ImColor NameColor = ImColor(255, 255, 255, 255);

		bool Distance = false;
		ImColor DistanceColor = ImColor(255, 255, 255, 255);

		bool Rank = false;
		ImColor RankColor = ImColor(255, 215, 0, 255);
		int EspRankSide = 2;

		bool HealthBar = false;
		int HealthBarPosition = 2;
		int players_healthbar = 0;
		int EspNameSide = 2;
		int EspDistanceSide = 3;
		int EspWeaponIconSide = 3;
		int EspWeaponTextSide = 2;

		bool Skeleton = false;
		ImColor SkeletonColor = ImColor(255, 255, 255, 255);

		bool WeaponName = false;
		bool WeaponIcon = false;
		ImColor WeaponColor = ImColor(255, 255, 255, 255);
		int WeaponInfo = 1; // 0=top, 1=bottom, 2=left, 3=right

		bool RainbowESP = false;
		bool IgnoreTrainingBots = false;
		ImColor KnockedEnemiesColor = ImColor(255, 0, 0, 255);
		float EspLineLogoSize = 64.f;
		int RenderDistance = 160;
		bool ShowActiveSkills = false;

		bool InvalidTimer = false;

		bool LineGlow = false;
		float LineGlowRadius = 12.f;
		bool BoxGlow = true;
		float BoxGlowRadius = 12.f;
		bool SkeletonGlow = false;
		float SkeletonGlowRadius = 14.f;

	} Visuals;

	struct Misc
	{
		int entitySpeed = 0;
		int speedOption = 0;

		bool UpPlayer;
		bool TeleKill = false;
		float TeleKillKeepDistance = 1.0f;
		bool BodyAimbot;
		bool BodyAimbotV2;
		float UpPlayerLift = 1.0f; // Lift amount (0.5 = 0.75m, 1.0 = 1.5m, max 1.5m) 
		int UpPlayerTickMs = 8;
		float UpPlayerStrength = 1.0f; // separate smoothing strength for UpPlayer
		bool CameraRight = false; // shift camera right
		bool CameraLeft = false;  // shift camera left
		float CameraUp = 0.0f; // camera up/down offset (default: normal)
		float CameraRightLeft = 0.0f; // camera right/left offset (default: normal)
		float CameraBack = 0.0f; // camera back/forward offset (default: normal)
		bool TeleScope; // Teleport target to crosshair when scoping (RMB)
		bool ShowAimbotFov = false;
		bool SniperScope;
		int  SniperScopeMode = 1;
		float SniperFov = 500.0f;
		bool FastReload = false; // Fast reload / No Reload (default off)
		bool ScopeTracking; // Scope tracking feature - track enemy body when scoped
		float ScopeTrackingFov = 1.0f; // FOV for scope tracking
		bool HeadTracking; // Head tracking feature - track enemy heads without scope
		bool UltraSwitch; // Ultra weapon switch feature - bypasses animations

		float AimbotFovColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		bool ShakeKill;
		float ShakeKillIntensity = 1.0f; // Shake intensity (0.5 = weak, 2.0 = strong)
		float ShakeKillDistance = 1.0f; // Maximum distance to shake enemy (meters, max 3.0m)
		bool SpawnKill;
		bool FastFire;
		bool AutoFire = false;
		bool StealthCameraHack;
		bool GhostHack; // Makes player invisible to other players

		// Additional exploit features
		bool DiveKillEnabled = false; // Dive kill feature
		bool HighJumpEnabled = false; // High jump / Fly exploit
		bool SpeedTeleportEnabled = false; // Speed teleport feature
		bool DownPlayer = false; // Down player feature
		bool FlyHackEnabled = false; // Fly hack feature
		bool FlyHackInternalEnabled = false; // Fly hack Internal feature
		bool MapTeleportEnabled = false; // Map teleport feature

		bool RemoveFireDelay = false;

		// Speed Hack - Code-based speed that persists across games (includes run, walk, and falling speed)
		bool SpeedHack = false;
		bool OffsetSpeed = false; // Sync flag
		float SpeedValue = 1.0f; // Speed multiplier (1x to 10x) - applies to all movement types

		// Speed Timer Ã¢â‚¬â€ fixed delta on/off (0.055 / 0.033)
		bool SpeedTimerEnabled = false;
		float SpeedTimerMultiplier = 1.0f; // legacy config field (unused)

		// Fast Medkit - Makes medkit use faster (with healing rate boost)
		bool FastMedkit = false;
		float FastMedkitMultiplier = 1.23f; // Medkit speed multiplier (1.0x to 3.0x, higher = faster)

		// PlayerAttributes / FollowCamera hacks (fixed values, checkbox only)
		bool VisionHackEnabled = false;

		// Camera Hack - Horizontal & Vertical camera offsets
		bool CameraHack = false;                  // Horizontal
		float CameraOffset = 0.0f;               // Horizontal offset in degrees (-45 to +45, negative = left, positive = right)
		bool CameraVertical = false;             // Vertical
		float CameraVerticalOffset = 0.0f;       // Vertical offset in degrees (-45 to +45, negative = down, positive = up)



		// No Bullet Spread - Completely removes bullet spread (sets scatter to 0.0)
		bool NoBulletSpread = false;
		bool ForceAim = false;
		int  ForceAimMode = 0; // 0 = Head (default), 1 = Body (Hip)
		float ForceAimMaxDistance = 200.0f;
		float ForceAimMaxPull = 8.0f; // max pull length (m) from lock; no pull if aim needs more
		float ForceAimMaxPullVertical = 1.0f; // max up/down from lock; no pull if exceeded

		// Enemy Pull (FOV pull onto fire line) + legacy magnet aliases
		bool EnemyPullEnabled = false;
		bool PullEnemy360Enabled = false; // legacy alias Ã¢â‚¬â€ prefer EnemyPullEnabled
		int PullEnemy360Mode = 0; // unused (kept for config layout)
		int EnemyPullTickMs = 6;
		float EnemyPullMaxDistance = 250.0f;
		int PullEnemy360TickMs = 6; // legacy alias
		float PullEnemy360MaxDistance = 250.0f; // legacy alias

		// Bullet scatter fix
		bool BulletScatterFix = false;
		float BulletScatterScale = 1.0f;

		// Speed Teleport settings
		float SpeedTeleportRange = 200.0f;
		float SpeedTeleportSpeed = 2.0f;

		// Down Player settings
		float DownPlayerSpeed = 0.5f;

		// Teleport Mark settings
		Vector3 SavedMark = Vector3::Zero();
		bool HasMarkedPosition = false;
		bool TeleportMarkEnabled = false;

		// Map Teleport settings
		int SelectedLocationIndex = 0;
		Vector3 teleport_coordinates[20];  // Array of 20 teleport coordinates
		char teleport_locations[20][64];  // Array of 20 location names

		// Spin player multiplier 1xÃ¢â‚¬â€œ15x (see SpinPlayer::DegreesPerTickFromMultiplier)
		bool SpinPlayer = false;
		float SpinPlayerSpeed = 5.0f;

		bool UnlimitedAmmo = false;

		// No gravity fly (WASD + Space/Ctrl) Ã¢â‚¬â€ movement 0x139C, grounded 0x13F0
		bool NoGravityFlyEnabled = false;

	} Misc;



	struct General
	{
		bool ShutDown = false;
		bool Capture = false;
		bool AutoColorChange = true;
		bool AutoSaveConfig = false;
		bool DisableAllEffects = false; // Performance mode Ã¢â‚¬â€ disables glow, shaders & heavy visuals
		bool MenuOpen = false; // synced from overlay UI Ã¢â‚¬â€ blocks game input hacks while menu is open
		int Delay = 1;
		char Username[64] = "";
		char PassWord[64] = "";
		char License[128] = "";
	} General;

	struct GameMode
	{
		int CurrentMode = 0; // 0=Unknown, 1=BR, 2=CS, 3=LoneWolf
		int MatchStatus = 0; // Match status from game
		bool IsInMatch = false;
		bool IsBR = false;
		bool IsCS = false;
		bool IsLoneWolf = false;
		std::string ModeName = "Unknown";

		// Match Statistics
		int TotalMatches = 0; // Total matches played since cheat started
		int BRMatches = 0; // Battle Royale matches
		int CSMatches = 0; // Clash Squad matches  
		int LoneWolfMatches = 0; // Lone Wolf matches
		int Wins = 0; // Total wins
		int Losses = 0; // Total losses
		float WinRate = 0.0f; // Win rate percentage

		// Detection Control
		int LastEntityCount = 0; // Last entity count when mode was detected

		// Stats Menu Control
		bool ShowStatsMenu = false; // Show bottom-left stats menu
	} GameMode;

	struct LootConfig {
		bool Enabled = false;
		bool ShowPicker = true;
		bool ShowDistanceOnLabel = true;
		int RenderDistance = 150;
		std::vector<GroundLootEntry> GroundLoot;
		std::unordered_map<uint32_t, bool> ItemFilters;

		int EnabledFilterCount() const {
			int n = 0;
			for (const auto& kv : ItemFilters) {
				if (kv.second)
					++n;
			}
			return n;
		}

		bool HasActiveFilter() const {
			return EnabledFilterCount() > 0;
		}

		bool IsItemFiltered(uint32_t itemId) const {
			const auto it = ItemFilters.find(itemId);
			return it != ItemFilters.end() && it->second;
		}

		void SetItemFilter(uint32_t itemId, bool enabled) {
			if (enabled)
				ItemFilters[itemId] = true;
			else
				ItemFilters.erase(itemId);
		}
	} Loot;

	struct Esp {
		std::unordered_map<long, Player> Entities;
		Matrix4x4 ViewMatrix{};
		Vector3 MainCamera{};
		uint32_t LocalPlayer = 0;
		uint32_t previousCount = 0;
		bool Matrix = false;
		int Width = 0;
		int Height = 0;
		Vector3 playerpos;
		bool showOnlyVisible = false;
		bool Refresh = false;
		bool AutoRefresh = false;
		int visibleEntityCount = 0;  // Count of visible enemies
	} EspConfig;

};

inline Globals g_Globals;
