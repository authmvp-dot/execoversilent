
// Tab 0 — Aim
if (iTabs == 0)
{
	ImGui::SetCursorPos(ImVec2(180, 85 + fTabOffset));
	custom::Child("Aim", ICON_AIMBOT_INTERNAL, "Aimbot Settings", ImVec2(270, ImGui::GetContentRegionAvail().y + 20), true, 0);
	{
		edited::Checkbox("Aimbot", "Enable Aim Assist", &ui.esp.AimbotEnabled);
		if (ui.esp.AimbotEnabled)
			edited::Combo("Aimbot Type", NULL, &ui.esp.AimbotType, "Visible\0Rage\0Legit\0");

		edited::Checkbox("AimBot External", "Aimbot External Aim", &ui.esp.AimExternalEnabled);
		if (ui.esp.AimExternalEnabled) {
			if (ui.esp.AimExternalBone < 0 || ui.esp.AimExternalBone > 4)
				ui.esp.AimExternalBone = 0;
			edited::Combo("External Bones", "Select Your Aim Pos", &ui.esp.AimExternalBone,
				"Head\0Neck\0Chest\0Left Neck\0Right Neck\0");
			edited::SliderFloat("External Fov", "External Aim Search Radius", &ui.esp.AimExternalFov, 1.f, 1200.f, "%.1f");
			edited::SliderInt("External Distance", "External Max Range", &ui.esp.AimExternalDistance, 1, 200, "%d m");
		}

		edited::Checkbox("Silent Aim", "Redirect Bullets While Firing", &ui.esp.AimSilentEnabled);
		if (ui.esp.AimSilentEnabled) {
			if (ui.esp.AimSilentHitbox < 0 || ui.esp.AimSilentHitbox > 1)
				ui.esp.AimSilentHitbox = 0;
			edited::Combo("Silent Aim Mode", "Max Head Or Body", &ui.esp.AimSilentHitbox, "Max (Head)\0Body\0");
		}

		edited::Checkbox("Enemy Pull", "Pull Enemy Onto Aim Line", &ui.esp.PullEnemyEnabled);
		if (ui.esp.PullEnemyEnabled)
			edited::SliderFloat("Pull Distance", "Max Pull Range", &ui.esp.PullEnemyDistance, 1.f, 500.f, "%.0fm");

		edited::Checkbox("Auto Fire", "Auto Click When In Fov", &ui.esp.AutoFireEnabled);

		edited::Checkbox("Show Aim Fov", "Shows Aim Search Circle", &ui.esp.AimFovEnabled);
		if (ui.esp.AimFovEnabled)
			edited::ColorEdit4("Fov Color", NULL, ui.esp.AimFovColor);

		edited::Checkbox("No Recoil", "Less Weapon Kick", &ui.esp.NoRecoil);
		edited::Checkbox("Fast Reload", "Faster Reload", &ui.esp.FastReload);
		edited::Checkbox("Ignore Bots", "Skip Bot Targets", &ui.esp.IgnoreBots);
		edited::Checkbox("Ignore Knocked", "Skip Downed Enemies", &ui.esp.IgnoreKnockedEntity);
	}
	custom::EndChild();

	ImGui::SetCursorPos(ImVec2(470, 85 + fTabOffset));
	custom::Child("Trigger Module", ICON_AIMBOT_EXTERNAL, "Fov & Trigger", ImVec2(270, ImGui::GetContentRegionAvail().y + 20), true, 0);
	{
		edited::SliderFloat("Fov Radius", "Aim Search Radius", &ui.esp.AimFovValue, 0.f, 1200.f, "%.1f");
		edited::SliderInt("Aim Distance", "Max Aim Range (m)", &ui.esp.AimbotDistance, 0, 200, "%d m");

		if (custom::Checkbox("Glitch Fire", &Backend_BlazeCheckbox(5931))) {
			BlazeMemOnCheckboxToggled(5931, "Glitch Fire",
				[]() { Aim.GlitchFireON(); }, []() { Aim.GlitchFireOFF(); });
		}

		edited::Checkbox("Sniper Scope", "Sniper Aim On Fire", &ui.esp.SniperScopeEnabled);
		if (ui.esp.SniperScopeEnabled)
			edited::Combo("Scope Mode", NULL, &ui.esp.SniperScopeMode, "Head\0Body\0");

		if (custom::Checkbox("Sniper Switch", &Backend_BlazeCheckbox(8))) {
			BlazeMemOnCheckboxToggled(8, "Sniper Switch",
				[]() { Aim.SniperSwitchon(); }, []() { Aim.SniperSwitchoff(); });
		}
	}
	custom::EndChild();
}

