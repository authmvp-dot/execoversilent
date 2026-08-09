#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

#include "custom_popup.hpp"

#include <string>
#include <wtypes.h>

#include <algorithm> // Для std::max_element
#include <vector>

#include "custom_popup.hpp"
#include <iostream>
#include <Windows.h>

#include <imgui_internal.h>
#include "notify.h"
#include "imstb_textedit.h"	

#include <cstdlib>
#include <imgui.h>

#include <vector>
#include <map>

using namespace ImGui;


#include <map>
#include <string>
#include <imgui.h>

namespace particle
{
	const int PARTICLE_COUNT = 35;
	void				RenderEffects(ImDrawList* drawList, ImVec2 renderSize, float timeOffset);
	void				AddParticle(ImVec2 origin, ImVec2 size, float flag);

}


class roll_text_anim
{
private:
	struct anim_state
	{
		float previous_value = 0.f;
		float global_offset = 0.f;
		std::vector<std::string> texts;
	};

	std::map<ImGuiID, anim_state> states; // Храним состояния для каждого элемента

public:
	const char* render_text(ImRect bb, float value, ImGuiID id)
	{
		auto& state = states[id]; // Получаем состояние для текущего элемента

		// Инициализация текстов, если состояние еще не создано
		if (state.texts.empty())
		{
			for (int i = 0; i < 7; i++)
			{
				state.texts.push_back("0");
			}
		}

		ImVec2 one_text_size = ImGui::CalcTextSize("0") + ImVec2(0, 4);

		// Плавное изменение global_offset для анимации
		float target_offset = (state.previous_value - value) * one_text_size.y * 2.0f;
		state.global_offset = ImLerp(state.global_offset, target_offset, ImGui::GetIO().DeltaTime * 3.f);
		state.previous_value = value;

		// Рендерим только 7 текстов
		for (int i = -3; i <= 3; i++)
		{
			int text_value = static_cast<int>(value) + i;
			if (text_value < 0) text_value += 10;
			if (text_value > 9) text_value -= 10;

			float text_y_offset = state.global_offset + (one_text_size.y * i);
			if (text_y_offset < -one_text_size.y * 3) text_y_offset += one_text_size.y * 7;
			if (text_y_offset > one_text_size.y * 3) text_y_offset -= one_text_size.y * 7;

			ImGui::PushClipRect(bb.Min, bb.Max, true);
			ImGui::GetWindowDrawList()->AddText(
				bb.GetCenter() - ImGui::CalcTextSize(std::to_string(text_value).c_str()) / 2 - ImVec2(0, text_y_offset),
				ImColor(1.f, 1.f, 1.f, 1.f),
				std::to_string(text_value).c_str()
			);
			ImGui::PopClipRect();
		}

		return std::to_string(static_cast<int>(value)).c_str();
	}
};


namespace custom
{
	extern bool GlobalMute;
	inline const char* keys[] =
	{
		"-",
		"Mouse 1",
		"Mouse 2",
		"CN",
		"Mouse 3",
		"Mouse 4",
		"Mouse 5",
		"-",
		"Back",
		"Tab",
		"-",
		"-",
		"CLR",
		"Enter",
		"-",
		"-",
		"Shift",
		"CTL",
		"Menu",
		"Pause",
		"Caps Lock",
		"KAN",
		"-",
		"JUN",
		"FIN",
		"KAN",
		"-",
		"Escape",
		"CON",
		"NCO",
		"ACC",
		"MAD",
		"Space",
		"PGU",
		"PGD",
		"End",
		"Home",
		"Left",
		"Up",
		"Right",
		"Down",
		"SEL",
		"PRI",
		"EXE",
		"PRI",
		"INS",
		"Delete",
		"HEL",
		"0",
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		"-",
		"-",
		"-",
		"-",
		"-",
		"-",
		"-",
		"A",
		"B",
		"C",
		"D",
		"E",
		"F",
		"G",
		"H",
		"I",
		"J",
		"K",
		"L",
		"M",
		"N",
		"O",
		"P",
		"Q",
		"R",
		"S",
		"T",
		"U",
		"V",
		"W",
		"X",
		"Y",
		"Z",
		"WIN",
		"WIN",
		"APP",
		"-",
		"SLE",
		"Numpad 0",
		"Numpad 1",
		"Numpad 2",
		"Numpad 3",
		"Numpad 4",
		"Numpad 5",
		"Numpad 6",
		"Numpad 7",
		"Numpad 8",
		"Numpad 9",
		"MUL",
		"ADD",
		"SEP",
		"MIN",
		"Delete",
		"DIV",
		"F1",
		"F2",
		"F3",
		"F4",
		"F5",
		"F6",
		"F7",
		"F8",
		"F9",
		"F10",
		"F11",
		"F12",
		"F13",
		"F14",
		"F15",
		"F16",
		"F17",
		"F18",
		"F19",
		"F20",
		"F21",
		"F22",
		"F23",
		"F24",
		"-",
		"-",
		"-",
		"-",
		"-",
		"-",
		"-",
		"-",
		"NUM",
		"SCR",
		"EQU",
		"MAS",
		"TOY",
		"OYA",
		"OYA",
		"-",
		"-",
		"-",
		"-",
		"-",
		"-",
		"-",
		"-",
		"-",
		"Shift",
		"Shift",
		"Ctrl",
		"Ctrl",
		"Alt",
		"Alt"
	};


