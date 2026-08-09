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
		ImGui::SetCursorPos(ImVec2(15, 80 + fTabOffset));
		custom::Child("Aim", ICON_AIMBOT_INTERNAL, "Aimbot & Trigger Settings", ImVec2(510, 350), true, 0);
		{
			FeatureCheckbox("Aimbot", "Enable Aim Assist", &ui.esp.AimbotEnabled);
			if (ui.esp.AimbotEnabled)
				edited::Combo("Aimbot Type", NULL, &ui.esp.AimbotType, "Visible\0Rage\0Legit\0");

			FeatureCheckbox("Silent Aim", "Redirect Bullets While Firing", &ui.esp.AimSilentEnabled);
			if (ui.esp.AimSilentEnabled) {
				if (ui.esp.AimSilentHitbox < 0 || ui.esp.AimSilentHitbox > 1)
					ui.esp.AimSilentHitbox = 0;
				edited::Combo("Silent Aim Mode", "Max Head Or Body", &ui.esp.AimSilentHitbox, "Max (Head)\0Body\0");
			}

			FeatureCheckbox("Auto Fire", "Auto Click When In Fov", &ui.esp.AutoFireEnabled);

			FeatureCheckbox("Show Aim Fov", "Shows Aim Search Circle", &ui.esp.AimFovEnabled);
			if (ui.esp.AimFovEnabled)
				edited::ColorEdit4("Fov Color", NULL, ui.esp.AimFovColor);

			FeatureCheckbox("Fast Reload", "Faster Reload", &ui.esp.FastReload);

			edited::SliderFloat("Fov Radius", "Aim Search Radius", &ui.esp.AimFovValue, 0.f, 1200.f, "%.1f");
			edited::SliderInt("Aim Distance", "Max Aim Range (m)", &ui.esp.AimbotDistance, 0, 200, "%d m");
		}
		custom::EndChild();
	}

	// Tab 1 — Visual
	if (iTabs == 1)
	{
		ImGui::SetCursorPos(ImVec2(15, 80 + fTabOffset));
		custom::Child("Esp", ICON_BRUSH_LINE, "Player Esp & Options", ImVec2(510, 350), true, 0);
		{
			if (FeatureCheckbox("Enable Esp", "Master Esp Toggle", &ui.esp.ESPMasterEnabled)) {
				SendCommandToBridge(102, ui.esp.ESPMasterEnabled ? 1 : 0);
			}

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

			edited::SliderInt("Esp Distance", "Draw Distance (m)", &ui.esp.espmaxdis, 0, 200, "%d m");
		}
		custom::EndChild();
	}

	// Tab 2 — Brutal
	if (iTabs == 2)
	{
		ImGui::SetCursorPos(ImVec2(15, 80 + fTabOffset));
		custom::Child("Brutal", ICON_COMPONENTS_LINE, "Movement & Exploits", ImVec2(510, 350), true, 0);
		{
			FeatureCheckbox("Speed Timer", "Faster Sprint", &ui.esp.SpeedTimerEnabled);
			FeatureCheckbox("Vision Hack", "See Farther", &ui.esp.VisionHackEnabled);
			FeatureCheckbox("Down Player", "Push Model Down", &ui.esp.DownPlayerEnabled);

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
		ImGui::SetCursorPos(ImVec2(15, 80 + fTabOffset));
		custom::Child("Settings", ICON_SETTINGS_2_LINE, "General Settings", ImVec2(510, 350), true, 0);
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

			FeatureCheckbox("RGB Rainbow Theme", "Cycle Menu Colors Automatically", &c::bRGBTheme);

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
