#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

#include <array>
#include <string>
#include <algorithm>
#include "imgui.h"
#include "imgui_internal.h"
#include <src/Globals.hpp>
#include <src/Fonts/Fonts.hpp>
#include <cmath>

static int esp_scale = 19;

namespace esp_preview_layout {
    static constexpr int   kBoxW = 124;
    static constexpr int   kBoxH = 210;
    static constexpr float kWeaponIconFontSize = 24.f;
    static constexpr float kLabelFontSize = 13.f;
}

namespace esp_drag_detail {
    inline float vec2_dist(const ImVec2& a, const ImVec2& b) {
        const float dx = a.x - b.x;
        const float dy = a.y - b.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    inline ImU32 color_alpha(ImColor c, float alpha_mul) {
        return ImGui::ColorConvertFloat4ToU32(ImVec4(
            c.Value.x, c.Value.y, c.Value.z, c.Value.w * alpha_mul));
    }
}

namespace esp_preview {
    inline ImU32 box_color_u32(float alpha) {
        const ImVec4& c = g_Globals.Visuals.BoxColor.Value;
        return IM_COL32((int)(c.x * 255.f), (int)(c.y * 255.f), (int)(c.z * 255.f), (int)(c.w * alpha * 255.f));
    }

    inline ImU32 skeleton_color_u32(float alpha) {
        const ImVec4& c = g_Globals.Visuals.SkeletonColor.Value;
        return IM_COL32((int)(c.x * 255.f), (int)(c.y * 255.f), (int)(c.z * 255.f), (int)(c.w * alpha * 255.f));
    }
}

class c_esp_drag {
public:
    class Box_t {
    public:
        int x, y, w, h;
    };

    struct Position {
        ImVec2 pos;
    };

    class c_drag_item {
    public:
        int pos = 0;
        int type = 0;
        ImColor col;
        std::string text;
        std::string name;
        bool small_text = false;
        ImVec2 pos_;
        ImVec2 size;
        bool hovered = false;
        int helding = 0;
        float move_animation = 0;
        float animations[6] = {};
        bool enabled = true;
        int think_pos = -1;
        bool enable_popup = false;
        int font = 0;
    };

    std::array<c_drag_item, 8> m_items = {
        c_drag_item{0, 1, ImColor(0, 255, 12), "Health bar", "Health bar"},
        c_drag_item{3, 0, ImColor(25, 120, 245), "\ue078", "Weapon Icon"},
        c_drag_item{2, 0, ImColor(255, 255, 255), "Nickname", "Nickname"},
        c_drag_item{3, 0, ImColor(255, 255, 255), "125m", "Distance", 1},
        c_drag_item{1, 0, ImColor(25, 110, 245), "Scoped", "SCOPED", 1},
        c_drag_item{1, 0, ImColor(255, 120, 0), "FD", "FD", 1},
        c_drag_item{1, 0, ImColor(255, 0, 0), "C4", "C4", 1},
        c_drag_item{2, 0, ImColor(255, 255, 255), "SCAR", "SCAR"} };
    int m_offsets[8] = {};
    Box_t box{};
    bool m_layoutLoadedFromGlobals = false;
    float m_boxAnim = 1.f;
    float m_skeletonAnim = 1.f;
    float m_fillAnim = 0.f;
    bool m_itemWasEnabled[8] = {};

    static int clamp_side_pos(int pos) {
        if (pos < 0 || pos > 3)
            return 0;
        return pos;
    }

    bool preview_item_visible(int idx) const {
        // Layout items always visible in preview (Eclipse) so drag handles stay usable.
        if (idx <= 3 || idx == 7)
            return true;
        return false;
    }