	inline void ShadeVertsAnimSine(ImDrawList* draw_list, int vert_start_idx, int vert_end_idx, float phase, float speed, float amplitude)
	{
		ImDrawVert* vert_start = draw_list->VtxBuffer.Data + vert_start_idx;
		ImDrawVert* vert_end = draw_list->VtxBuffer.Data + vert_end_idx;

		for (ImDrawVert* vertex = vert_start; vertex < vert_end; ++vertex)
			vertex->pos.y += sinf(vertex->pos.x * speed + phase) * amplitude;
	}

	static class chroma
	{
	public:

		inline static std::vector<ImColor> colors = {
		ImColor(1.0f, 0.0f, 0.0f),  // Red
		ImColor(1.0f, 0.0f, 1.0f),  // Pink
		ImColor(0.0f, 0.0f, 1.0f),  // Blue
		ImColor(0.0f, 1.0f, 1.0f),  // Cyan
		ImColor(0.0f, 1.0f, 0.0f),  // Green
		ImColor(1.0f, 1.0f, 0.0f),  // Yellow
		ImColor(1.0f, 0.5f, 0.0f),  // Orange
		ImColor(1.0f, 0.0f, 0.0f)   // Back to Red
		};

	private:

		struct gradient_info
		{
			float shadow_delta;
		};

		inline static std::vector<ImColor> rgb_colors = {
			ImColor(1.0f, 0.0f, 0.0f),  // Red
			ImColor(1.0f, 0.0f, 1.0f),  // Pink
			ImColor(0.0f, 0.0f, 1.0f),  // Blue
			ImColor(0.0f, 1.0f, 1.0f),  // Cyan
			ImColor(0.0f, 1.0f, 0.0f),  // Green
			ImColor(1.0f, 1.0f, 0.0f),  // Yellow
			ImColor(1.0f, 0.5f, 0.0f),  // Orange
			ImColor(1.0f, 0.0f, 0.0f)   // Back to Red
		};


		inline static int text_idx;
		const float changing_speed = 0.15f;
		inline static std::vector<ImColor> part_color = { c::main_color, c::accent_color };

		ImColor GetColorWithAlpha(ImColor color, float alpha)
		{
			return ImColor(color.Value.x, color.Value.y, color.Value.z, alpha);
		}

		ImColor GetDarkColor(const ImColor& color)
		{
			float darkPercentage = 0.35f;
			return ImColor(
				color.Value.x * darkPercentage,
				color.Value.y * darkPercentage,
				color.Value.z * darkPercentage,
				1.f // Сохраняем исходный альфа-канал
			);
		}

		void GradientRect(ImRect bb, ImColor c1, ImColor c2, float rounding, bool filled, float thinkess, const char* str_id, float shadow_thinkess, ImDrawFlags flags)
		{
			part_color = { c::main_color, c::accent_color };

			ImGuiWindow* window = ImGui::GetCurrentWindow();
			const ImGuiID id = window->GetID(str_id);


			static std::map<ImGuiID, gradient_info> anim;
			auto it_anim = anim.emplace(id, gradient_info()).first;

			it_anim->second.shadow_delta = ImLerp(it_anim->second.shadow_delta, bb.Contains(ImGui::GetMousePos()) ? 1.f : 0.f, ImGui::GetIO().DeltaTime * 22.f);

			const int vtx_idx_0 = ImGui::GetWindowDrawList()->VtxBuffer.Size;

			if (!filled)
				ImGui::GetWindowDrawList()->AddRect(bb.Min, bb.Max, ImColor(1.f, 1.f, 1.f, 1.f), rounding, 0, thinkess);
			else
			{
				if (str_id != "scroll") {
					ImGui::GetWindowDrawList()->AddShadowRect(bb.Min + ImVec2(1, 1), bb.Max - ImVec2(1, 1), ImColor(1.f, 1.f, 1.f, (thinkess / 2) * shadow_thinkess), 10.f + 55.f * shadow_thinkess, ImVec2(0, 0), flags, rounding);
					ImGui::GetWindowDrawList()->AddRectFilled(bb.Min, bb.Max, ImColor(1.f, 1.f, 1.f, thinkess), rounding, flags);
					//ImGui::GetWindowDrawList()->AddShadowCircle(ImClamp(ImGui::GetMousePos(), bb.Min, bb.Max), 3.f + (5.f * it_anim->second.shadow_delta), ImColor(1.f, 1.f, 1.f, 0.5f * it_anim->second.shadow_delta), 15.f + 65.f * it_anim->second.shadow_delta, ImVec2(0, 0), flags, 36.f);
				}
				else
				{
					window->DrawList->AddShadowRect(bb.Min + ImVec2(1, 1), bb.Max - ImVec2(1, 1), ImColor(1.f, 1.f, 1.f, (thinkess / 2) * shadow_thinkess), 10.f + 55.f * shadow_thinkess, ImVec2(0, 0), flags, rounding);
					window->DrawList->AddRectFilled(bb.Min, bb.Max, ImColor(1.f, 1.f, 1.f, thinkess), rounding, flags);
				}
			}
			const int vtx_idx_1 = ImGui::GetWindowDrawList()->VtxBuffer.Size;
			ImGui::ShadeVertsLinearColorGradientKeepAlpha(ImGui::GetWindowDrawList(), vtx_idx_0, vtx_idx_1, bb.Min, bb.Max,
				GetColorWithAlpha(part_color[0], thinkess), GetColorWithAlpha(part_color[1], thinkess));


		}


