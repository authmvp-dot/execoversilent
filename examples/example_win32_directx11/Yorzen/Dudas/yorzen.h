#pragma once

#include <imgui.h>
#include <Windows.h>
#include <string>
#include <vector>
#include <unordered_map>

std::string nameop;

inline namespace YorzenMain
{
	bool Auth = false;
	bool Login = false;
	bool Login_Window = true;
	bool Verify = true;

	bool WaterMark;
	bool EspMode;
	bool EnabledEsp;

	bool Status;
}

namespace var {
	char username[65] = { "" };
	char password[65] = { "" };
	char keyer[65] = { "" };
}

static bool Remember = true;
static bool logn;
static bool regst;

inline namespace YorzenName
{
	const char* cheat_name = "BLAZE";
	const char* cheat_version = "X";
}

inline namespace YorzenParticle
{
	bool Enabldparticle;
}

inline namespace YorzenEtc
{
	bool ModoStreamer;
	bool BlockInternet;
}

inline namespace YorzenActive
{
	bool FlyWallCheck;
	bool WallHack1Check;
	bool WallHack2Check;
	bool WallHack3Check;
	bool WallHack4Check;

	bool TPInfinitoC;
	bool SilentLagC;

	bool WallHackOffset;

	bool WallHackAC;
}

inline namespace YorzenKey
{
	bool HideMenuCheck = true;
	bool AimbotExtCheck;
	bool FakeLagCheck;
	bool FreezeLagCheck;
	bool SilentLagCheck;
	bool TpInfinitoCheck;
	bool GhostLagCheck;
	bool ClosedCheck;

	bool FlyWallCheck;
	bool FlyWall1Check;
	bool FlyWall2Check;
	bool WallHack1Check;
	bool WallHack2Check;
	bool WallHack3Check;
	bool WallHack4Check;
	bool WallHack5Check;

	bool SPJoystickCheck;
	bool SpeedCheck;
	bool MagnetCheck;
	bool TpBaseCheck;
	bool TpEnemyCheck;
	bool UpCheck;
	bool TeleKCheck;
	bool PullCheck;
	bool CamaraJipiCheck;
	bool TeleportKillCheck = false;
	bool AimbotToggleCheck = false;
	bool SniperMacroCheck = false;
	bool RefreshEspCheck = true;
	bool StreamerModeCheck = false;
	bool WukongModeCheck = false;
	bool SniperScopeCheck = false;
	bool AimbotLegitCheck = true;
	bool SilentAimCheck = false;
	bool EnemyPullCheck = false;

	int HideMenuKey = VK_INSERT;
	int RefreshEspKey = VK_F3;
	int StreamerModeKey = 0;
	int WukongModeKey = 0;
	int SniperScopeKey = 0;
	int AimbotLegitKey = 0;
	int AimbotExtKey = 0;
	int ClosedKey = VK_DELETE;

	int FakeLagKey;
	int CamaraJipiKey;
	int FreezeLagKey;
	int SilentLagKey;
	int TPInfinitoKey;
	int GhostLagKey;

	int FlyWallKey;
	int FlyWall1Key;
	int FlyWall2Key;
	int WallHack1Key;
	int WallHack2Key;
	int WallHack3Key;
	int WallHack4Key;
	int WallHack5Key;

	int SpeedKey;
	int MagnetKey;
	int TpBaseKey;
	int TpEnemyKey;
	int UpKey;
	int TeleKKey;
	int PullKey = 0;
	int SilentAimKey = 0;
	int EnemyPullKey = 0;
	int TeleportKillKey;
	int AimbotToggleKey;
	int SniperMacroKey;

	int SPJoystickKey;
}

inline namespace YorzenCheck
{
	bool AimbotRage;
	bool AimbotRage2;
	bool AimbotPlayer;
	bool AimbotRotation;
	bool AimbotPlayerX;
	bool AimbotPlayerX2;
	bool AimSniper;
	bool QuickSniper;
	bool DelaySniper;
	bool ScopeSniper;
	bool BugCamara;
	bool DamageFake;
	bool AutoFreezelag;
	bool FastReload;
	bool NoRecoil;
	bool SilentHook;
	bool RapidFall;
	bool AgressiveHook;
	bool InfiniteBullents;
	bool MedikitFast;
	bool InstantFire;
	bool SpeedFire;
	bool VisionHack;
	bool NightMode;

	bool ByPassEmulator;
	bool SilentCheat;
	bool AntiBlacklist;
	bool AntiBan;
	bool DetectByPass;

	bool AimSilentHookX;

	bool g3siedkcheroesp = true;
}

HWND hdPlayerWindow;
HWND hwnd;
RECT rc;

void ToggleClickability(bool clickable)
{
	if (!hwnd || !IsWindow(hwnd))
		return;

	if (clickable) {
		SetWindowLongPtr(hwnd, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED);
		SetForegroundWindow(hwnd);
	}
	else {
		SetWindowLongPtr(hwnd, GWL_EXSTYLE,
			WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE);
	}

	SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);
	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
}