// Tab 1 — Visual (Esp toggles left with inline options, Extra right)
if (iTabs == 1)
{
	ImGui::SetCursorPos(ImVec2(180, 85 + fTabOffset));
	custom::Child("Esp", ICON_BRUSH_LINE, "Player Esp", ImVec2(270, ImGui::GetContentRegionAvail().y + 20), true, 0);
	{
		edited::Checkbox("Enable Esp", "Master Esp Toggle", &ui.esp.ESPMasterEnabled);

		edited::Checkbox("Esp Line", "Tracer To Enemies", &ui.esp.ESPLineEnabled, []() {
			edited::ColorEdit4("Line Color", NULL, ui.esp.ESPLineColor);
			edited::Combo("Line Pos", NULL, &ui.esp.ESPLineStartPos, "Top\0Center\0Bottom\0");
			edited::Checkbox("Top Line Logo", "Logo On Line Start", &ui.esp.ShowLineLogo);
		});

		edited::Checkbox("Esp Box", "Box Around Enemies", &ui.esp.ESPBoxEnabled, []() {
			edited::ColorEdit4("Box Color", NULL, ui.esp.ESPBoxColor);
			edited::Combo("Box Type", NULL, &ui.esp.ESPBoxMode, "Dynamic\0Cornered\0");
		});

		edited::Checkbox("Box Fill", "Filled Box Overlay", &ui.esp.ESPBoxFill, []() {
			edited::ColorEdit4("Fill Color", NULL, ui.esp.ESPBoxFillColor);
		});

		edited::Checkbox("Skeleton", "Bone Overlay", &ui.esp.ESPBoneEnabled, []() {
			edited::ColorEdit4("Bone Color", NULL, ui.esp.ESPBoneColor);
		});

		edited::Checkbox("Health Bar", "Hp Bar", &ui.esp.PlayerHealthBar);
		edited::Checkbox("Weapon Icon", "Weapon Glyph", &ui.esp.PlayerWeaponIcon);

		edited::Checkbox("Weapon Name", "Weapon Text", &ui.esp.PlayerWeaponNameEnabled, []() {
			edited::ColorEdit4("Weapon Color", NULL, ui.esp.PlayerWeaponNameColor);
		});

		edited::Checkbox("Enemy Name", "Player Name", &ui.esp.PlayerNameEnabled, []() {
			edited::ColorEdit4("Name Color", NULL, ui.esp.PlayerNameColor);
		});

		edited::Checkbox("Distance", "Distance Text", &ui.esp.PlayerDistanceEnabled, []() {
			edited::ColorEdit4("Distance Color", NULL, ui.esp.PlayerDistanceColor);
		});
	}
	custom::EndChild();

	ImGui::SetCursorPos(ImVec2(470, 85 + fTabOffset));
	custom::Child("Esp Extra", ICON_EYE_2_LINE, "Extra & Options", ImVec2(270, ImGui::GetContentRegionAvail().y + 20), true, 0);
	{
		edited::Checkbox("Esp Rank", "Rank Tag", &ui.esp.RankEnabled, []() {
			edited::ColorEdit4("Rank Color", NULL, ui.esp.RankColor);
		});

		edited::Checkbox("Origin Line", "Line Under 20m", &ui.esp.SnapLinesEnabled);
		edited::Checkbox("Wukong Mode", "Visible Only", &ui.esp.WukongMode);
		edited::Checkbox("Rainbow Esp", "Cycle Colors", &ui.esp.RainbowESP);
		edited::Checkbox("Ignore Training Bots", "Hide Training Bots", &ui.esp.IgnoreTrainingBots);
		edited::Checkbox("Invalid Timer", "Match Timer", &ui.esp.InvalidTimer);
		edited::SliderInt("Esp Distance", "Draw Distance (m)", &ui.esp.espmaxdis, 0, 200, "%d m");

		if (custom::Button("Refresh Esp", ImVec2(ImGui::GetContentRegionAvail().x, 35))) {
			g_Globals.EspConfig.Refresh = true;
		}
	}
	custom::EndChild();
}

