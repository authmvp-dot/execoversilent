#pragma once
#include "imgui.h"

struct ESPData {
	bool ESPMasterEnabled = false;
	bool PlayerWeaponIcon = false;
	bool PlayerHealthBar = false;
	int espmaxdis = 160;
	bool IgnoreKnockedEntity = false;
	bool IgnoreBots = false;
	bool NoRecoil = false;
	bool FastReload = false;
	bool BurstFire = false;
	bool EspStreamerMode = false;
	bool DisableAllEffects = true;
	int AimbotDistance = 150;

	bool AimFovEnabled = false;
	float AimFovValue = 120.f;
	float AimFovColor[4] = { 1.f, 1.f, 1.f, 1.f };

	bool AimSilentEnabled = false;
	int AimSilentType = 0; // unused
	int AimSilentHitbox = 0; // 0 = Max (Head), 1 = Body

	int AimbotDelay = 290;
	int AimbotType = 0;
	bool AimbotEnabled = false;

	bool PullEnemyEnabled = false;
	int PullEnemyType = 0;
	float PullEnemyDistance = 250.0f;
	bool AutoFireEnabled = false;

	bool SpeedTimerEnabled = false;
	bool SniperScopeEnabled = false;
	int SniperScopeMode = 1;
	bool VisionHackEnabled = false;
	bool DownPlayerEnabled = false;
	bool NoGravityFlyEnabled = false;
	bool SpinPlayerEnabled = false;
	float SpinPlayerSpeed = 5.f;

	bool ESPLineEnabled = false;
	float ESPLineGlowRadius = 12.f;
	bool ESPLineIsGlow = false;
	int ESPLineStartPos = 0;
	float ESPLineColor[4] = { 1.f, 1.f, 1.f, 1.f };
	bool ShowLineLogo = true;

	bool ESPBoxEnabled = false;
	int ESPBoxMode = 0;
	bool ESPBoxIsGlow = false;
	bool ESPBoxFill = false;
	float ESPBoxColor[4] = { 1.f, 1.f, 1.f, 1.f };
	float ESPBoxFillColor[4] = { 0.f, 0.f, 0.f, 0.35f };
	float ESPBoxGlowRadius = 12.f;

	bool PlayerNameEnabled = false;
	float PlayerNameColor[4] = { 1.f, 1.f, 1.f, 1.f };

	bool PlayerHealthTextEnabled = false;
	float PlayerHealthTextColor[4] = { 1.f, 1.f, 1.f, 1.f };

	bool PlayerWeaponNameEnabled = false;
	float PlayerWeaponNameColor[4] = { 1.f, 1.f, 1.f, 1.f };

	bool PlayerDistanceEnabled = false;
	float PlayerDistanceColor[4] = { 1.f, 1.f, 1.f, 1.f };

	bool ESPBoneEnabled = false;
	float ESPBoneGlowRadius = 14.f;
	bool ESPBoneIsGlow = false;
	float ESPBoneColor[4] = { 1.f, 1.f, 1.f, 1.f };

	bool RankEnabled = false;
	float RankColor[4] = { 1.f, 0.84f, 0.f, 1.f };
	bool SnapLinesEnabled = false;
	float SnapLinesColor[4] = { 1.f, 1.f, 1.f, 1.f };
	bool WukongMode = false;
	bool RainbowESP = false;
	bool IgnoreTrainingBots = false;
	bool InvalidTimer = false;

	bool LootEnabled = false;
	int LootRenderDistance = 150;
	bool LootShowPicker = true;

	bool AimExternalEnabled = false;
	int AimExternalBone = 1;
	float AimExternalFov = 120.f;
	int AimExternalDistance = 150;
	int AimExternalKey = 0;

	bool EnableKeybinds = false;
	bool ShowKeybindStatus = true;
};

struct ChamsData {
	float blendColor[4] = { 1.f, 0.f, 0.f, 1.f };
	float blendColor1[4] = { 1.f, 0.f, 0.f, 1.f };
	bool chams3d = false;
	bool black = false;
	bool moco = false;
	bool lineart = false;
	bool solid = false;
};

struct UISettings {
	ESPData esp;
	ChamsData chams;
};

inline UISettings ui;