inline namespace YorzenCombo
{
	static int bypass_combo;
	const char* bypass_list[] = { "Normal", "Max", "x86" };

	static int aimbot_combo;
	const char* aimbot_list[] = { "Rage", "Legit", "Hex" ,  "Visible" };

	static int rotation_combo;
	const char* rotation_list[] = { "Head", "Neck", "Chest" };

	static int Esppostions;
	const char* Esp_Postions[] = { "Up", "Middle", "Bottom"};

	static int silent_combo;
	const char* silent_list[] = { "Normal", "360" };

	static int bypass2_combo;
}

inline namespace YorzenSliders
{
	static float valueFire = 0.7f;
	static float valuexde = 10.0f;

	float NightIntensity = 0.7f;

	int FovAll = 180;
	int DistanceAll = 150;

	int FovRage = 180;
	int DistanceRage = 150;
	int delayToHeadMs = 200;
	int DelayHead = 300;

	int FovRotation = 360;
	int DistanceRotation = 150;

	int VisionSlider = 30;
	int SecondsLag = 1;
	int InfinitySliders = 555;

	int EnemyDistance = 0.0f;
	int EnemyDistanceX = 8;
	int EnemySide = 0.0f;

	int UpX = 5;

	int FovHook = 360;
	int DistanceHook = 150;
}

inline namespace YorzenEsp
{
	int BoxThickness = 1.5f;
	int LineW = 6.f;
	int LineThickness = 1.0f;
	int BonesThickness = 1.5f;

	inline namespace FovX
	{
		bool ShowFov;
		bool ShowCrosshair;
		inline ImVec4 ColorFov = ImColor(255, 255, 255);
		inline ImVec4 ColorCrosshair = ImColor(255, 255, 255);

		bool SniperAllGunSwitch = false;
		bool SniperSwitch = false;
		bool SniperDelayFix = false;
		bool SniperAim = false;
		bool SniperScopeTracking = false;
		bool SniperScanSuccess = false;

		bool AwmLocation = false;
		bool AwmYLocation = false;
		bool M82bLocation = false;
		bool VskLocation = false;
		bool SniperScopeVar = false;
	}

	inline namespace Line
	{
		bool EspLine = false;
		static int linePos = 0;
		inline ImVec4 LineColor = ImColor(255, 255, 255);
	}
	inline namespace Box
	{
		bool EspBox = false;
		inline ImVec4 Colorbox = ImColor(255, 255, 255);
		static int boxStyle = 0;
	}
	inline namespace Health
	{
		bool ESPHealth = false;
		static int healthSide = 2;
		static float healthValue = 75.f;
		static bool draggingHealth = false;
		static int ghostSide = healthSide;
		auto Lerp = [](float a, float b, float t) {
			return a + (b - a) * t;
		};
	}
	inline namespace Bones
	{
		bool ESPBones = false;
		bool rgbMode = false;
		inline ImVec4 bonesColor = ImColor(255, 255, 255);
	}
	inline namespace Name
	{
		bool ESPnames = false;
		static int nameSide = 0;
		static int ghostSideName = 0;
		static bool draggingName = false;
		static std::string previewName = "Enemy_Bot";
		inline ImVec4 nameColor = ImColor(255, 255, 255);
	}
	inline namespace Distance
	{
		bool ESPDisctancia = false;
		static int distanceSide = 0;
		static int ghostSideDistance = 0;
		static bool draggingDistance = false;
		static float previewDistance = 123.0f;
		inline ImVec4 DistanciaC = ImColor(255, 255, 255);
	}
	inline namespace WeaponName
	{
		bool ESPWeaponName = false;
		int weaponNameSide = 1;
		int ghostSideWeaponName = 1;
		bool draggingWeaponName = false;
		inline ImVec4 weaponColorN = ImColor(255, 255, 255);
	}
	inline namespace WeaponIcon
	{
		bool ESPWeaponIcon = false;
		int weaponIconSide = 1;
		int ghostSideWeaponIcon = 1;
		bool draggingWeaponIcon = false;
		inline ImVec4 weaponColorI = ImColor(255, 255, 255);
	}

	inline namespace Cheat
	{
		static float speedValue = 17.0f;

		bool SpeedCheck = false;
		bool MagnetCheck;
		bool TpBaseCheck;
		bool TpEnemyCheck;
		bool SPJoystickCheck;
		bool PullCheck;
		bool UpCheck;
		bool TeleKCheck;

		bool FlyWall;
		bool UnderPlayer;
		bool Rapidfall;
	}
}

struct RoleInfo {
	std::string labelText;
	ImU32 color;
	bool useEffects;
};

static const std::unordered_map<std::string, RoleInfo> roleDefinitions = {
	{ "owner", { "Owner", ImColor(111, 111, 111), true } },
	{ "client", { "Internal", ImColor(111, 111, 111), true } }
};

static const std::unordered_map<std::string, std::string> userRoles = {
	{ "Yorzen", "owner" }
};