// Tab 2 — Brutal + Machine (dual panel)
if (iTabs == 2)
{
	ImGui::SetCursorPos(ImVec2(180, 85 + fTabOffset));
	custom::Child("Brutal", ICON_COMPONENTS_LINE, "Silent & Pull", ImVec2(270, ImGui::GetContentRegionAvail().y + 20), true, 0);
	{
		

		edited::Checkbox("Speed Timer", "Faster Sprint", &ui.esp.SpeedTimerEnabled);

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
	}
	custom::EndChild();

	ImGui::SetCursorPos(ImVec2(470, 85 + fTabOffset));
	custom::Child("Machine", ICON_COMPONENTS_LINE, "Extra Exploits", ImVec2(270, ImGui::GetContentRegionAvail().y + 20), true, 0);
	{
		edited::Checkbox("Burst Fire", "Rapid Burst", &ui.esp.BurstFire);
		edited::Checkbox("Vision Hack", "See Farther", &ui.esp.VisionHackEnabled);
		edited::Checkbox("Down Player", "Push Model Down", &ui.esp.DownPlayerEnabled);

		if (edited::Checkbox("No Gravity Fly", "WASD Space Up Ctrl Down", &ui.esp.NoGravityFlyEnabled)) {
			Backend_Notify(ui.esp.NoGravityFlyEnabled ? "No Gravity Fly Enabled" : "No Gravity Fly Disabled",
				ui.esp.NoGravityFlyEnabled);
		}

		edited::Checkbox("Spin Player", "Spin Character", &ui.esp.SpinPlayerEnabled);
		if (ui.esp.SpinPlayerEnabled)
			edited::SliderFloat("Spin Speed", NULL, &ui.esp.SpinPlayerSpeed, 1.f, 15.f, "%.0fx");

		if (custom::Checkbox("Guest Reset", &Backend_BlazeCheckbox(9154))) {
			BlazeMemOnCheckboxToggled(9154, "Guest Reset",
				[]() { Aim.GuestResetON(); }, []() { Aim.GuestResetOFF(); });
		}
	}
	custom::EndChild();
}