    ImVec2 calc_text_item_size(int itemIdx, const c_drag_item& item) const {
        ImFont* font = ImGui::GetFont();
        float fontSize = font->FontSize;

        if (itemIdx == 1 && FWork::Fonts::IconWeapon) {
            font = FWork::Fonts::IconWeapon;
            fontSize = esp_preview_layout::kWeaponIconFontSize;
        }
        else if ((itemIdx == 2 || itemIdx == 7) && FWork::Fonts::InterBold) {
            font = FWork::Fonts::InterBold;
            fontSize = esp_preview_layout::kLabelFontSize;
        }
        else if (itemIdx == 3 && FWork::Fonts::InterBold) {
            font = FWork::Fonts::InterBold;
            fontSize = esp_preview_layout::kLabelFontSize;
        }

        ImVec2 size = font->CalcTextSizeA(fontSize, FLT_MAX, 0.f, item.text.c_str());
        if (item.small_text)
            size.y = 12.f;
        return size;
    }

    void get_text_font(int itemIdx, ImFont*& font, float& fontSize) const {
        font = ImGui::GetFont();
        fontSize = font->FontSize;

        if (itemIdx == 1 && FWork::Fonts::IconWeapon) {
            font = FWork::Fonts::IconWeapon;
            fontSize = esp_preview_layout::kWeaponIconFontSize;
        }
        else if ((itemIdx == 2 || itemIdx == 3 || itemIdx == 7) && FWork::Fonts::InterBold) {
            font = FWork::Fonts::InterBold;
            fontSize = esp_preview_layout::kLabelFontSize;
        }
    }

    int find_closest_position(ImVec2 curr, Position positions[]) {
        float closest = FLT_MAX;
        int best = -1;
        for (int i = 0; i < 4; i++) {
            const ImVec2& p = positions[i].pos;
            const float dist = esp_drag_detail::vec2_dist(p, curr);
            if (closest > dist) {
                closest = dist;
                best = i;
            }
        }
        return best;
    }

    void sync_layout_to_globals() {
        if (m_items[0].pos >= 0 && m_items[0].pos <= 3) {
            const int p = m_items[0].pos;
            g_Globals.Visuals.players_healthbar = (p == 2) ? 3 : (p == 3) ? 2 : p;
            g_Globals.Visuals.HealthBarPosition = (p == 0) ? 3 : (p == 1) ? 2 : (p == 2) ? 0 : 1;
        }
        if (m_items[2].pos >= 0 && m_items[2].pos <= 3)
            g_Globals.Visuals.EspNameSide = m_items[2].pos;
        if (m_items[3].pos >= 0 && m_items[3].pos <= 3)
            g_Globals.Visuals.EspDistanceSide = m_items[3].pos;
        if (m_items[1].pos >= 0 && m_items[1].pos <= 3) {
            g_Globals.Visuals.EspWeaponIconSide = m_items[1].pos;
            g_Globals.Visuals.WeaponInfo = (m_items[1].pos == 0) ? 2 : (m_items[1].pos == 1) ? 3 : (m_items[1].pos == 2) ? 0 : 1;
        }
        if (m_items[7].pos >= 0 && m_items[7].pos <= 3) {
            g_Globals.Visuals.EspWeaponTextSide = m_items[7].pos;
            g_Globals.Visuals.WeaponInfo = (m_items[7].pos == 0) ? 2 : (m_items[7].pos == 1) ? 3 : (m_items[7].pos == 2) ? 0 : 1;
        }
    }

    void layout_box_from_window() {
        const ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
        const ImVec2 contentMax = ImGui::GetWindowContentRegionMax();
        const float contentW = contentMax.x - contentMin.x;
        const float contentH = contentMax.y - contentMin.y;
        box.w = esp_preview_layout::kBoxW;
        box.h = esp_preview_layout::kBoxH;
        box.x = (int)(contentMin.x + (contentW - (float)box.w) * 0.5f);
        box.y = (int)(contentMin.y + (contentH - (float)box.h) * 0.5f);
        if (box.x < 0) box.x = 0;
        if (box.y < 0) box.y = 0;
    }

