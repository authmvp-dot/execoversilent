#pragma once

#include <array>
#include <string>
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_settings.h"

#include <vector>
#include <algorithm>
#include "imgui.h"

#ifndef SCALE
#define SCALE(x) (x)
#endif

#define ESP_DND_TYPE "ESP_ITEM"

struct item_state
{
    std::string      name;
    ImVec4           color;
    ImVec4           color_two;
    bool             double_col;
    int              font_type;
    int              position;
    bool             allowed_positions[4] = { true, true, true, true };
    bool             enabled = true;
    bool             active = false;
    bool             swapped = false;
    ImVec2           location = ImVec2(0, 0);
    ImRect           rect;
};

inline float esp_bar_size = 6.f;

struct add_item_state
{
    bool window_opened = false;
    float window_alpha = 0.f;
    bool window_hovered = false;
    float height{ 0 };
};

enum item_position
{
    position_top,
    position_bottom,
    position_left,
    position_right
};

struct preview_box
{
    ImVec2 position;
    ImVec2 size;
};

enum preview_area
{
    area_top,
    area_bottom,
    area_left,
    area_right,
    area_none
};

struct
{
    ImVec2 pos{ 0, 0 };
    ImVec2 window_size{ 320, 520 };
    float m_padding{ 20 };
    ImVec2 size{ 95, 210 };
    ImVec2 padding{ 10, 10 };
    ImVec2 spacing{ 10, 10 };
    float rounding{ 2 };
    float settings_width{ 260 };
    float window_padding{ 5 };
    std::vector<std::string> fonts{ "Regular", "Bold" };

    bool menu_opened{ false };
    float menu_height{ 0 };
    float menu_offset{ 0 };
    float menu_alpha{ 0 };
    ImVec2 menu_padding{ 20, 20 };
    ImVec2 menu_spacing{ 20, 20 };
} inline esp_info;

class esp_preview
{
public:

    template <typename T>
    T* anim_container(T** state_ptr, ImGuiID id)
    {
        T* state = static_cast<T*>(GetStateStorage()->GetVoidPtr(id));
        if (!state)
            GetStateStorage()->SetVoidPtr(id, state = new T());
        *state_ptr = state;
        return state;
    }

    std::vector<item_state> text =
    {
        { "Username", ImVec4(1.f, 1.f, 1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 1.f), true, 0, position_top },

        { "Distance", ImVec4(1.f, 1.f, 1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 1.f), true, 0, position_left, { true, true, true, true } },
        { "Flashed",  ImVec4(1.f, 1.f, 1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 1.f), true, 0, position_left, { true, true, true, true } },
        { "Money",    ImVec4(1.f, 1.f, 1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 1.f), true, 0, position_left, { true, true, true, true } },
        { "Zoom",     ImVec4(1.f, 1.f, 1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 1.f), true, 0, position_left, { true, true, true, true } },
        { "Hit",      ImVec4(1.f, 1.f, 1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 1.f), true, 0, position_left, { true, true, true, true } },

        { "Defusing", ImVec4(1.f, 1.f, 1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 1.f), true, 0, position_right, { true, true, true, true } },
        { "Scoped",   ImVec4(1.f, 1.f, 1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 1.f), true, 0, position_right, { true, true, true, true } },
        { "Bomb",     ImVec4(1.f, 1.f, 1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 1.f), true, 0, position_right, { true, true, true, true } },
        { "LC",       ImVec4(1.f, 1.f, 1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 1.f), true, 0, position_right, { true, true, true, true } },
    };

    std::vector<item_state> bar =
    {
        { "Health", ImVec4(0.9f, 0.25f, 0.25f, 1.f), ImVec4(0.9f, 0.25f, 0.25f, 1.f), false, 0,  position_left,  { true, true, true, true } },
        { "Armor",  ImVec4(0.25f, 0.55f, 0.9f, 1.f), ImVec4(0.25f, 0.55f, 0.9f, 1.f), false, 0,  position_right, { true, true, true, true } },
    };

