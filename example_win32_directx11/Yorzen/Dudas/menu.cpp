#include "menu.hpp"
#include "menu_widgets.hpp"

#include <Yorzen/Dudas/ui_settings.hpp>
#include <UIStubs.hpp>
#include <src/Globals.hpp>
#include <src/Overlay/Overlay.hpp>
#include <BlazeMem.h>
#include <ImGui/font_defines.h>
#include <ImGui/custom_widgets.hpp>
#include <imgui_settings.h>
#include <Windows.h>

#include <functional>
#include <string>

#include <AIMBOTMEMORY.H>

extern AimbotMemory Aim;

// Extern-only YorzenKey access (do NOT include yorzen.h Ã¢â‚¬â€ it defines globals).
inline namespace YorzenKey {
	extern bool HideMenuCheck;
	extern bool ClosedCheck;
	extern bool SniperMacroCheck;
	extern bool AimbotToggleCheck;
	extern bool AimbotLegitCheck;
	extern bool AimbotExtCheck;
	extern bool SilentAimCheck;
	extern bool EnemyPullCheck;
	extern bool TeleportKillCheck;
	extern bool WallHack1Check;
	extern bool WallHack2Check;
	extern bool CamaraJipiCheck;
	extern bool RefreshEspCheck;
	extern bool FakeLagCheck;
	extern bool StreamerModeCheck;
	extern bool WukongModeCheck;
	extern bool SniperScopeCheck;

	extern int HideMenuKey;
	extern int ClosedKey;
	extern int SniperMacroKey;
	extern int AimbotToggleKey;
	extern int AimbotLegitKey;
	extern int AimbotExtKey;
	extern int SilentAimKey;
	extern int EnemyPullKey;
	extern int TeleportKillKey;
	extern int WallHack1Key;
	extern int WallHack2Key;
	extern int CamaraJipiKey;
	extern int RefreshEspKey;
	extern int FakeLagKey;
	extern int StreamerModeKey;
	extern int WukongModeKey;
	extern int SniperScopeKey;
}

namespace {

void NotifyToggle(const char* name, bool enabled)
{
	std::string msg = std::string(name) + (enabled ? " Enabled" : " Disabled");
	Backend_Notify(msg.c_str(), enabled);
}

bool FeatureCheckbox(const char* label, const char* tip, bool* v, std::function<void()> cb = nullptr)
{
	bool changed = edited::Checkbox(label, tip, v, std::move(cb));
	if (changed)
		NotifyToggle(label, *v);
	return changed;
}

} // namespace