    void set_positions() {
        m_items[4].enabled = false;
        m_items[5].enabled = false;
        m_items[6].enabled = false;

        layout_box_from_window();

        Position Positions[] = {
            {ImVec2(ImGui::GetWindowPos().x + box.x - 5, ImGui::GetWindowPos().y + box.y)},
            {ImVec2(ImGui::GetWindowPos().x + box.x + box.w + 2, ImGui::GetWindowPos().y + box.y)},
            {ImVec2(ImGui::GetWindowPos().x + box.x, ImGui::GetWindowPos().y + box.y - 5)},
            {ImVec2(ImGui::GetWindowPos().x + box.x, ImGui::GetWindowPos().y + box.y + box.h + 2)},
        };

        const ImVec2 barSize(2.f + (float)esp_scale - 15.f, (float)box.h);

        for (int i = 0; i < (int)m_items.size(); i++) {
            auto& item = m_items[i];
            if (i >= 4 && i <= 6)
                continue;

            if (item.pos != 4)
                item.pos = clamp_side_pos(item.pos);

            if (item.type == 0)
                item.size = calc_text_item_size(i, item);
            else if (item.size.x <= 0.f || item.size.y <= 0.f)
                item.size = barSize;

            ImGui::PushID(i);
            ImGui::SetCursorScreenPos(item.pos_);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.f);
            ImGui::Button(("#esp_drag_" + std::to_string(i)).c_str(), item.size);
            const bool hovered = ImGui::IsItemHovered();
            ImGui::PopStyleVar();

            const int dropPos = find_closest_position(ImGui::GetMousePos(), Positions);
            item.hovered = false;
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoPreviewTooltip)) {
                ImGui::SetDragDropPayload("#esp_preview_drag", &i, sizeof(int), 0);
                for (int t = 0; t < (int)m_items.size(); t++)
                    m_items[t].move_animation = ImGui::GetIO().DeltaTime * 34.f;

                item.pos = 4;
                item.think_pos = dropPos;
                item.helding = dropPos > 1;
                item.hovered = true;
                ImGui::EndDragDropSource();
            }
            else if (item.pos == 4) {
                item.pos = clamp_side_pos(item.think_pos);
                item.think_pos = -1;
                item.move_animation = 0.f;
            }