    int box_padding = 5;
    int texts_spacing = 20;
    int bars_spacing = 10;

private:
    int texts_buffer[4] = { 0, 0, 0, 0 };
    int bars_buffer[4] = { 0, 0, 0, 0 };
    int hovered_area = area_none;
    float area_size = 80.f;
    float anim_speed = 12.f;
    item_state* dnd_preview_item = nullptr;
    int dnd_preview_type = -1;


public:
    enum
    {
        position_top = 0,
        position_bottom,
        position_left,
        position_right,
        area_none
    };

    void initialize_preview(const ImVec2& pos, const ImVec2& size)
    {
        fill_box(pos, size);
        fill_areas();
        fill_buffers();
    }

    void render_box()
    {
        fill_areas();
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        ImVec2 box_pos = box.position;
        ImVec2 box_sz = box.size;
        ImGui::GetWindowDrawList()->AddRect(window->Pos + box_pos + ImVec2(2, 2), window->Pos + box_pos + box_sz - -ImVec2(2, 2), IM_COL32(0, 0, 0, 150), 2.f);
        ImGui::GetWindowDrawList()->AddRect(window->Pos + box_pos - ImVec2(2, 2), window->Pos + box_pos + box_sz + -ImVec2(2, 2), IM_COL32(0, 0, 0, 150), 2.f);
        ImGui::GetWindowDrawList()->AddRect(window->Pos + box_pos, window->Pos + box_pos + box_sz, IM_COL32(200, 200, 200, 255), 2.f, 0, 2.f);
        render_settings_icon();
        accept_external_drop();
        draw_drag_preview();

    }

    void draw()
    {
        ImVec2 regionMin = ImGui::GetWindowContentRegionMin();
        ImVec2 regionMax = ImGui::GetWindowContentRegionMax();
        const float cw = regionMax.x - regionMin.x;
        const float ch = regionMax.y - regionMin.y;
        const ImVec2 boxSize(95.f, 210.f);
        const ImVec2 boxPos(
            regionMin.x + (cw - boxSize.x) * 0.5f,
            regionMin.y + (ch - boxSize.y) * 0.5f);

        fill_box(boxPos, boxSize);
        fill_buffers();
        render_box();
        for (auto& b : bar)
            render_bar(b);
        for (auto& t : text)
            render_text(t);
    }

