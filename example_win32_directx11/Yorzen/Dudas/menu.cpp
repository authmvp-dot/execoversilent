#include <winsock2.h>
#include <ws2tcpip.h>
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

#include <map>
namespace custom {
    inline std::map<std::string, int> FeatureKeys;
    inline std::map<std::string, int> FeatureKeyModes;
    inline std::map<std::string, bool> FeatureKeyStates;
}

namespace {

void NotifyToggle(const char* name, const char* tip, bool enabled)
{
	std::string title = std::string(name) + (enabled ? " Enabled" : " Disabled");
	Backend_Notify(title.c_str(), tip ? tip : "Status updated.", enabled);
}

bool FeatureCheckbox(const char* label, const char* tip, bool* v, std::function<void()> cb = nullptr)
{
	int& key = custom::FeatureKeys[label];
	int& mode = custom::FeatureKeyModes[label];
	bool& state = custom::FeatureKeyStates[label];

	if (key != 0 && mode == 0) {
		if (GetAsyncKeyState(key) & 0x8000) {
			if (!state) {
				state = true;
				*v = !(*v);
				NotifyToggle(label, tip, *v);
			}
		} else {
			state = false;
		}
	}

	bool changed = custom::Checkbox(label, v, 70.f);
	if (changed)
		NotifyToggle(label, tip, *v);

	ImGui::SameLine(ImGui::GetContentRegionAvail().x - 65.f);
	ImGui::PushID(label);
	ImGui::PushItemWidth(65.f);
	custom::Keybind("##kb", &key, &mode);
	ImGui::PopItemWidth();
	ImGui::PopID();

	if (*v) {
		ImGui::Indent(10.0f);
		ImGui::PushID(label);
		if (cb) cb();
		ImGui::PopID();
		ImGui::Unindent(10.0f);
	}

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
			if (FeatureCheckbox("Enable Functions", "All Functions Enable", &ui.esp.ESPMasterEnabled)) {
				SendCommandToBridge(102, ui.esp.ESPMasterEnabled ? 1 : 0);
			}
			
			if (FeatureCheckbox("Bypass Report", "Anti Report", &Backend_BlazeCheckbox(23))) {
				SendCommandToBridge(23, Backend_BlazeCheckbox(23) ? 1 : 0);
			}
			auto UpdateAimKillModes = []() {
				if (!ui.esp.AimbotEnabled) {
					SendCommandToBridge(973201, 0);
					SendCommandToBridge(12213, 0);
					SendCommandToBridge(103, 0);
				} else {
					SendCommandToBridge(973201, ui.esp.AimbotType == 0 ? 1 : 0);
					SendCommandToBridge(12213, ui.esp.AimbotType == 1 ? 1 : 0);
					SendCommandToBridge(103, ui.esp.AimbotType == 2 ? 1 : 0);
				}
			};
			if (FeatureCheckbox("AimKill", "Enable Aim Assist", &ui.esp.AimbotEnabled)) {
				UpdateAimKillModes();
			}
			if (ui.esp.AimbotEnabled) {
				if (edited::Combo("AimKill Type", NULL, &ui.esp.AimbotType, "Safe\0Take\0Send\0")) {
					UpdateAimKillModes();
				}
			}

			if (FeatureCheckbox("Silent Kill", "Redirect Bullets While Firing", &ui.esp.AimSilentEnabled)) {
				SendCommandToBridge(510010, ui.esp.AimSilentEnabled ? 1 : 0);
			}

			if (FeatureCheckbox("Auto Fuck Fire", "Auto Click When In Fov", &ui.esp.AutoFireEnabled)) {
				SendCommandToBridge(175, ui.esp.AutoFireEnabled ? 1 : 0);
			}

			

			if (FeatureCheckbox("Fast Reload", "Faster Reload", &ui.esp.FastReload)) {
				SendCommandToBridge(700971, ui.esp.FastReload ? 1 : 0);
			}

			
			if (FeatureCheckbox("Auto Revive", "Teammate Auto Revive", &Backend_BlazeCheckbox(4510))) {
				SendCommandToBridge(4510, Backend_BlazeCheckbox(4510) ? 1 : 0);
			}
			if (FeatureCheckbox("Auto Fly 100x", "Auto Fly In Sky With Glider", &Backend_BlazeCheckbox(2323256))) {
				SendCommandToBridge(2323256, Backend_BlazeCheckbox(2323256) ? 1 : 0);
			}
			if (FeatureCheckbox("Fly x999", "Fly 80X Distance In Sky", &Backend_BlazeCheckbox(201012))) {
				SendCommandToBridge(201012, Backend_BlazeCheckbox(201012) ? 1 : 0);
			}
			if (FeatureCheckbox("Glider Mode", "Fly 50X Distance In Sky", &Backend_BlazeCheckbox(54120))) {
				SendCommandToBridge(54120, Backend_BlazeCheckbox(54120) ? 1 : 0);
			}
			if (FeatureCheckbox("Teleport To Player", "Enemy Auto Teleport", &Backend_BlazeCheckbox(7777))) {
				SendCommandToBridge(7777, Backend_BlazeCheckbox(7777) ? 1 : 0);
			}
			if (FeatureCheckbox("Teleport Anywhere", "Fly Sqeuence Map Tp Without Fly", &Backend_BlazeCheckbox(7778))) {
				SendCommandToBridge(7778, Backend_BlazeCheckbox(7778) ? 1 : 0);
			}
			if (FeatureCheckbox("Auto TP Anywhere", "Barrier Open Auto Teleport Anywhere", &Backend_BlazeCheckbox(7779))) {
				SendCommandToBridge(7779, Backend_BlazeCheckbox(7779) ? 1 : 0);
			}
			if (FeatureCheckbox("Auto TP Fly 100x", "Auto Fly In Sky With Glider", &Backend_BlazeCheckbox(6756383))) {
				SendCommandToBridge(6756383, Backend_BlazeCheckbox(6756383) ? 1 : 0);
			}
			if (FeatureCheckbox("Glider Hold", "Glide Hold In Sky", &Backend_BlazeCheckbox(9001))) {
				SendCommandToBridge(9001, Backend_BlazeCheckbox(9001) ? 1 : 0);
			}
			static int gliderSpeedVal = 1;
			if (edited::SliderInt("Glider Speed", "Speed", &gliderSpeedVal, 1, 20, "%d")) {
				SendCommandToBridge(9002, gliderSpeedVal);
			}
			if (FeatureCheckbox("Glider Hold V2", "Glide Hold In Sky", &Backend_BlazeCheckbox(700120))) {
				SendCommandToBridge(700120, Backend_BlazeCheckbox(700120) ? 1 : 0);
			}
			static int gliderSpeedV2Val = 10;
			if (edited::SliderInt("Glider Speed V2", "Speed", &gliderSpeedV2Val, 10, 40, "%d")) {
				SendCommandToBridge(700122, gliderSpeedV2Val);
			}
			if (FeatureCheckbox("Invisible Mode", "Invisible Body In Invisible Mode", &Backend_BlazeCheckbox(2005))) {
				SendCommandToBridge(2005, Backend_BlazeCheckbox(2005) ? 1 : 0);
			}
			if (FeatureCheckbox("Unvisible Players", "Unvisible Enemy in Invisible Mode", &Backend_BlazeCheckbox(2006))) {
				SendCommandToBridge(2006, Backend_BlazeCheckbox(2006) ? 1 : 0);
			}

			if (FeatureCheckbox("Auto Rotate", "Auto Target Lock", &Backend_BlazeCheckbox(1055))) {
				SendCommandToBridge(1055, Backend_BlazeCheckbox(1055) ? 1 : 0);
			}
			if (FeatureCheckbox("Down Kill", "Underground Kill", &Backend_BlazeCheckbox(504))) {
				SendCommandToBridge(504, Backend_BlazeCheckbox(504) ? 1 : 0);
			}
			if (FeatureCheckbox("Auto Down Kill", "Barrier Open Auto Down Kill 3.5s", &Backend_BlazeCheckbox(5040))) {
				SendCommandToBridge(5040, Backend_BlazeCheckbox(5040) ? 1 : 0);
			}
			if (FeatureCheckbox("Speed Hack", "Speed Hack Joystick", &Backend_BlazeCheckbox(507))) {
				SendCommandToBridge(507, Backend_BlazeCheckbox(507) ? 1 : 0);
			}
		}
		custom::EndChild();
	}

	// Tab 1 — Visual
	if (iTabs == 1)
	{
		ImGui::SetCursorPos(ImVec2(15, 80 + fTabOffset));
		custom::Child("Esp", ICON_BRUSH_LINE, "Player Esp & Options", ImVec2(510, 350), true, 0);
		{
			static int drawColorIdx = 0;
			if (FeatureCheckbox("Esp Line", "Tracer To Enemies", &ui.esp.ESPLineEnabled, []() {
				if (edited::Combo("Draw Color", "Color Palette", &drawColorIdx, "White\0Green\0Blue\0Red\0Black\0Yellow\0Cyan\0Magenta\0Gray\0Purple\0Orange\0Pink\0SpringGreen\0SteelBlue\0Purple2\0DeepPink\0LimeGreen\0SkyBlue\0DarkOrange\0Magenta2\0Cyan2\0")) {
					SendCommandToBridge(5, drawColorIdx);
				}
				if (edited::Combo("Line Variety", "Line Position", &ui.esp.ESPLineStartPos, "Top\0Center\0Bottom\0")) {
					SendCommandToBridge(6, ui.esp.ESPLineStartPos);
				}
			})) {
				SendCommandToBridge(1, ui.esp.ESPLineEnabled ? 1 : 0);
			}

			if (FeatureCheckbox("Esp Box", "Box Around Enemies", &ui.esp.ESPBoxEnabled, []() {
				if (edited::Combo("Box Variety", "Box Style", &ui.esp.ESPBoxMode, "Dynamic\0 3D Box\0Cornered\0")) {
					SendCommandToBridge(7, ui.esp.ESPBoxMode);
				}
			})) {
				SendCommandToBridge(2, ui.esp.ESPBoxEnabled ? 1 : 0);
			}

	
			if (FeatureCheckbox("Health Bar", "Hp Bar", &ui.esp.PlayerHealthBar)) {
				SendCommandToBridge(3, ui.esp.PlayerHealthBar ? 1 : 0);
			}

			if (FeatureCheckbox("Weapon Icon", "Weapon Glyph", &ui.esp.PlayerWeaponIcon)) {
				ui.esp.PlayerWeaponNameEnabled = ui.esp.PlayerWeaponIcon;
				SendCommandToBridge(100, ui.esp.PlayerWeaponIcon ? 1 : 0);
			}

			if (FeatureCheckbox("Weapon Name", "Weapon Text", &ui.esp.PlayerWeaponNameEnabled, []() {
				edited::ColorEdit4("Weapon Color", NULL, ui.esp.PlayerWeaponNameColor);
			})) {
				ui.esp.PlayerWeaponIcon = ui.esp.PlayerWeaponNameEnabled;
			}

			if (FeatureCheckbox("Enemy Name", "Player Name", &ui.esp.PlayerNameEnabled, []() {
				edited::ColorEdit4("Name Color", NULL, ui.esp.PlayerNameColor);
				static int nameColorIdx = 0;
				if (edited::Combo("Name Color Palette", "Name Color", &nameColorIdx, "White\0Green\0Blue\0Red\0Black\0Yellow\0Cyan\0Magenta\0Gray\0Purple\0Orange\0Pink\0SpringGreen\0SteelBlue\0Purple2\0DeepPink\0LimeGreen\0SkyBlue\0DarkOrange\0Magenta2\0Cyan2\0")) {
					SendCommandToBridge(8, nameColorIdx);
				}
			})) {
				SendCommandToBridge(4, ui.esp.PlayerNameEnabled ? 1 : 0);
			}

			if (FeatureCheckbox("Distance", "Distance Text", &ui.esp.PlayerDistanceEnabled, []() {
				edited::ColorEdit4("Distance Color", NULL, ui.esp.PlayerDistanceColor);
			})) {
				SendCommandToBridge(6, ui.esp.PlayerDistanceEnabled ? 1 : 0);
			}
			if (FeatureCheckbox("Show Aim Fov", "Shows Aim Search Circle", &ui.esp.AimFovEnabled, []() {
				edited::ColorEdit4("Fov Color", NULL, ui.esp.AimFovColor);
				static int fovColorIdx = 5;
				if (edited::Combo("Fov Color Palette", "Fov Color", &fovColorIdx, "White\0Green\0Blue\0Red\0Black\0Yellow\0Cyan\0Magenta\0Gray\0Purple\0Orange\0Pink\0SpringGreen\0SteelBlue\0Purple2\0DeepPink\0LimeGreen\0SkyBlue\0DarkOrange\0Magenta2\0Cyan2\0")) {
					SendCommandToBridge(160, fovColorIdx);
				}
			})) {
				SendCommandToBridge(16, ui.esp.AimFovEnabled ? 1 : 0);
			}

			if (edited::SliderFloat("Fov Radius", "Aim Search Radius", &ui.esp.AimFovValue, 0.f, 1200.f, "%.1f")) {
				SendCommandToBridge(104, (int)ui.esp.AimFovValue);
			}
		}
		custom::EndChild();
	}

	// Tab 2 — Brutal
	if (iTabs == 2)
	{
		ImGui::SetCursorPos(ImVec2(15, 80 + fTabOffset));
		custom::Child("Brutal", ICON_COMPONENTS_LINE, "Movement & Exploits", ImVec2(510, 350), true, 0);
		{
			if (FeatureCheckbox("Speed Timer", "Run Timer In Fight Phase Only", &Backend_BlazeCheckbox(8881))) {
				SendCommandToBridge(8881, Backend_BlazeCheckbox(8881) ? 1 : 0);
			}
			if (FeatureCheckbox("Joystick Speed", "Speed Hack Joystick", &Backend_BlazeCheckbox(15))) {
				SendCommandToBridge(15, Backend_BlazeCheckbox(15) ? 1 : 0);
			}
			if (FeatureCheckbox("Fly Mini", "Fly Jump", &Backend_BlazeCheckbox(5195))) {
				SendCommandToBridge(5195, Backend_BlazeCheckbox(5195) ? 1 : 0);
			}
			if (FeatureCheckbox("Unlock Level 8", "Unlock Level 8 For Training", &Backend_BlazeCheckbox(2522))) {
				SendCommandToBridge(2522, Backend_BlazeCheckbox(2522) ? 1 : 0);
			}
			if (FeatureCheckbox("ESP Grenade", "Shows Grenade Line To Closest Enemy", &Backend_BlazeCheckbox(1000))) {
				SendCommandToBridge(1000, Backend_BlazeCheckbox(1000) ? 1 : 0);
			}

			ImGui::Text("LOOK CHANGERS");

			if (FeatureCheckbox("Dreamspace", "Dreamspace Look Changer", &Backend_BlazeCheckbox(7373))) {
				SendCommandToBridge(7373, Backend_BlazeCheckbox(7373) ? 1 : 0);
			}
			if (FeatureCheckbox("Rampage", "Rampage Look Changer", &Backend_BlazeCheckbox(7378))) {
				SendCommandToBridge(7378, Backend_BlazeCheckbox(7378) ? 1 : 0);
			}
			if (FeatureCheckbox("Itachi", "Itachi Look Changer", &Backend_BlazeCheckbox(7379))) {
				SendCommandToBridge(7379, Backend_BlazeCheckbox(7379) ? 1 : 0);
			}
			if (FeatureCheckbox("Midnight Ace", "Midnight Ace Look Changer", &Backend_BlazeCheckbox(7380))) {
				SendCommandToBridge(7380, Backend_BlazeCheckbox(7380) ? 1 : 0);
			}
			if (FeatureCheckbox("Aurora", "Aurora Look Changer", &Backend_BlazeCheckbox(7381))) {
				SendCommandToBridge(7381, Backend_BlazeCheckbox(7381) ? 1 : 0);
			}
			if (FeatureCheckbox("Naruto's Ascent", "Naruto's Ascent Look Changer", &Backend_BlazeCheckbox(7382))) {
				SendCommandToBridge(7382, Backend_BlazeCheckbox(7382) ? 1 : 0);
			}
			if (FeatureCheckbox("Last Paradox", "Last Paradox Look Changer", &Backend_BlazeCheckbox(7383))) {
				SendCommandToBridge(7383, Backend_BlazeCheckbox(7383) ? 1 : 0);
			}
			if (FeatureCheckbox("Frostfire", "Frostfire Look Changer", &Backend_BlazeCheckbox(7384))) {
				SendCommandToBridge(7384, Backend_BlazeCheckbox(7384) ? 1 : 0);
			}
			if (FeatureCheckbox("Scorpio", "Scorpio Look Changer", &Backend_BlazeCheckbox(7385))) {
				SendCommandToBridge(7385, Backend_BlazeCheckbox(7385) ? 1 : 0);
			}
			if (FeatureCheckbox("Devil Trigger", "Devil Trigger Look Changer", &Backend_BlazeCheckbox(7386))) {
				SendCommandToBridge(7386, Backend_BlazeCheckbox(7386) ? 1 : 0);
			}
			if (FeatureCheckbox("Cannibal Havoc", "Cannibal Havoc Look Changer", &Backend_BlazeCheckbox(7387))) {
				SendCommandToBridge(7387, Backend_BlazeCheckbox(7387) ? 1 : 0);
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
				Backend_Notify("Theme Changed", bTheme ? "Dark Theme applied." : "Light Theme applied.", true);
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


			if (FeatureCheckbox("Disable All Effects", "For Low-end Pcs. Disables Animations, Glow, And Heavy Shaders For Smoother Fps.", &ui.esp.DisableAllEffects)) {
				g_Globals.General.DisableAllEffects = ui.esp.DisableAllEffects;
			}

			static bool saveConfigOnce = false;
			if (FeatureCheckbox("Save Configuration", "Saves current settings for the next launch.", &saveConfigOnce)) {
				Backend_SaveConfig();
				saveConfigOnce = false;
			}

			if (custom::Button("Reset Config", ImVec2(ImGui::GetContentRegionAvail().x, 40))) {
				Backend_ResetConfig();
			}

			if (custom::Checkbox("Mute", &custom::GlobalMute))
				NotifyToggle("Mute", "Global sound muted.", custom::GlobalMute);



			if (custom::Button("Exit Panel", ImVec2(ImGui::GetContentRegionAvail().x, 40))) {
				Backend_ExitPanel();
			}
		}
		custom::EndChild();
	}
}