            item.animations[0] = ImLerp(item.animations[0], item.hovered || hovered ? 1.f : 0.f, ImGui::GetIO().DeltaTime * 34.f);
            ImGui::GetWindowDrawList()->AddRect(
                item.pos_ - ImVec2(1, 1),
                item.pos_ + item.size + ImVec2(1, 1),
                ImColor(255, 255, 255, (int)(255 * item.animations[0])));
            ImGui::PopID();
        }

        if (!m_layoutLoadedFromGlobals) {
            m_layoutLoadedFromGlobals = true;
            const int h = g_Globals.Visuals.players_healthbar;
            m_items[0].pos = clamp_side_pos((h == 2) ? 3 : (h == 3) ? 2 : h);
            m_items[2].pos = std::clamp(g_Globals.Visuals.EspNameSide, 0, 3);
            m_items[3].pos = std::clamp(g_Globals.Visuals.EspDistanceSide, 0, 3);
            m_items[1].pos = std::clamp(g_Globals.Visuals.EspWeaponIconSide, 0, 3);
            m_items[7].pos = std::clamp(g_Globals.Visuals.EspWeaponTextSide, 0, 3);
            for (auto& item : m_items)
                item.move_animation = 1.f;
        }

        sync_layout_to_globals();
    }

    void draw_box_preview(float bx, float by, float alpha) {
        const float dt = ImGui::GetIO().DeltaTime * 34.f;
        m_boxAnim = ImLerp(m_boxAnim, g_Globals.Visuals.Box ? 1.f : 0.f, dt);
        m_fillAnim = ImLerp(m_fillAnim, (g_Globals.Visuals.Box && g_Globals.Visuals.fillBox) ? 1.f : 0.f, dt);
        if (m_boxAnim < 0.01f && m_fillAnim < 0.01f)
            return;

        const float drawAlpha = alpha * ImMax(m_boxAnim, m_fillAnim);
        const ImU32 boxCol = esp_preview::box_color_u32(drawAlpha);
        const ImU32 outlineCol = IM_COL32(0, 0, 0, (int)(255 * drawAlpha));
        const float th = 1.0f;
        ImDrawList* dl = ImGui::GetWindowDrawList();

        if (m_fillAnim > 0.01f) {
            const ImVec4& fc = g_Globals.Visuals.fillBoxColor.Value;
            const ImU32 fillCol = IM_COL32(
                (int)(fc.x * 255.f), (int)(fc.y * 255.f), (int)(fc.z * 255.f),
                (int)(fc.w * alpha * m_fillAnim * 255.f));
            dl->AddRectFilled(ImVec2(bx, by), ImVec2(bx + box.w, by + box.h), fillCol);
        }

        if (m_boxAnim < 0.01f)
            return;

        if (g_Globals.Visuals.players_box == 1) {
            const float cw = (float)box.w / 3.0f;
            const float ch = (float)box.h / 3.0f;
            dl->AddLine(ImVec2(bx, by), ImVec2(bx + cw, by), outlineCol, th + 1.0f);
            dl->AddLine(ImVec2(bx, by), ImVec2(bx, by + ch), outlineCol, th + 1.0f);
            dl->AddLine(ImVec2(bx, by), ImVec2(bx + cw, by), boxCol, th);
            dl->AddLine(ImVec2(bx, by), ImVec2(bx, by + ch), boxCol, th);
            dl->AddLine(ImVec2(bx + box.w, by), ImVec2(bx + box.w - cw, by), outlineCol, th + 1.0f);
            dl->AddLine(ImVec2(bx + box.w, by), ImVec2(bx + box.w, by + ch), outlineCol, th + 1.0f);
            dl->AddLine(ImVec2(bx + box.w, by), ImVec2(bx + box.w - cw, by), boxCol, th);
            dl->AddLine(ImVec2(bx + box.w, by), ImVec2(bx + box.w, by + ch), boxCol, th);
            dl->AddLine(ImVec2(bx, by + box.h), ImVec2(bx + cw, by + box.h), outlineCol, th + 1.0f);
            dl->AddLine(ImVec2(bx, by + box.h), ImVec2(bx, by + box.h - ch), outlineCol, th + 1.0f);
            dl->AddLine(ImVec2(bx, by + box.h), ImVec2(bx + cw, by + box.h), boxCol, th);
            dl->AddLine(ImVec2(bx, by + box.h), ImVec2(bx, by + box.h - ch), boxCol, th);
            dl->AddLine(ImVec2(bx + box.w, by + box.h), ImVec2(bx + box.w - cw, by + box.h), outlineCol, th + 1.0f);
            dl->AddLine(ImVec2(bx + box.w, by + box.h), ImVec2(bx + box.w, by + box.h - ch), outlineCol, th + 1.0f);
            dl->AddLine(ImVec2(bx + box.w, by + box.h), ImVec2(bx + box.w - cw, by + box.h), boxCol, th);
            dl->AddLine(ImVec2(bx + box.w, by + box.h), ImVec2(bx + box.w, by + box.h - ch), boxCol, th);
        }
        else {
            dl->AddRect(ImVec2(bx, by), ImVec2(bx + box.w, by + box.h), boxCol, 0.0f, 0, th);
            dl->AddRect(ImVec2(bx - 1, by - 1), ImVec2(bx + box.w + 1, by + box.h + 1), outlineCol);
            dl->AddRect(ImVec2(bx + 1, by + 1), ImVec2(bx + box.w - 1, by + box.h - 1), outlineCol);
        }
    }

    void draw_skeleton_preview(float bx, float by, float alpha) {
        const float dt = ImGui::GetIO().DeltaTime * 34.f;
        m_skeletonAnim = ImLerp(m_skeletonAnim, g_Globals.Visuals.Skeleton ? 1.f : 0.f, dt);
        if (m_skeletonAnim < 0.01f)
            return;

        const float skAlpha = alpha * m_skeletonAnim;
        const float bw = (float)box.w;
        const float bh = (float)box.h;
        const ImVec2 head(bx + bw * 0.5f, by + bh * 0.06f);
        const ImVec2 neck(bx + bw * 0.5f, by + bh * 0.16f);
        const ImVec2 l_sh(bx + bw * 0.20f, by + bh * 0.20f);
        const ImVec2 r_sh(bx + bw * 0.80f, by + bh * 0.20f);
        const ImVec2 l_el(bx + bw * 0.10f, by + bh * 0.34f);
        const ImVec2 r_el(bx + bw * 0.90f, by + bh * 0.34f);
        const ImVec2 l_wr(bx + bw * 0.04f, by + bh * 0.48f);
        const ImVec2 r_wr(bx + bw * 0.96f, by + bh * 0.48f);
        const ImVec2 l_hand(bx + bw * 0.01f, by + bh * 0.50f);
        const ImVec2 r_hand(bx + bw * 0.99f, by + bh * 0.50f);
        const ImVec2 hip(bx + bw * 0.5f, by + bh * 0.54f);
        const ImVec2 l_ank(bx + bw * 0.38f, by + bh * 0.82f);
        const ImVec2 r_ank(bx + bw * 0.62f, by + bh * 0.82f);
        const ImVec2 l_foot(bx + bw * 0.34f, by + bh * 0.97f);
        const ImVec2 r_foot(bx + bw * 0.66f, by + bh * 0.97f);

        const ImU32 skCol = esp_preview::skeleton_color_u32(skAlpha);
        ImDrawList* dl = ImGui::GetWindowDrawList();

        const float glowRadius = 14.0f;
        const float feather = 2.0f;
        auto DrawBoneGlow = [&](const ImVec2& from, const ImVec2& to) {
            for (float i = glowRadius; i > 0; i -= feather) {
                const int a = (int)(255 * skAlpha * (i / glowRadius) * 0.12f);
                dl->AddLine(from, to, IM_COL32(255, 255, 255, a), 1.5f + i * 0.5f);
            }
            dl->AddLine(from, to, skCol, 1.5f);
        };

        for (float i = glowRadius; i > 0; i -= feather) {
            const int a = (int)(255 * skAlpha * (i / glowRadius) * 0.12f);
            dl->AddCircle(head, 4.0f + i * 0.5f, IM_COL32(255, 255, 255, a), 0, 1.0f + i * 0.5f);
        }
        dl->AddCircle(head, 4.0f, skCol, 0, 1.0f);

        DrawBoneGlow(head, neck);
        DrawBoneGlow(neck, l_sh);
        DrawBoneGlow(neck, r_sh);
        DrawBoneGlow(l_sh, l_el);
        DrawBoneGlow(r_sh, r_el);
        DrawBoneGlow(l_el, l_wr);
        DrawBoneGlow(r_el, r_wr);
        DrawBoneGlow(l_wr, l_hand);
        DrawBoneGlow(r_wr, r_hand);
        DrawBoneGlow(neck, hip);
        DrawBoneGlow(hip, l_ank);
        DrawBoneGlow(hip, r_ank);
        DrawBoneGlow(l_ank, l_foot);
        DrawBoneGlow(r_ank, r_foot);
    }

    void draw_text_item(c_drag_item& item, int itemIdx, const ImVec2& size, Position Positions[], ImVec2 Sizes[]) {
        ImFont* textFont = nullptr;
        float fontSize = 0.f;
        get_text_font(itemIdx, textFont, fontSize);

        const float alpha = item.animations[2] * ImGui::GetStyle().Alpha;
        const ImU32 outlineCol = IM_COL32(0, 0, 0, (int)(255.f * alpha));
        const ImU32 textCol = esp_drag_detail::color_alpha(item.col, alpha);
        auto drawLabel = [&](const ImVec2& pos) {
            ImGui::GetWindowDrawList()->AddText(textFont, fontSize, pos - ImVec2(1, 0), outlineCol, item.text.c_str());
            ImGui::GetWindowDrawList()->AddText(textFont, fontSize, pos + ImVec2(0, 1), outlineCol, item.text.c_str());
            ImGui::GetWindowDrawList()->AddText(textFont, fontSize, pos - ImVec2(0, 1), outlineCol, item.text.c_str());
            ImGui::GetWindowDrawList()->AddText(textFont, fontSize, pos + ImVec2(1, 0), outlineCol, item.text.c_str());
            ImGui::GetWindowDrawList()->AddText(textFont, fontSize, pos, textCol, item.text.c_str());
        };

        switch (item.pos) {
        case 0:
            item.pos_ = ImLerp(item.pos_, Positions[0].pos + ImVec2(-m_offsets[0] - size.x, m_offsets[4]), item.move_animation);
            drawLabel(item.pos_);
            m_offsets[4] += 2.f + size.y;
            break;
        case 1:
            item.pos_ = ImLerp(item.pos_, Positions[1].pos + ImVec2(m_offsets[1] + esp_scale - 15, m_offsets[5]), item.move_animation);
            drawLabel(item.pos_);
            m_offsets[5] += size.y;
            break;
        case 2:
            item.pos_ = ImLerp(item.pos_, Positions[2].pos + ImVec2(Sizes[2].x / 2.f - size.x / 2.f, -m_offsets[2] - size.y - m_offsets[6]), item.move_animation);
            drawLabel(item.pos_);
            m_offsets[6] += 2.f + size.y;
            break;
        case 3:
            item.pos_ = ImLerp(item.pos_, Positions[3].pos + ImVec2(Sizes[2].x / 2.f - size.x / 2.f, m_offsets[3] + m_offsets[7]), item.move_animation);
            drawLabel(item.pos_);
            m_offsets[7] += 2.f + size.y;
            break;
        case 4:
            item.pos_ = ImLerp(item.pos_, ImGui::GetMousePos() + ImVec2(-size.x / 2.f, 0), ImGui::GetIO().DeltaTime * 14.f);
            drawLabel(item.pos_);
            break;
        default:
            break;
        }
    }

    void draw_bar_item(c_drag_item& item, Position Positions[], ImVec2 Sizes[]) {
        const float alpha = item.animations[2] * ImGui::GetStyle().Alpha;

        switch (item.pos) {
        case 0:
            item.size = Sizes[0];
            item.pos_ = ImLerp(item.pos_, Positions[0].pos + ImVec2(-m_offsets[0] + 15 - esp_scale, 0.f), item.move_animation);
            ImGui::GetWindowDrawList()->AddRectFilled(item.pos_, item.pos_ + Sizes[0], esp_drag_detail::color_alpha(item.col, alpha));
            m_offsets[0] += 5.f + esp_scale - 15;
            break;
        case 1:
            item.size = Sizes[1];
            item.pos_ = ImLerp(item.pos_, Positions[1].pos + ImVec2(m_offsets[1] + esp_scale - 15, 0.f), item.move_animation);
            ImGui::GetWindowDrawList()->AddRectFilled(item.pos_, item.pos_ + Sizes[1], esp_drag_detail::color_alpha(item.col, alpha));
            m_offsets[1] += 5.f + esp_scale - 15;
            break;
        case 2:
            item.size = Sizes[2];
            item.pos_ = ImLerp(item.pos_, Positions[2].pos + ImVec2(0.f, -m_offsets[2] + 15 - esp_scale), item.move_animation);
            ImGui::GetWindowDrawList()->AddRectFilled(item.pos_, item.pos_ + Sizes[2], esp_drag_detail::color_alpha(item.col, alpha));
            m_offsets[2] += 5.f + esp_scale - 15;
            break;
        case 3:
            item.size = Sizes[3];
            item.pos_ = ImLerp(item.pos_, Positions[3].pos + ImVec2(0.f, m_offsets[3] + esp_scale - 15), item.move_animation);
            ImGui::GetWindowDrawList()->AddRectFilled(item.pos_, item.pos_ + Sizes[3], esp_drag_detail::color_alpha(item.col, alpha));
            m_offsets[3] += 5.f + esp_scale - 15;
            break;
        case 4:
            item.pos_ = ImLerp(item.pos_, ImGui::GetMousePos() + ImVec2(0.f, m_offsets[3]), ImGui::GetIO().DeltaTime * 34.f);
            if (item.helding == 1) {
                item.size = Sizes[3];
                ImGui::GetWindowDrawList()->AddRectFilled(item.pos_, item.pos_ + Sizes[3], esp_drag_detail::color_alpha(item.col, alpha));
            }
            else {
                item.size = Sizes[1];
                ImGui::GetWindowDrawList()->AddRectFilled(item.pos_, item.pos_ + Sizes[1], esp_drag_detail::color_alpha(item.col, alpha));
            }
            break;
        default:
            break;
        }
    }

    void on_draw() {
        layout_box_from_window();

        Position Positions[] = {
            {ImVec2(ImGui::GetWindowPos().x + box.x - 5, ImGui::GetWindowPos().y + box.y)},
            {ImVec2(ImGui::GetWindowPos().x + box.x + box.w + 2, ImGui::GetWindowPos().y + box.y)},
            {ImVec2(ImGui::GetWindowPos().x + box.x, ImGui::GetWindowPos().y + box.y - 5)},
            {ImVec2(ImGui::GetWindowPos().x + box.x, ImGui::GetWindowPos().y + box.y + box.h + 2)},
        };

        ImVec2 Sizes[] = {
            ImVec2(2 + esp_scale - 15, box.h),
            ImVec2(2 + esp_scale - 15, box.h),
            ImVec2(box.w, 2 + esp_scale - 15),
            ImVec2(box.w, 2 + esp_scale - 15)
        };

        const float bx = ImGui::GetWindowPos().x + (float)box.x;
        const float by = ImGui::GetWindowPos().y + (float)box.y;
        const float alpha = ImGui::GetStyle().Alpha;

        draw_box_preview(bx, by, alpha);
        draw_skeleton_preview(bx, by, alpha);

        float offsetY = 0.f;

        for (int itemIdx = 0; itemIdx < (int)m_items.size(); ++itemIdx) {
            auto& item = m_items[itemIdx];
            if (itemIdx >= 4 && itemIdx <= 6)
                continue;

            const bool show = preview_item_visible(itemIdx);
            const bool wasVisible = m_itemWasEnabled[itemIdx];
            m_itemWasEnabled[itemIdx] = show;

            item.animations[2] = ImLerp(item.animations[2], show ? 1.f : 0.f, ImGui::GetIO().DeltaTime * 34.f);
            if (item.animations[2] < 0.01f)
                continue;

            if (show && !wasVisible)
                item.move_animation = 0.f;

            item.move_animation += ImGui::GetIO().DeltaTime * 34.f;
            item.move_animation = ImClamp(item.move_animation, 0.f, 1.f);

            if (item.hovered) {
                ImVec2 size = (item.type == 0)
                    ? calc_text_item_size(itemIdx, item)
                    : ImGui::CalcTextSize(item.text.c_str());

                switch (item.think_pos) {
                case 0: item.type == 0 ? m_offsets[4] += 2.f + size.y + offsetY : m_offsets[0] += 5.f; break;
                case 1: item.type == 0 ? m_offsets[5] += 2.f + size.y + offsetY : m_offsets[1] += 5.f; break;
                case 2: item.type == 0 ? m_offsets[6] += 2.f + size.y + offsetY : m_offsets[2] += 5.f; break;
                case 3: item.type == 0 ? m_offsets[7] += 2.f + size.y + offsetY : m_offsets[3] += 5.f; break;
                }
                offsetY += size.y;
            }

            if (item.type == 0) {
                ImVec2 size = calc_text_item_size(itemIdx, item);
                item.size = size;
                draw_text_item(item, itemIdx, size, Positions, Sizes);
                continue;
            }

            draw_bar_item(item, Positions, Sizes);
        }

        for (int i = 0; i < 8; i++)
            m_offsets[i] = 0.f;
    }
} inline m_esp_draw;