    void render_text(item_state& state)
    {
        if (!state.enabled) return;

        add_item_state* anim = anim_container(&anim, ImGui::GetID(state.name.c_str()));
        const ImVec2 text_size = ImGui::CalcTextSize(state.name.c_str());
        const ImVec2 box_pos = box.position;
        ImGuiWindow* window = ImGui::GetCurrentWindow();

        ImVec2 pos_offset[] = {
            ImVec2(0.f, texts_buffer[state.position] * -texts_spacing
                        - bars_buffer[state.position] * bars_spacing),
            ImVec2(0.f, texts_buffer[state.position] * texts_spacing
                        + bars_buffer[state.position] * bars_spacing),
            ImVec2(bars_buffer[state.position] * -bars_spacing,
                   texts_buffer[state.position] * texts_spacing),
            ImVec2(bars_buffer[state.position] * bars_spacing,
                   texts_buffer[state.position] * texts_spacing),
        };
        texts_buffer[state.position]++;

        ImVec2 positions[] = {
            ImVec2(box_pos.x + box.size.x / 2 - text_size.x / 2,
                   box_pos.y - text_size.y - box_padding)
                + pos_offset[position_top],
            ImVec2(box_pos.x + box.size.x / 2 - text_size.x / 2,
                   box_pos.y + box.size.y + box_padding)
                + pos_offset[position_bottom],
            ImVec2(box_pos.x - text_size.x - box_padding,
                   box_pos.y)
                + pos_offset[position_left],
            ImVec2(box_pos.x + box.size.x + box_padding,
                   box_pos.y)
                + pos_offset[position_right],
        };

        state.location = ImLerp(
            state.location,
            positions[state.position],
            ImGui::GetIO().DeltaTime * anim_speed);

        if (state.swapped)
        {
            if (fabs(state.location.x - positions[state.position].x) < 1.f &&
                fabs(state.location.y - positions[state.position].y) < 1.f)
                state.swapped = false;
        }

        ImRect rect(
            window->Pos + state.location,
            window->Pos + state.location + text_size);
        state.rect = rect;

        const ImGuiHoveredFlags hv = ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem;
        if (ImGui::IsMouseHoveringRect(rect.Min, rect.Max) && ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered(hv))
            state.active = true;


        if (ImGui::IsMouseHoveringRect(rect.Min, rect.Max) &&
            ImGui::IsMouseClicked(0) &&
            ImGui::IsWindowHovered())
        {
            state.active = true;
        }

        if (state.active)
        {
            ImVec2 mouse = ImGui::GetMousePos() - ImVec2(text_size.x / 2, text_size.y / 2);
            auto drawList = ImGui::GetWindowDrawList();
            ImU32 outline_col = IM_COL32(0, 0, 0, 255);
            const ImVec2 outline_offsets[4] = {
                ImVec2(-1,  0),
                ImVec2(1,  0),
                ImVec2(0, -1),
                ImVec2(0,  1),
            };
            if (!state.double_col)
            {
                for (int i = 0; i < 4; i++)
                    drawList->AddText(
                        ImGui::GetFont(), ImGui::GetFontSize(),
                        mouse + outline_offsets[i],
                        outline_col,
                        state.name.c_str());
                drawList->AddText(
                    ImGui::GetFont(), ImGui::GetFontSize(),
                    mouse,
                    ImGui::GetColorU32(state.color),
                    state.name.c_str());
            }
            else
            {
                for (int i = 0; i < 4; i++)
                    drawList->AddText(
                        ImGui::GetFont(), ImGui::GetFontSize(),
                        mouse + outline_offsets[i],
                        outline_col,
                        state.name.c_str());
                int vtx_start = drawList->VtxBuffer.Size;
                drawList->AddText(
                    ImGui::GetFont(), ImGui::GetFontSize(),
                    mouse,
                    IM_COL32_WHITE,
                    state.name.c_str());
                int vtx_end = drawList->VtxBuffer.Size;
                ShadeVertsLinearColorGradientKeepAlpha(
                    drawList,
                    vtx_start, vtx_end,
                    mouse,
                    mouse + ImVec2(text_size.x, -text_size.y),
                    ImGui::GetColorU32(state.color),
                    ImGui::GetColorU32(state.color_two));
            }

            for (auto& stored : text)
            {
                if (stored.name == state.name ||
                    stored.position != state.position ||
                    stored.swapped ||
                    !stored.enabled)
                    continue;

                if (ImGui::IsMouseHoveringRect(stored.rect.Min, stored.rect.Max))
                {
                    std::swap(state, stored);
                    state.active = false;
                    state.swapped = true;
                    stored.active = true;
                    stored.swapped = true;
                }
            }

            if (hovered_area != area_none &&
                state.allowed_positions[hovered_area])
            {
                state.position = hovered_area;
            }

            if (ImGui::IsMouseReleased(0))
                state.active = false;
        }

        add_item_state* anim_popup = anim;
        if (!anim_popup->window_opened) {
            if ((ImGui::IsMouseHoveringRect(rect.Min, rect.Max) && ImGui::IsMouseClicked(1)) || (anim_popup->window_opened && (ImGui::IsMouseClicked(0) || ImGui::IsMouseClicked(1)))) {
                anim_popup->window_opened = true;
            }
        }

        anim_popup->window_alpha = ImClamp(
            anim_popup->window_alpha +
            (ImGui::GetIO().DeltaTime * anim_speed *
                (anim_popup->window_opened ? 1.f : -1.f)),
            0.f, 1.f);

        if (anim_popup->window_alpha > 0.01f)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, anim_popup->window_alpha);
            ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 0.f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, esp_info.rounding);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, esp_info.spacing);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, esp_info.padding);

            ImVec2 wpos = ImVec2(rect.Min.x, rect.Max.y + esp_info.window_padding);
            wpos = ImClamp(wpos, ImVec2(0, 0), ImGui::GetIO().DisplaySize - ImVec2(esp_info.settings_width, anim_popup->height));
            ImGui::SetNextWindowPos(wpos);
            ImGui::SetNextWindowSize(ImVec2(esp_info.settings_width, anim_popup->height));

            ImGui::Begin("ESP", nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);


            ImVec2 wp = ImGui::GetWindowPos();
            ImVec2 ws = ImGui::GetWindowSize();
            ImDrawList* dl = ImGui::GetWindowDrawList();
            dl->PushClipRect(ImVec2(0, 0), ImGui::GetMainViewport()->Size, false);

            dl->AddRectFilled(wp, wp + ws, c::second_color, 8.f);
            dl->AddRect(wp, wp + ws, c::stroke_color, 8.f);
            dl->PopClipRect();

            ImColor color_one = ImColor{ state.color.x, state.color.y, state.color.z, state.color.w };
            DWORD picker_flags = ImGuiColorEditFlags_NoSidePreview |  ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview;

            PushFont(font::inter_regular);
            ImGui::TextColored(utils::ImColorToImVec4(c::label::regular), state.name.c_str());
            PopFont();

            PushFont(font::inter_medium);
            custom::ColorEdit4("Color", "123", (float*)&color_one, picker_flags);

            if (custom::Button("Disable", ImVec2(ImGui::GetContentRegionAvail().x / 2, 35)))
            {
                state.enabled = false;
                anim_popup->window_opened = false;
            } ImGui::SameLine();
            if (custom::Button("Close", ImVec2(ImGui::GetContentRegionAvail().x, 35)))
                anim_popup->window_opened = !anim_popup->window_opened;
            state.color = ImVec4(color_one.Value.x, color_one.Value.y, color_one.Value.z, color_one.Value.w);

            anim_popup->window_hovered = ImGui::GetCurrentContext()->HoveredWindow &&
                strstr(ImGui::GetCurrentContext()->HoveredWindow->Name, "ESP");

            anim_popup->height = ImGui::GetCurrentWindow()->ContentSize.y + ImGui::GetStyle().WindowPadding.y * 2;
            PopFont();


            ImGui::End();


            ImGui::PopStyleVar(5);
        }

        auto drawList = ImGui::GetWindowDrawList();
        ImVec2 pos = window->Pos + state.location;
        ImU32 outline_col = IM_COL32(0, 0, 0, 255);
        const ImVec2 outline_offsets[4] = {
            ImVec2(-1,  0),
            ImVec2(1,  0),
            ImVec2(0, -1),
            ImVec2(0,  1),
        };
        for (int i = 0; i < 4; i++)
            drawList->AddText(
                ImGui::GetFont(), ImGui::GetFontSize(),
                pos + outline_offsets[i],
                outline_col,
                state.name.c_str());
        drawList->AddText(
            pos,
            ImGui::GetColorU32(state.color),
            state.name.c_str());
    }

    void render_bar(item_state& state)
    {
        if (!state.enabled) return;

        add_item_state* anim = anim_container(&anim, ImGui::GetID(state.name.c_str()));
        const ImVec2 box_pos = box.position;
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        float bar_thickness = esp_bar_size;

        ImVec2 pos_offset[] = {
            ImVec2(0.f, bars_buffer[state.position] * -bars_spacing),
            ImVec2(0.f, bars_buffer[state.position] * bars_spacing),
            ImVec2(bars_buffer[state.position] * -bars_spacing, 0.f),
            ImVec2(bars_buffer[state.position] * bars_spacing, 0.f),
        };

        bars_buffer[state.position]++;

        ImVec2 positions[] = {
            ImVec2(box_pos.x, box_pos.y - box_padding - bar_thickness) + pos_offset[position_top],
            ImVec2(box_pos.x, box_pos.y + box.size.y + box_padding) + pos_offset[position_bottom],
            ImVec2(box_pos.x - box_padding - bar_thickness, box_pos.y) + pos_offset[position_left],
            ImVec2(box_pos.x + box.size.x + box_padding, box_pos.y) + pos_offset[position_right],
        };

        ImVec2 bar_sizes[] = {
            ImVec2(box.size.x,            bar_thickness),
            ImVec2(box.size.x,            bar_thickness),
            ImVec2(bar_thickness,         box.size.y),
            ImVec2(bar_thickness,         box.size.y),
        };

        state.location = ImLerp(state.location, positions[state.position], ImGui::GetIO().DeltaTime * 12.f);

        if (state.swapped) {
            if (abs(state.location.x - positions[state.position].x) < 1.f && abs(state.location.y - positions[state.position].y) < 1.f) {
                state.swapped = false;
            }
        }

        ImRect rect = ImRect(ImGui::GetWindowPos() + state.location, ImGui::GetWindowPos() + state.location + bar_sizes[state.position]);
        state.rect = rect;

        if (ImGui::IsMouseHoveringRect(rect.Min, rect.Max) && ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered())
            state.active = true;

        const ImGuiHoveredFlags hv = ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem;
        if (ImGui::IsMouseHoveringRect(rect.Min, rect.Max) && ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered(hv))
            state.active = true;


        if (state.active)
        {
            if (!state.double_col)
                ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetMousePos() - rect.GetSize() / 2, ImGui::GetMousePos() + rect.GetSize() / 2, GetColorU32(state.color_two));
            else
            {
                const int vtx_start = GetForegroundDrawList()->VtxBuffer.Size;
                const int vtx_end = GetForegroundDrawList()->VtxBuffer.Size;
                if (state.position == position_left || state.position == position_right)
                    ShadeVertsLinearColorGradientKeepAlpha(GetForegroundDrawList(), vtx_start, vtx_end, GetMousePos() - ImVec2(0, rect.GetHeight() / 2), GetMousePos() + ImVec2(0, rect.GetHeight() / 2), GetColorU32(state.color), GetColorU32(state.color_two));
                else
                    ShadeVertsLinearColorGradientKeepAlpha(GetForegroundDrawList(), vtx_start, vtx_end, GetMousePos() - ImVec2(rect.GetWidth() / 2, 0), GetMousePos() + ImVec2(rect.GetWidth() / 2, 0), GetColorU32(state.color), GetColorU32(state.color_two));
            }

            for (auto& stored : bar) {
                if (stored.name == state.name || stored.position != state.position || stored.swapped || !stored.enabled)
                    continue;

                if (ImGui::IsMouseHoveringRect(stored.rect.Min, stored.rect.Max))
                {
                    std::swap(state, stored);
                    state.active = false;
                    state.swapped = true;
                    stored.active = true;
                    stored.swapped = true;
                }
            }

            if (hovered_area != area_none && state.allowed_positions[hovered_area])
            {
                state.position = hovered_area;
            }

            if (ImGui::IsMouseReleased(0))
            {
                state.active = false;
            }
        }

        if ((ImGui::IsMouseHoveringRect(rect.Min, rect.Max) && ImGui::IsMouseClicked(1) || anim->window_opened && (ImGui::IsMouseClicked(1) || ImGui::IsMouseClicked(0)) && !anim->window_hovered) && ImGui::IsWindowHovered())
            anim->window_opened = !anim->window_opened;

        anim->window_alpha = ImClamp(anim->window_alpha + (ImGui::GetIO().DeltaTime * 12.f * (anim->window_opened ? 1.f : -1.f)), 0.f, 1.f);

        if (anim->window_alpha > 0.01f)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, anim->window_alpha);
            ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 0.f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, esp_info.rounding);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, esp_info.spacing);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, esp_info.padding);
            ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.08f, 0.08f, 0.10f, 1.f));
            ImVec2 wpos = ImVec2(rect.Min.x, rect.GetCenter().y);
            wpos = ImClamp(wpos, ImVec2(0, 0), GetIO().DisplaySize - ImVec2(SCALE(esp_info.settings_width), anim->height));
            ImGui::SetNextWindowPos(wpos);
            ImGui::SetNextWindowSize(ImVec2(esp_info.settings_width, anim->height));
            ImGui::Begin("ESP", nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_AlwaysAutoResize);
            {
                if (anim->window_opened && IsMouseHoveringRect(GetWindowPos(), GetWindowPos() + GetWindowSize()))
                    SetWindowFocus();

                anim->window_hovered = GetCurrentContext()->HoveredWindow && strstr(GetCurrentContext()->HoveredWindow->Name, "ESP");

                float color_one[4] = { state.color.x, state.color.y, state.color.z, state.color.w };
                float color_two[4] = { state.color_two.x, state.color_two.y, state.color_two.z, state.color_two.w };

                custom::Checkbox("Gradient", &state.double_col);

                if (!state.double_col)
                {
                    state.color = ImVec4(color_one[0], color_one[1], color_one[2], color_one[3]);
                }
                else
                {
                    state.color = ImVec4(color_one[0], color_one[1], color_one[2], color_one[3]);
                    state.color_two = ImVec4(color_two[0], color_two[1], color_two[2], color_two[3]);
                }

                if (custom::Button("Disable", ImVec2(ImGui::GetContentRegionAvail().x, 35)))
                {
                    state.enabled = false;
                    anim->window_opened = false;
                }

                anim->height = GetCurrentWindow()->ContentSize.y + GetStyle().WindowPadding.y * 2;
            }
            ImGui::End();
            ImGui::PopStyleColor();
            ImGui::PopStyleVar(5);
        }

        if (!state.double_col)
            ImGui::GetWindowDrawList()->AddRectFilled(rect.Min, rect.Max, GetColorU32(state.color));
        else
        {
            const int vtx_start = window->DrawList->VtxBuffer.Size;
            ImGui::GetWindowDrawList()->AddRectFilled(rect.Min, rect.Max, ImColor{ 1.f, 1.f, 1.f, 1.f });
            const int vtx_end = window->DrawList->VtxBuffer.Size;
            if (state.position == position_left || state.position == position_right)
                ShadeVertsLinearColorGradientKeepAlpha(window->DrawList, vtx_start, vtx_end, rect.Min, ImVec2(rect.Min.x, rect.Max.y), GetColorU32(state.color), GetColorU32(state.color_two));
            else
                ShadeVertsLinearColorGradientKeepAlpha(window->DrawList, vtx_start, vtx_end, rect.Min, ImVec2(rect.Max.x, rect.Min.y), GetColorU32(state.color), GetColorU32(state.color_two));
        }
    }

