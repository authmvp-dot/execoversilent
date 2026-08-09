#pragma once

static bool checkboxes[60];
static int slider_int[60];
static float float_slider[60];
float color_edit[10][4];
static int combo[30];
static int keybind[30];
static int keybind_mode[30];

static ImColor test_col = ImColor(255, 255, 255, 240);


static auto mExecEditor = new TextEditor();


static bool hovered_esp_preview = false;

inline const TextEditor::Palette rE_palette = { {
				0xff7f7f7f,    // Default
				ImColor(187, 134, 192),    // Keyword    
				ImColor(181, 206, 155),    // Number
				ImColor(206, 145, 120),    // String
				ImColor(206, 145, 120), // Char literal
				0xffffffff, // Punctuation
				0xff408080,    // Preprocessor
				0xffaaaaaa, // Identifier
				ImColor(220, 205, 121), // Known identifier
				0xffc040a0, // Preproc identifier
				ImColor(106, 153, 62), // Comment (single line)
				ImColor(106, 153, 62), // Comment (multi line)
				c::second_color, // Background
				0xffe0e0e0, // Cursor
				0x80a06020, // Selection
				0x800020ff, // ErrorMarker
				0x40f08000, // Breakpoint
				ImColor(1.f, 1.f, 1.f, 0.2f), // Line number,
				0x40000000, // Current line fill
				0x40808080, // Current line fill (inactive)
				0x40a0a0a0, // Current line edge
			} };


void BarSmallText(const char* text, const char* text2)
{
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::Text(text); ImGui::SameLine();
	ImGui::PushFont(font::medium_small);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);
	ImGui::TextDisabled(text2);  ImGui::SameLine();
	ImGui::Dummy(ImVec2(5, 0)); ImGui::SameLine();
	ImGui::PopFont();
	ImGui::PopStyleVar();
}

static char input[256] = "Hello world";

static int iSubTabsAim = 0;
static int iSubTabsVisual = 0;
static int iSubTabsBrutal = 0;
static int iSubTabsKeys = 0;

const char* tab_list[] = { ICON_AIMING_2_LINE " Aim", ICON_EYE_2_LINE " Visual", ICON_SKULL_LINE " Brutal", ICON_SETTINGS_3_LINE " Settings" };
const char* tab_ico_list[] = { ICON_AIMING_2_LINE, ICON_EYE_2_LINE, ICON_SKULL_LINE, ICON_SETTINGS_3_LINE };


const char* tab_list1[] = { "Login", "Register" };
const char* tab_ico_list1[] = { ICON_ENTER_DOOR_LINE, ICON_INVITE_LINE };


const char* combo_list[] = { "Low", "Medium", "High", "Ultra" };
const char* theme_list[] = { "Classic", "Dark", "Light", "Neon", "Cyberpunk" };
const char* log_levels[] = { "Error", "Warning", "Info", "Debug", "Trace" };

static bool multi[5] = { false, false, false, false, false };
static bool blur_reuse = false;


namespace ImGui
{
    int rotation_start_index;
    void ImRotateStart()
    {
        rotation_start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size;
    }

    ImVec2 ImRotationCenter()
    {
        ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX); // bounds

        const auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
        for (int i = rotation_start_index; i < buf.Size; i++)
            l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

        return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2); // or use _ClipRectStack?
    }


    void ImRotateEnd(float rad, ImVec2 center = ImRotationCenter())
    {
        float s = sin(rad), c = cos(rad);
        center = ImRotate(center, s, c) - center;

        auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
        for (int i = rotation_start_index; i < buf.Size; i++)
            buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
    }
}

void ParticlesSpot()
{
    ImVec2 screen_size = { (float)GetSystemMetrics(SM_CXSCREEN), (float)GetSystemMetrics(SM_CYSCREEN) };

    static ImVec2 partile_pos[100];
    static ImVec2 partile_target_pos[100];
    static float partile_speed[100];
    static float partile_size[100];
    static float partile_radius[100];
    static float partile_rotate[100];

    for (int i = 1; i < 70; i++)
    {
        if (partile_pos[i].x == 0 || partile_pos[i].y == 0)
        {
            partile_pos[i].x = rand() % (int)screen_size.x + 1;
            partile_pos[i].y = (float)(rand() % 30);
            partile_speed[i] = 1 + rand() % 35;
            partile_radius[i] = rand() % 4;
            partile_size[i] = 1 + rand() % 3;
            partile_target_pos[i].x = rand() % (int)screen_size.x;
            partile_target_pos[i].y = screen_size.y * 2;
            partile_rotate[i] = 0;
        }

        partile_pos[i] = ImLerp(partile_pos[i], partile_target_pos[i], ImGui::GetIO().DeltaTime * (partile_speed[i] / 120));
        partile_rotate[i] += ImGui::GetIO().DeltaTime;

        if (partile_pos[i].y > screen_size.y)
        {
            partile_pos[i].x = 0;
            partile_pos[i].y = 0;
            partile_rotate[i] = 0;
        }

        ImGui::ImRotateStart();
        ImGui::GetWindowDrawList()->AddCircleFilled(partile_pos[i], partile_size[i], ImGui::GetColorU32(c::accent), 12);
        ImGui::GetWindowDrawList()->AddShadowCircle(partile_pos[i], 6.f, ImGui::GetColorU32(c::accent), 30.f + partile_size[i], ImVec2(0, 0), 0, 1);
        ImGui::ImRotateEnd(partile_rotate[i]);
    }
}