	public:

		// Render Text with Gradient
		void RenderText(ImVec2 pos, const char* text, float size, float alpha = 1.0f, ImDrawList* draw_list = ImGui::GetWindowDrawList()) {
			const int vtx_idx_2 = draw_list->VtxBuffer.Size;
			draw_list->AddText(pos, ImColor(1.f, 1.f, 1.f, alpha), text);
			const int vtx_idx_3 = draw_list->VtxBuffer.Size;
			ImGui::ShadeVertsLinearColorGradientKeepAlpha(draw_list, vtx_idx_2, vtx_idx_3, pos + ImVec2(CalcTextSize(text).x / 2, 0), pos + CalcTextSize(text), part_color[0], part_color[1]);

		}

		void RenderRect(ImVec2 p1, ImVec2 p2, float rounding, bool filled = false, float thickness = 2.f, const char* str_id = "", float shadow_thinkess = 0.f, ImDrawFlags flags = ImDrawFlags_RoundCornersAll) {
			GradientRect(ImRect(p1, p2), part_color[0], part_color[1], rounding, filled, thickness, str_id, shadow_thinkess, flags);
		}
	};

	bool				Tab(const char* label, const char* icon, int* v, int number);

	bool				ThemeButton(const char* id_theme, bool dark, const ImVec2& size_arg);
	bool				Button(const char* label, const ImVec2& size_arg);

	bool				ChildEx(const char* name, const char* icon, const char* description, ImGuiID id, const ImVec2& size_arg, bool cap = false, ImGuiWindowFlags flags = 0);
	bool				Child(const char* str_id, const char* icon, const char* description, const ImVec2& size = ImVec2(0, 0), bool cap = false, ImGuiWindowFlags flags = 0);
	bool				ChildID(ImGuiID id, const char* icon, const char* description, const ImVec2& size = ImVec2(0, 0), bool cap = false, ImGuiWindowFlags flags = 0);
	void				EndChild();

	void				BeginGroup();
	void				EndGroup();

	bool				SubTab(const char* label, int* v, int number);
	bool				Checkbox(const char* label, bool* v);
	bool				KeyYorzen(const char* label, bool* v);
	bool				CheckboxClicked(const char* label, bool* v);

	bool			    Selectable(const char* label, bool selected = false, ImGuiSelectableFlags flags = 0, const ImVec2& size = ImVec2(0, 0));
	bool				Selectable(const char* label, bool* p_selected, ImGuiSelectableFlags flags = 0, const ImVec2& size = ImVec2(0, 0));

	bool				BeginCombo(const char* label, const char* preview_value, int val = 0, bool multi = false, ImGuiComboFlags flags = 0);
	void				EndCombo();
	void				MultiCombo(const char* label, bool variable[], const char* labels[], int count);
	bool				Combo(const char* label, int* current_item, const char* const items[], int items_count, int popup_max_height_in_items = -1);
	bool				Combo(const char* label, int* current_item, const char* items_separated_by_zeros, int popup_max_height_in_items = -1);
	bool				Combo(const char* label, int* current_item, const char* (*getter)(void* user_data, int idx), void* user_data, int items_count, int popup_max_height_in_items = -1);

	bool				IsComboPopupOpen();

	bool				ColorButton(const char* desc_id, const ImVec4& col, ImGuiColorEditFlags flags = 0, const ImVec2& size = ImVec2(0, 0));
	bool				ColorEdit4(const char* label, const char* description, float col[4], ImGuiColorEditFlags flags = 0);
	bool			    ColorPicker4(const char* label, float col[4], ImGuiColorEditFlags flags = 0, const float* ref_col = NULL);
	bool				ColorSlider(const char* label, float col[4], ImGuiColorEditFlags flags = 0);
	bool				ColorEditor(const char* label, ImColor* col, ImGuiColorEditFlags flags = 0);

	bool				KnobScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags = 0);
	bool				KnobFloat(const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags = 0);
	bool				KnobInt(const char* label, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags = 0);

	bool				SliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags = 0);
	bool				SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
	bool				SliderInt(const char* label, int* v, int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);

	bool				Keybind(const char* label, int* key, int* mode);
	const char*         KeyName(int vk);

	void				Separator_line();

	void				SeparatorEx(ImGuiSeparatorFlags flags, float thickness);
	void				Separator();

	bool ImConfig(const char* label, const char* date, const char* time, const char* creator);
}
