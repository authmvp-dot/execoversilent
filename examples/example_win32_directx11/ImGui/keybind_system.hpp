#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "imgui.h"
#include "imgui_settings.h"
#include "custom_popup.hpp"
#include "custom_widgets.hpp"
#include <map>
#include <vector>

custom_popup popup("keybind popup");

custom::chroma rainbow;

class ckeybind_system
{
private:
    float scale = 0.75f; // Масштаб клавиатуры

    // Базовые размеры клавиш (в пикселях)
    const float base_key_width = 40.f; // Ширина обычной клавиши
    const float base_key_height = 40.f; // Высота обычной клавиши
    const float base_large_key_width = base_key_width * 1.5f; // Ширина больших клавиш (Shift, Enter и т.д.)
    const float base_spacebar_width = base_key_width * 6.25f; // Ширина пробела

    static inline float keys_alpha = 1.f;
    static inline float popup_width = 515;
    static inline float current_height = 315;
    static inline bool saving = false;
    static inline bool saved = false;

    struct bind_state
    {

    };

    struct key_info
    {
        ImGuiKey key;
        bool hold;
        float alpha = 0.f; 
    };

    struct button_state
    {
        ImVec4 background, text;
        float shadow_thinkess;
    };

    static inline std::string general_full_text;

    static inline key_info key_buffer;

    static inline std::vector<key_info> binds;

    bool bind_info(key_info key)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        ImGuiID id = window->GetID(GetKeyName(key.key));

        ImVec2 scaled_size = ImVec2(base_key_width * 0.75f * scale + CalcTextSize(GetKeyName(key.key)).x, base_key_height * scale);
        ImVec2 button_pos = window->DC.CursorPos;
        ImVec2 close_size = CalcTextSize(ICON_CLOSE_LINE);
        ImRect button_bb(button_pos, button_pos + scaled_size + ImVec2(close_size.x, 0));
        ImRect close_bb(ImVec2(button_bb.Max.x - close_size.x - 7.f, button_bb.Min.y), button_bb.Max - ImVec2(7.f, 0.f));


        ImGui::ItemSize(button_bb, 0.f);
        ImGui::ItemAdd(button_bb, id);

        bool hovered, held, pressed = ImGui::ButtonBehavior(button_bb, id, &hovered, &held);
        bool key_pressed = ImGui::IsKeyPressed(key.key);

        static std::map<ImGuiID, button_state> anim;
        auto it_anim = anim.find(id);
        if (it_anim == anim.end()) {
            anim.insert({ id, button_state() });
            it_anim = anim.find(id);
        }

        static std::map<ImGuiID, bool> is_held_anim;

        is_held_anim[id] = true;

        ImVec4 target_bg = is_held_anim[id] ? c::main_color : hovered ? c::second_color : c::background_color;
        ImColor target_text = held ? ImGui::GetColorU32(c::label::active.Value) : hovered ? ImGui::GetColorU32(c::label::hovered.Value) : ImGui::GetColorU32(c::label::regular.Value);
        float target_shadow = 1.f;

        it_anim->second.background = ImLerp(it_anim->second.background, target_bg, g.IO.DeltaTime * 6.f);
        it_anim->second.text = ImLerp(it_anim->second.text, target_text, g.IO.DeltaTime * 6.f);
        it_anim->second.shadow_thinkess = ImLerp(it_anim->second.shadow_thinkess, target_shadow, g.IO.DeltaTime * 6.f);

        window->DrawList->AddRectFilled(button_bb.Min, button_bb.Max, GetColorU32(c::second_color.Value), 4.f);

        window->DrawList->AddText(close_bb.GetCenter() - CalcTextSize(ICON_CLOSE_LINE) / 2, GetColorU32(c::label::regular.Value), ICON_CLOSE_LINE);

        window->DrawList->AddText(ImVec2(button_bb.Min.x + 10.f, utils::center_text(button_bb.Min, button_bb.Max, GetKeyName(key.key)).y), ImGui::ColorConvertFloat4ToU32(it_anim->second.text), GetKeyName(key.key));