private:
    struct preview_box { ImVec2 position; ImVec2 size; } box;
    ImRect      area_rect[4];

    void draw_drag_preview()
    {
        if (!ImGui::IsDragDropActive() || dnd_preview_item == nullptr) { dnd_preview_item = nullptr; dnd_preview_type = -1; return; }
        ImDrawList* dl = ImGui::GetForegroundDrawList();
        const char* txt = dnd_preview_item->name.c_str();
        ImVec2 ts = ImGui::CalcTextSize(txt);
        ImVec2 pos = ImGui::GetMousePos() - ts * 0.5f;
        ImU32 outline = IM_COL32(0, 0, 0, 255);
        const ImVec2 off[4] = { ImVec2(-1,0), ImVec2(1,0), ImVec2(0,-1), ImVec2(0,1) };
        for (int i = 0; i < 4; i++) dl->AddText(ImGui::GetFont(), ImGui::GetFontSize(), pos + off[i], outline, txt);
        if (!dnd_preview_item->double_col)
            dl->AddText(pos, ImGui::GetColorU32(dnd_preview_item->color), txt);
        else
        {
            int v0 = dl->VtxBuffer.Size;
            dl->AddText(ImGui::GetFont(), ImGui::GetFontSize(), pos, IM_COL32_WHITE, txt);
            int v1 = dl->VtxBuffer.Size;
            ShadeVertsLinearColorGradientKeepAlpha(dl, v0, v1, pos, pos + ImVec2(ts.x, -ts.y), ImGui::GetColorU32(dnd_preview_item->color), ImGui::GetColorU32(dnd_preview_item->color_two));
        }
    }

    void render_settings_icon()
    {
        ImGuiWindow* w = ImGui::GetCurrentWindow();
        ImVec2 s(ImGui::GetFontSize() + 8, ImGui::GetFontSize() + 8);
        ImVec2 p = w->Pos + ImVec2(w->Size.x - s.x - 8, 8);

        ImGui::SetCursorScreenPos(p);
        ImGui::PushID("esp_settings_icon");
        ImGui::InvisibleButton("btn", s);
        bool hovered = ImGui::IsItemHovered();
        bool clicked = ImGui::IsItemClicked();
        w->DrawList->AddRectFilled(p, p + s, IM_COL32(0, 0, 0, hovered ? 90 : 60), 6);

#ifdef ICON_SETTINGS_6_FILL
        const char* icon_txt = ICON_SETTINGS_6_FILL;
#else
        const char* icon_txt = u8"⚙";
#endif
        ImVec2 ts = ImGui::CalcTextSize(icon_txt);
        ImVec2 tp = p + (s - ts) * 0.5f;
        w->DrawList->AddText(tp, IM_COL32(255, 255, 255, 255), icon_txt);
        if (clicked) esp_info.menu_opened = !esp_info.menu_opened;
        ImGui::PopID();

        esp_info.menu_alpha = ImClamp(esp_info.menu_alpha + (ImGui::GetIO().DeltaTime * 14.f * (esp_info.menu_opened ? 1.f : -1.f)), 0.f, 1.f);

        if (esp_info.menu_alpha <= 0.01f) return;

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, esp_info.menu_alpha);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 8));

        ImVec2 base = p + ImVec2(-esp_info.settings_width + s.x, s.y + 6);
        ImVec2 appear_off = ImVec2(0, (1.f - esp_info.menu_alpha) * -10.f);
        ImGui::SetNextWindowPos(base + appear_off + ImVec2(150, -80));
        ImGui::SetNextWindowSize(ImVec2(esp_info.settings_width, -1));
        ImGui::Begin("##esp_settings_popup", nullptr,
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysUseWindowPadding);

        {
            ImVec2 wp = ImGui::GetWindowPos();
            ImVec2 ws = ImGui::GetWindowSize();
            ImDrawList* dl = ImGui::GetWindowDrawList();
            dl->PushClipRect(ImVec2(0, 0), ImGui::GetMainViewport()->Size, false);
            
            dl->AddRectFilled(wp, wp + ws, c::second_color, 8.f);
            dl->AddRect(wp, wp + ws, c::stroke_color, 8.f);
            dl->PopClipRect();
        }

        static bool outside_down_l = false, outside_down_r = false;

        ImVec2 wp = ImGui::GetWindowPos();
        ImVec2 ws = ImGui::GetWindowSize();
        ImRect popup_rect(wp, wp + ws);

        ImVec2 m = ImGui::GetMousePos();
        if (ImGui::IsMouseClicked(0)) outside_down_l = (!popup_rect.Contains(m) && !hovered);
        if (ImGui::IsMouseClicked(1)) outside_down_r = (!popup_rect.Contains(m) && !hovered);
        if ((outside_down_l && ImGui::IsMouseReleased(0)) || (outside_down_r && ImGui::IsMouseReleased(1)))
        {
            bool still_out = (!popup_rect.Contains(ImGui::GetMousePos()) && !hovered);
            if (still_out && !ImGui::IsDragDropActive()) esp_info.menu_opened = false;
            outside_down_l = outside_down_r = false;
        }

        static bool is_clean = true;
        static int iSubtabs = 0;




        auto FlowList = [&](int type, std::vector<item_state>& list, bool need_enabled)
            {
                float start_x = ImGui::GetCursorPosX();
                float max_x = start_x + ImGui::GetContentRegionAvail().x;
                float spacing = ImGui::GetStyle().ItemSpacing.x;

                int disabled_count = std::count_if(list.begin(), list.end(),
                    [&](const item_state& it) { return it.enabled == false; });

                if (disabled_count == 0 && type == 0 && iSubtabs == 0)
                {
                    PushFont(font::inter_regular);
                    ImGui::TextColored(utils::ImColorToImVec4(c::label::regular), "Disabled items will appear here.");
                    PopFont();
                }

                if (disabled_count == list.size() && iSubtabs == 1 && type == 0)
                {
                    PushFont(font::inter_regular);
                    ImGui::TextColored(utils::ImColorToImVec4(c::label::regular), "Enabled items will appear here.");
                    PopFont();
                }

                for (int i = 0; i < (int)list.size(); ++i)
                {
                    if (list[i].enabled != need_enabled) continue;
                    const char* label = list[i].name.c_str();
                    ImVec2 sz(ImGui::CalcTextSize(label).x + 1.f, 11.f);

                    if (ImGui::GetCursorPosX() + sz.x > max_x && ImGui::GetCursorPosX() > start_x)
                        ImGui::NewLine();
                    ImGui::TextColoredAnim(label, ImGui::IsMouseHoveringRect(ImGui::GetCursorScreenPos(), ImGui::GetCursorScreenPos() + CalcTextSize(label)) ? (list[i].enabled ? ImVec4(1.f, 0.5f, 0.5f, 1.f) : ImVec4(1.f, 1.f, 1.f, 1.f)) : ImVec4(0.8f, 0.8f, 0.8f, 1.f), label);

                    if (ImGui::IsItemClicked(0))
                    {
                        list[i].enabled = false;
                    }

                    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                    {
                        int pl[2] = { type, i };
                        ImGui::SetDragDropPayload(ESP_DND_TYPE, pl, sizeof(pl));
                        dnd_preview_item = &list[i];
                        dnd_preview_type = type;
                        ImGui::Dummy(ImVec2(sz.x, sz.y));
                        ImGui::EndDragDropSource();
                    }

                    ImGui::SameLine(0.f, spacing);
                }
                ImGui::NewLine();
            };


        {


            custom::SubTab("Disabled", &iSubtabs, 0); ImGui::SameLine();
            custom::SubTab("Active", &iSubtabs, 1);

            if (iSubtabs == 0)
            {
                FlowList(0, text, false);
                FlowList(1, bar, false);
            }
            if (iSubtabs == 1)
            {
                FlowList(0, text, true);
                FlowList(1, bar, true);
            }

        }

        ImGui::End();
        ImGui::PopStyleVar(4);
    }


    void accept_external_drop()
    {
        for (int i = 0; i < 4; ++i)
        {
            ImGui::SetCursorScreenPos(area_rect[i].Min);
            ImGui::Dummy(area_rect[i].GetSize());
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ESP_DND_TYPE))
                {
                    const int* pl = (const int*)payload->Data;
                    int t = pl[0];
                    int idx = pl[1];
                    if (t == 0)
                    {
                        if (idx >= 0 && idx < (int)text.size() && text[idx].allowed_positions[i])
                        {
                            text[idx].position = i;
                            text[idx].enabled = true;
                        }
                    }
                    else
                    {
                        if (idx >= 0 && idx < (int)bar.size() && bar[idx].allowed_positions[i])
                        {
                            bar[idx].position = i;
                            bar[idx].enabled = true;
                        }
                    }
                }
                ImGui::EndDragDropTarget();
            }
        }
    }

    void fill_box(const ImVec2& pos, const ImVec2& size)
    {
        box.position = pos;
        box.size = size;
    }

    void fill_areas()
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        area_rect[position_top] = ImRect(window->Pos + box.position - ImVec2(0, SCALE(area_size)), window->Pos + box.position + ImVec2(box.size.x, 0));
        area_rect[position_bottom] = ImRect(window->Pos + box.position + ImVec2(0, box.size.y), window->Pos + box.position + box.size + ImVec2(0, SCALE(area_size)));
        area_rect[position_left] = ImRect(window->Pos + box.position - ImVec2(SCALE(area_size), 0), window->Pos + box.position + ImVec2(0, box.size.y));
        area_rect[position_right] = ImRect(window->Pos + box.position + ImVec2(box.size.x, 0), window->Pos + box.position + box.size + ImVec2(SCALE(area_size), 0));
        hovered_area = area_none;
        for (int i = 0; i < 4; i++)
            if (ImGui::IsMouseHoveringRect(area_rect[i].Min, area_rect[i].Max))
                hovered_area = i;
    }

    void fill_buffers()
    {
        std::fill(std::begin(texts_buffer), std::end(texts_buffer), 0);
        std::fill(std::begin(bars_buffer), std::end(bars_buffer), 0);
    }
};

inline esp_preview* esp_p = new esp_preview();