// Tab 3 — Keybinds
if (iTabs == 3)
{
	ImGui::SetCursorPos(ImVec2(180, 85 + fTabOffset));
	custom::Child("Keybinds 1", ICON_KEYBOARD_LINE, "Primary Keys", ImVec2(270, ImGui::GetContentRegionAvail().y + 20), true, 0);
	{
		ImGui::Keybox("Hide Menu", &YorzenKey::HideMenuCheck, &YorzenKey::HideMenuKey);
		ImGui::Keybox("Close Menu", &YorzenKey::ClosedCheck, &YorzenKey::ClosedKey);
		ImGui::Keybox("Sniper Switch", &YorzenKey::SniperMacroCheck, &YorzenKey::SniperMacroKey);
		ImGui::Keybox("Aimbot On/Off", &YorzenKey::AimbotToggleCheck, &YorzenKey::AimbotToggleKey);
		ImGui::Keybox("Aimbot Legit Key", &YorzenKey::AimbotLegitCheck, &YorzenKey::AimbotLegitKey);
		ImGui::Keybox("External Aim Toggle", &YorzenKey::AimbotExtCheck, &YorzenKey::AimbotExtKey);
		ImGui::Keybox("Silent Aim Toggle", &YorzenKey::SilentAimCheck, &YorzenKey::SilentAimKey);
		ImGui::Keybox("Enemy Pull Toggle", &YorzenKey::EnemyPullCheck, &YorzenKey::EnemyPullKey);
		ImGui::Keybox("Teleport Kill", &YorzenKey::TeleportKillCheck, &YorzenKey::TeleportKillKey);
	}
	custom::EndChild();

	ImGui::SetCursorPos(ImVec2(470, 85 + fTabOffset));
	custom::Child("Keybinds 2", ICON_CRYSTAL_BALL_LINE, "Utility Keys", ImVec2(270, ImGui::GetContentRegionAvail().y + 20), true, 0);
	{
		ImGui::Keybox("Wall Hack", &YorzenKey::WallHack1Check, &YorzenKey::WallHack1Key);
		ImGui::Keybox("Fast Landing", &YorzenKey::WallHack2Check, &YorzenKey::WallHack2Key);
		ImGui::Keybox("Camera Right", &YorzenKey::CamaraJipiCheck, &YorzenKey::CamaraJipiKey);
		ImGui::Keybox("Refresh Esp", &YorzenKey::RefreshEspCheck, &YorzenKey::RefreshEspKey);
		ImGui::Keybox("Speed Timer", &YorzenKey::FakeLagCheck, &YorzenKey::FakeLagKey);
		ImGui::Keybox("Streamer Mode", &YorzenKey::StreamerModeCheck, &YorzenKey::StreamerModeKey);
		ImGui::Keybox("Wukong Mode", &YorzenKey::WukongModeCheck, &YorzenKey::WukongModeKey);
		ImGui::Keybox("Sniper Scope", &YorzenKey::SniperScopeCheck, &YorzenKey::SniperScopeKey);
	}
	custom::EndChild();
}

// Tab 4 — Settings
if (iTabs == 4)
{
	ImGui::SetCursorPos(ImVec2(180, 85 + fTabOffset));
	custom::Child("Settings", ICON_SETTINGS_2_LINE, "General Settings", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y + 20), true, 0);
	{
		std::string button_name = std::string(bTheme ? ICON_MOON_FILL : ICON_SUN_FILL) + " Change Color Theme" + std::string(bTheme ? ICON_MOON_FILL : ICON_SUN_FILL);
		if (custom::Button(button_name.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 35))) {
			bTheme = !bTheme;
		}

		if (custom::ColorEdit4("Primary Color", "Menu Accent Color", (float*)&c::main_color, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoInputs))
		{
			c::accent = c::main_color;
			c::text::text_hov = c::main_color;
		}

		static bool tempCleanerOnce = false;
		if (edited::Checkbox("Temp File Cleaner", "Delete Temp/Cache Files.", &tempCleanerOnce)) {
			if (tempCleanerOnce) {
				Backend_RunTempCleaner();
				tempCleanerOnce = false;
			}
		}

		if (custom::Checkbox("Mute", "Mutes  Checkbox Sound", &custom::GlobalMute));

		if (edited::Checkbox("Streamer Mode", "Hide From Capture", &ui.esp.EspStreamerMode)) {
			HWND overlayHwnd = FWork::Overlay::GetOverlayWindow();
			if (ui.esp.EspStreamerMode)
				SetWindowDisplayAffinity(overlayHwnd, WDA_EXCLUDEFROMCAPTURE);
			else
				SetWindowDisplayAffinity(overlayHwnd, WDA_NONE);
		}

		edited::Checkbox("Disable All Effects", "For Low-end Pcs. Disables Animations, Glow, And Heavy Shaders For Smoother Fps.", &ui.esp.DisableAllEffects);

		static bool saveConfigOnce = false;
		if (edited::Checkbox("Save Configuration", "Saves current settings for the next launch.", &saveConfigOnce)) {
			if (saveConfigOnce) {
				Backend_SaveConfig();
				saveConfigOnce = false;
			}
		}

		if (custom::Button("Reset Config", ImVec2(ImGui::GetContentRegionAvail().x, 40))) {
			Backend_ResetConfig();
		}

		

		if (custom::Button("Exit Panel", ImVec2(ImGui::GetContentRegionAvail().x, 40))) {
			Backend_ExitPanel();
		}
	}
	custom::EndChild();
}