        return close_bb.Contains(GetMousePos()) && ImGui::IsMouseClicked(0);
    }

    void draw_key(const char* label, ImGuiKey key_id, float width_multiplier = 1.0f)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return;

        ImGuiContext& g = *GImGui;
        ImGuiID id = window->GetID(label);

        ImVec2 scaled_size = ImVec2(base_key_width * width_multiplier * scale, base_key_height * scale);
        ImVec2 button_pos = window->DC.CursorPos;
        ImRect button_bb(button_pos, button_pos + scaled_size);

        ImGui::ItemSize(button_bb, 0.f);
        ImGui::ItemAdd(button_bb, id);

        bool hovered, held, pressed = ImGui::ButtonBehavior(button_bb, id, &hovered, &held);
        bool key_pressed = ImGui::IsKeyPressed(key_id);

        static std::map<ImGuiID, button_state> anim;
        auto it_anim = anim.find(id);
        if (it_anim == anim.end()) {
            anim.insert({ id, button_state() });
            it_anim = anim.find(id);
        }

        static std::map<ImGuiID, bool> is_held_anim;
        if (pressed || key_pressed)
            is_held_anim[id] = true;
        else if (!held && !key_pressed)
            is_held_anim[id] = false;

        ImColor target_bg = held ? ImGui::GetColorU32(c::main_color.Value) : hovered ? ImGui::GetColorU32(c::second_color.Value) : ImGui::GetColorU32(c::second_color.Value);
        ImColor target_text = held ? ImGui::GetColorU32(c::label::active.Value) : hovered ? ImGui::GetColorU32(c::label::hovered.Value) : ImGui::GetColorU32(c::label::regular.Value);
        float target_shadow = held ? 1.f : 0.f;

        it_anim->second.background = ImLerp(it_anim->second.background, target_bg, g.IO.DeltaTime * 6.f);
        it_anim->second.text = ImLerp(it_anim->second.text, target_text, g.IO.DeltaTime * 6.f);
        it_anim->second.shadow_thinkess = ImLerp(it_anim->second.shadow_thinkess, target_shadow, g.IO.DeltaTime * 6.f);

        window->DrawList->AddRectFilled(button_bb.Min, button_bb.Max, ImGui::ColorConvertFloat4ToU32(it_anim->second.background), 4.f);
        rainbow.RenderRect(button_bb.Min, button_bb.Max, 4.f, true, it_anim->second.shadow_thinkess, label, it_anim->second.shadow_thinkess);
        window->DrawList->AddText(utils::center_text(button_bb.Min, button_bb.Max, label), ImGui::ColorConvertFloat4ToU32(it_anim->second.text), label);

        ImGui::SameLine(0, 4.f * scale);

        if (pressed) {
            key_buffer = { key_id, false };

            binds.push_back(key_buffer);
        }
    }

    void remove_bind(ImGuiKey key_to_remove)
    {
        auto it = std::remove_if(binds.begin(), binds.end(),
            [&](const key_info& key) { return key.key == key_to_remove; });

        if (it != binds.end())
            binds.erase(it, binds.end()); // Удаляем элементы
    }