void YorzenRenderMenuTabs(float fTabOffset)
{
	// Tab 0 — Aim
	if (iTabs == 0)
	{
		ImGui::SetCursorPos(ImVec2(15, 85 + fTabOffset));
		custom::Child("Aim", ICON_AIMBOT_INTERNAL, "Aimbot & Trigger Settings", ImVec2(510, 395), true, 0);
		{
			FeatureCheckbox("Aimbot", "Enable Aim Assist", &ui.esp.AimbotEnabled);
			if (ui.esp.AimbotEnabled)
				edited::Combo("Aimbot Type", NULL, &ui.esp.AimbotType, "Visible\0Rage\0Legit\0");

			FeatureCheckbox("AimBot External", "Aimbot External Aim", &ui.esp.AimExternalEnabled);
			if (ui.esp.AimExternalEnabled) {
				if (ui.esp.AimExternalBone < 0 || ui.esp.AimExternalBone > 4)
					ui.esp.AimExternalBone = 0;
				edited::Combo("External Bones", "Select Your Aim Pos", &ui.esp.AimExternalBone,
					"Head\0Neck\0Chest\0Left Neck\0Right Neck\0");
				edited::SliderFloat("External Fov", "External Aim Search Radius", &ui.esp.AimExternalFov, 1.f, 1200.f, "%.1f");
				edited::SliderInt("External Distance", "External Max Range", &ui.esp.AimExternalDistance, 1, 200, "%d m");
			}

			FeatureCheckbox("Silent Aim", "Redirect Bullets While Firing", &ui.esp.AimSilentEnabled);
			if (ui.esp.AimSilentEnabled) {
				if (ui.esp.AimSilentHitbox < 0 || ui.esp.AimSilentHitbox > 1)
					ui.esp.AimSilentHitbox = 0;
				edited::Combo("Silent Aim Mode", "Max Head Or Body", &ui.esp.AimSilentHitbox, "Max (Head)\0Body\0");
			}

			FeatureCheckbox("Enemy Pull", "Pull Enemy Onto Aim Line", &ui.esp.PullEnemyEnabled);
			if (ui.esp.PullEnemyEnabled)
				edited::SliderFloat("Pull Distance", "Max Pull Range", &ui.esp.PullEnemyDistance, 1.f, 500.f, "%.0fm");

			FeatureCheckbox("Auto Fire", "Auto Click When In Fov", &ui.esp.AutoFireEnabled);

			FeatureCheckbox("Show Aim Fov", "Shows Aim Search Circle", &ui.esp.AimFovEnabled);
			if (ui.esp.AimFovEnabled)
				edited::ColorEdit4("Fov Color", NULL, ui.esp.AimFovColor);

			FeatureCheckbox("No Recoil", "Less Weapon Kick", &ui.esp.NoRecoil);
			FeatureCheckbox("Fast Reload", "Faster Reload", &ui.esp.FastReload);
			FeatureCheckbox("Ignore Bots", "Skip Bot Targets", &ui.esp.IgnoreBots);
			FeatureCheckbox("Ignore Knocked", "Skip Downed Enemies", &ui.esp.IgnoreKnockedEntity);

			edited::SliderFloat("Fov Radius", "Aim Search Radius", &ui.esp.AimFovValue, 0.f, 1200.f, "%.1f");
			edited::SliderInt("Aim Distance", "Max Aim Range (m)", &ui.esp.AimbotDistance, 0, 200, "%d m");

			if (custom::Checkbox("Glitch Fire", &Backend_BlazeCheckbox(5931))) {
				BlazeMemOnCheckboxToggled(5931, "Glitch Fire",
					[]() { Aim.GlitchFireON(); }, []() { Aim.GlitchFireOFF(); });
			}

			FeatureCheckbox("Sniper Scope", "Sniper Aim On Fire", &ui.esp.SniperScopeEnabled);
			if (ui.esp.SniperScopeEnabled)
				edited::Combo("Scope Mode", NULL, &ui.esp.SniperScopeMode, "Head\0Body\0");

			if (custom::Checkbox("Sniper Switch", &Backend_BlazeCheckbox(8))) {
				BlazeMemOnCheckboxToggled(8, "Sniper Switch",
					[]() { Aim.SniperSwitchon(); }, []() { Aim.SniperSwitchoff(); });
			}
		}
		custom::EndChild();
	}

	// Tab 1 — Visual
	if (iTabs == 1)
	{
		ImGui::SetCursorPos(ImVec2(15, 85 + fTabOffset));
		custom::Child("Esp", ICON_BRUSH_LINE, "Player Esp & Options", ImVec2(510, 395), true, 0);
		{
			FeatureCheckbox("Enable Esp", "Master Esp Toggle", &ui.esp.ESPMasterEnabled);

			FeatureCheckbox("Esp Line", "Tracer To Enemies", &ui.esp.ESPLineEnabled, []() {
				edited::ColorEdit4("Line Color", NULL, ui.esp.ESPLineColor);
				edited::Combo("Line Pos", NULL, &ui.esp.ESPLineStartPos, "Top\0Center\0Bottom\0");
				FeatureCheckbox("Top Line Logo", "Logo On Line Start", &ui.esp.ShowLineLogo);
			});

			FeatureCheckbox("Esp Box", "Box Around Enemies", &ui.esp.ESPBoxEnabled, []() {
				edited::ColorEdit4("Box Color", NULL, ui.esp.ESPBoxColor);
				edited::Combo("Box Type", NULL, &ui.esp.ESPBoxMode, "Dynamic\0Cornered\0");
			});

			FeatureCheckbox("Box Fill", "Filled Box Overlay", &ui.esp.ESPBoxFill, []() {
				edited::ColorEdit4("Fill Color", NULL, ui.esp.ESPBoxFillColor);
			});

			FeatureCheckbox("Skeleton", "Bone Overlay", &ui.esp.ESPBoneEnabled, []() {
				edited::ColorEdit4("Bone Color", NULL, ui.esp.ESPBoneColor);
			});

			FeatureCheckbox("Health Bar", "Hp Bar", &ui.esp.PlayerHealthBar);
			FeatureCheckbox("Weapon Icon", "Weapon Glyph", &ui.esp.PlayerWeaponIcon);

			FeatureCheckbox("Weapon Name", "Weapon Text", &ui.esp.PlayerWeaponNameEnabled, []() {
				edited::ColorEdit4("Weapon Color", NULL, ui.esp.PlayerWeaponNameColor);
			});

			FeatureCheckbox("Enemy Name", "Player Name", &ui.esp.PlayerNameEnabled, []() {
				edited::ColorEdit4("Name Color", NULL, ui.esp.PlayerNameColor);
			});

			FeatureCheckbox("Distance", "Distance Text", &ui.esp.PlayerDistanceEnabled, []() {
				edited::ColorEdit4("Distance Color", NULL, ui.esp.PlayerDistanceColor);
			});

			FeatureCheckbox("Esp Rank", "Rank Tag", &ui.esp.RankEnabled, []() {
				edited::ColorEdit4("Rank Color", NULL, ui.esp.RankColor);
			});

			FeatureCheckbox("Origin Line", "Line Under 20m", &ui.esp.SnapLinesEnabled);
			FeatureCheckbox("Wukong Mode", "Visible Only", &ui.esp.WukongMode);
			FeatureCheckbox("Rainbow Esp", "Cycle Colors", &ui.esp.RainbowESP);
			FeatureCheckbox("Ignore Training Bots", "Hide Training Bots", &ui.esp.IgnoreTrainingBots);
			FeatureCheckbox("Invalid Timer", "Match Timer", &ui.esp.InvalidTimer);
			edited::SliderInt("Esp Distance", "Draw Distance (m)", &ui.esp.espmaxdis, 0, 200, "%d m");

			if (custom::Button("Refresh Esp", ImVec2(ImGui::GetContentRegionAvail().x, 35))) {
				g_Globals.EspConfig.Refresh = true;
				Backend_Notify("ESP Refreshed", true);
			}
		}
		custom::EndChild();
	}

	// Tab 2 — Brutal + Machine
	if (iTabs == 2)
	{
		ImGui::SetCursorPos(ImVec2(15, 85 + fTabOffset));
		custom::Child("Brutal", ICON_COMPONENTS_LINE, "Movement, Wall Hacks & Exploits", ImVec2(510, 395), true, 0);
		{
			FeatureCheckbox("Speed Timer", "Faster Sprint", &ui.esp.SpeedTimerEnabled);

			if (custom::Button("Load Wall Hack", ImVec2(ImGui::GetContentRegionAvail().x, 36))) {
				BlazeMemRunLoad("Wall Hack", []() { Aim.SaveWallhackAoB(); });
			}
			if (custom::Checkbox("Wall Hack", &Backend_BlazeCheckbox(31))) {
				BlazeMemOnCheckboxToggled(31, "Wall Hack",
					[]() { Aim.ActivateWallhack(); }, []() { Aim.OFFWallhack(); });
			}

			if (custom::Button("Load Camera Right", ImVec2(ImGui::GetContentRegionAvail().x, 36))) {
				BlazeMemRunLoad("Camera Right", []() { Aim.SaveCameraAoB(); });
			}
			if (custom::Checkbox("Camera Right", &Backend_BlazeCheckbox(301))) {
				BlazeMemOnCheckboxToggled(301, "Camera Right",
					[]() { Aim.ActivateCamera(); }, []() { Aim.OFFCamera(); });
			}

			if (custom::Button("Load Fast Landing", ImVec2(ImGui::GetContentRegionAvail().x, 36))) {
				BlazeMemRunLoad("Fast Landing Hack", []() { Aim.SaveFastlandingAoB(); });
			}
			if (custom::Checkbox("Fast Landing", &Backend_BlazeCheckbox(111))) {
				BlazeMemOnCheckboxToggled(111, "Fast Landing",
					[]() { Aim.ActivateFastlanding(); }, []() { Aim.OFFFastlanding(); });
			}

			FeatureCheckbox("Burst Fire", "Rapid Burst", &ui.esp.BurstFire);
			FeatureCheckbox("Vision Hack", "See Farther", &ui.esp.VisionHackEnabled);
			FeatureCheckbox("Down Player", "Push Model Down", &ui.esp.DownPlayerEnabled);

			if (FeatureCheckbox("No Gravity Fly", "WASD Space Up Ctrl Down", &ui.esp.NoGravityFlyEnabled)) {
				g_Globals.Misc.NoGravityFlyEnabled = ui.esp.NoGravityFlyEnabled;
				if (ui.esp.NoGravityFlyEnabled) {
					g_Globals.Misc.FlyHackInternalEnabled = false;
					FlyHack_LocalPlayer::Stop();
					NoGravityFly::Start();
				}
				else {
					NoGravityFly::Stop();
				}
			}

			FeatureCheckbox("Spin Player", "Spin Character", &ui.esp.SpinPlayerEnabled);
			if (ui.esp.SpinPlayerEnabled)
				edited::SliderFloat("Spin Speed", NULL, &ui.esp.SpinPlayerSpeed, 1.f, 15.f, "%.0fx");

			if (custom::Checkbox("Guest Reset", &Backend_BlazeCheckbox(9154))) {
				BlazeMemOnCheckboxToggled(9154, "Guest Reset",
					[]() { Aim.GuestResetON(); }, []() { Aim.GuestResetOFF(); });
			}
		}
		custom::EndChild();
	}

	// Tab 3 — Settings
	if (iTabs == 3)
	{
		ImGui::SetCursorPos(ImVec2(15, 85 + fTabOffset));
		custom::Child("Settings", ICON_SETTINGS_2_LINE, "General Settings", ImVec2(510, 395), true, 0);
		{
			std::string button_name = std::string(bTheme ? ICON_MOON_FILL : ICON_SUN_FILL) + " Change Color Theme" + std::string(bTheme ? ICON_MOON_FILL : ICON_SUN_FILL);
			if (custom::Button(button_name.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 35))) {
				bTheme = !bTheme;
				Backend_Notify(bTheme ? "Dark Theme" : "Light Theme", true);
			}

			if (custom::ColorEdit4("Primary Color", "Menu Accent Color", (float*)&c::main_color, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoInputs))
			{
				c::accent = c::main_color;
				c::text::text_hov = c::main_color;
			}

			static bool tempCleanerOnce = false;
			if (FeatureCheckbox("Temp File Cleaner", "Delete Temp/Cache Files.", &tempCleanerOnce)) {
				if (tempCleanerOnce) {
					Backend_RunTempCleaner();
					tempCleanerOnce = false;
				}
			}

			if (FeatureCheckbox("Streamer Mode", "Hide From Capture", &ui.esp.EspStreamerMode)) {
				HWND overlayHwnd = FWork::Overlay::GetOverlayWindow();
				if (ui.esp.EspStreamerMode)
					SetWindowDisplayAffinity(overlayHwnd, WDA_EXCLUDEFROMCAPTURE);
				else
					SetWindowDisplayAffinity(overlayHwnd, WDA_NONE);
			}

			FeatureCheckbox("Disable All Effects", "For Low-end Pcs. Disables Animations, Glow, And Heavy Shaders For Smoother Fps.", &ui.esp.DisableAllEffects);

			static bool saveConfigOnce = false;
			if (FeatureCheckbox("Save Configuration", "Saves current settings for the next launch.", &saveConfigOnce)) {
				if (saveConfigOnce) {
					Backend_SaveConfig();
					saveConfigOnce = false;
				}
			}

			if (custom::Button("Reset Config", ImVec2(ImGui::GetContentRegionAvail().x, 40))) {
				Backend_ResetConfig();
			}

			if (custom::Checkbox("Mute", &custom::GlobalMute))
				NotifyToggle("Mute", custom::GlobalMute);

			if (custom::Button("Exit Panel", ImVec2(ImGui::GetContentRegionAvail().x, 40))) {
				Backend_ExitPanel();
			}
		}
		custom::EndChild();
	}
}