public:

    void draw_mouse()
    {

        draw_key("L. Mouse", ImGuiKey_MouseLeft, 3.1f);
        draw_key("R. Mouse", ImGuiKey_MouseRight, 3.1f);
        draw_key("M. Mouse", ImGuiKey_MouseMiddle, 3.2f);
        draw_key("X1 Mouse", ImGuiKey_MouseX1, 3.2f);
        draw_key("X2 Mouse", ImGuiKey_MouseX2, 3.25f);
    }

    void draw_keyboard()
    {
        float row_width = 14.0f;

        draw_key("~", ImGuiKey_GraveAccent);
        draw_key("1", ImGuiKey_1);
        draw_key("2", ImGuiKey_2);
        draw_key("3", ImGuiKey_3);
        draw_key("4", ImGuiKey_4);
        draw_key("5", ImGuiKey_5);
        draw_key("6", ImGuiKey_6);
        draw_key("7", ImGuiKey_7);
        draw_key("8", ImGuiKey_8);
        draw_key("9", ImGuiKey_9);
        draw_key("0", ImGuiKey_0);
        draw_key("-", ImGuiKey_Minus);
        draw_key("=", ImGuiKey_Equal);
        draw_key("Back", ImGuiKey_Backspace, 2.0f);
        ImGui::NewLine();

        draw_key("Tab", ImGuiKey_Tab, 1.5f);
        draw_key("Q", ImGuiKey_Q);
        draw_key("W", ImGuiKey_W);
        draw_key("E", ImGuiKey_E);
        draw_key("R", ImGuiKey_R);
        draw_key("T", ImGuiKey_T);
        draw_key("Y", ImGuiKey_Y);
        draw_key("U", ImGuiKey_U);
        draw_key("I", ImGuiKey_I);
        draw_key("O", ImGuiKey_O);
        draw_key("P", ImGuiKey_P);
        draw_key("[", ImGuiKey_LeftBracket);
        draw_key("]", ImGuiKey_RightBracket);
        draw_key("\\", ImGuiKey_Backslash, 1.5f);
        ImGui::NewLine();

        draw_key("Caps", ImGuiKey_CapsLock, 1.75f);
        draw_key("A", ImGuiKey_A);
        draw_key("S", ImGuiKey_S);
        draw_key("D", ImGuiKey_D);
        draw_key("F", ImGuiKey_F);
        draw_key("G", ImGuiKey_G);
        draw_key("H", ImGuiKey_H);
        draw_key("J", ImGuiKey_J);
        draw_key("K", ImGuiKey_K);
        draw_key("L", ImGuiKey_L);
        draw_key(";", ImGuiKey_Semicolon);
        draw_key("'", ImGuiKey_Apostrophe);
        draw_key("Enter", ImGuiKey_Enter, 2.31f);
        ImGui::NewLine();

        draw_key("Shift", ImGuiKey_LeftShift, 2.35f);
        draw_key("Z", ImGuiKey_Z);
        draw_key("X", ImGuiKey_X);
        draw_key("C", ImGuiKey_C);
        draw_key("V", ImGuiKey_V);
        draw_key("B", ImGuiKey_B);
        draw_key("N", ImGuiKey_N);
        draw_key("M", ImGuiKey_M);
        draw_key(",", ImGuiKey_Comma);
        draw_key(".", ImGuiKey_Period);
        draw_key("/", ImGuiKey_Slash);
        draw_key("Shift", ImGuiKey_RightShift, 2.8f);
        ImGui::NewLine();

        draw_key("Ctrl", ImGuiKey_LeftCtrl, 1.75f);
        draw_key("Win", ImGuiKey_LeftSuper, 1.75f);
        draw_key("Alt", ImGuiKey_LeftAlt, 1.75f);
        draw_key("Space", ImGuiKey_Space, 7.0f);
        draw_key("Alt", ImGuiKey_RightAlt, 1.75f);
        draw_key("Ctrl", ImGuiKey_RightCtrl, 1.75f);
        ImGui::NewLine();
        draw_mouse();
    }

    float current_x_offset;

    void animate_keybinds(std::vector<key_info>& binds)
    {
        ImGuiContext& g = *GImGui;
        static DWORD dwTickStart = 0; // Начальное время старта анимации

        if (saving && dwTickStart == 0) {
            dwTickStart = GetTickCount(); // Обновляем старт анимации при каждом сохранении
        }

        std::string full_text;
        for (int i = 0; i < binds.size(); i++) {
            std::string key_str = ImGui::GetKeyName(binds[i].key);
            full_text += (i != 0 ? "  +  " : "") + key_str;
        }
        general_full_text = full_text;

        ImVec2 text_pos = ImGui::GetCurrentWindow()->Rect().GetCenter() - CalcTextSize(full_text.c_str()) / 2;
        current_x_offset = 0.f;

        bool all_faded_out = true; // Флаг проверки полного исчезновения

        for (int i = 0; i < binds.size(); i++) {
            std::string key_str = ImGui::GetKeyName(binds[i].key);
            std::string plus_str = (i == 0) ? "" : "  +  "; // Пробелы для выравнивания

            // Определяем, должны ли клавиши появляться или исчезать
            if (GetTickCount() - dwTickStart < (1000 * binds.size()) + 2000) {
                if (GetTickCount() - dwTickStart > 1000 * i + 1000) {
                    binds[i].alpha = ImLerp(binds[i].alpha, 1.f, g.IO.DeltaTime * 7.f);
                }
                all_faded_out = false;
            }
            else {
                binds[i].alpha = ImLerp(binds[i].alpha, 0.f, g.IO.DeltaTime * 7.f);
                if (binds[i].alpha > 0.05f) {
                    all_faded_out = false;
                }
            }

            float plus_alpha = (i == 0) ? 0.f : binds[i].alpha; // Анимация для `+`

            // Отрисовка "+"
            if (i > 0) {
                ImGui::GetWindowDrawList()->AddText(
                    text_pos + ImVec2(current_x_offset + 5.f, 0),
                    utils::GetColorWithAlpha(c::label::active, plus_alpha),
                    "+"
                );
                current_x_offset += CalcTextSize("+").x + CalcTextSize("  ").x; // Учитываем пробелы
            }

            // Отрисовка клавиши с учетом анимации появления
            ImGui::GetWindowDrawList()->AddText(
                text_pos + ImVec2(current_x_offset, 0),
                utils::GetColorWithAlpha(c::label::active, binds[i].alpha),
                key_str.c_str()
            );

            current_x_offset += CalcTextSize(key_str.c_str()).x;
        }

        // Завершаем анимацию и сбрасываем состояния
        if (all_faded_out && saving) {
            dwTickStart = 0; // Сброс времени старта
            saving = false;
            saved = true;
        }
    }

    void draw_with_popup()
    {
        if (custom::Button("Open keybind", ImVec2(ImGui::GetContentRegionAvail().x, 35))) {
            popup.open();
        }

        

        current_height = ImLerp(current_height, saving || popup.is_closing() ? 40.f : binds.size() > 0 ? 322 : 312, ImGui::GetAnimSpeed());

        if (popup.begin(current_height, "123")) {

            if (popup.animated_button(ICON_CLOSE_LINE, ImDrawFlags_RoundCornersLeft)) {
                popup.close();
            } ImGui::SameLine();

            keys_alpha = ImLerp(keys_alpha, !saving ? 1.f : 0.f, ImGui::GetIO().DeltaTime * 4.f);
            popup_width = ImLerp(popup_width, !saving ? 550 : (50 + ImGui::CalcTextSize(general_full_text.c_str()).x), ImGui::GetIO().DeltaTime * 4.f);

            ImVec2 back_cursor_pos = ImGui::GetCursorPos();

            ImGui::Dummy(ImVec2(popup_width, ImGui::GetContentRegionAvail().y));

            ImGui::SetCursorPos(back_cursor_pos);

                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, keys_alpha);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
                ImGui::BeginChild("Keybinds##Window$$$", ImVec2(ImGui::GetContentRegionAvail().x, -1.f), false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar);
                {
                    if (keys_alpha > 0.05f || saved) {
                        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 3));

                        ImGui::PushFont(font::medium_small);
                        for (int i = 0; i < binds.size(); i++)
                        {
                            if (bind_info(binds[i])) remove_bind(binds[i].key);
                            ImGui::SameLine(0, 4.f);
                        }
                        ImGui::PopStyleVar();

                        ImGui::NewLine();
                        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(3, 3));
                        ImGui::Spacing();
                        ImGui::Separator();


                        draw_keyboard();
                        ImGui::PopFont();

                        ImGui::NewLine();
                        ImGui::Spacing();
                        ImGui::Separator();
                        ImGui::Spacing();

                        if (custom::Button("Save", ImVec2(ImGui::GetContentRegionAvail().x / 2, 50))) {
                            saving = true;
                            saved = false;
                        }

                        ImGui::SameLine();
                        if (custom::Button("Reset", ImVec2(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().WindowPadding.x, 50)))
                            binds.clear();

                        ImGui::PopStyleVar();
                    }
                    else
                    {
                        animate_keybinds(binds);
                    }

                }ImGui::EndChild(false);
                ImGui::PopStyleVar(3);
        
            popup.end();
        }

    }
};