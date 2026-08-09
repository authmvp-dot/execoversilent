#pragma once

#include <example_win32_directx11/ImGui/custom_widgets.hpp>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_settings.h>

#include <cstring>
#include <functional>
#include <map>
#include <string>
#include <vector>

// Shared menu UI helpers (safe to include from multiple TUs).
namespace edited {

inline bool Checkbox(const char* label, const char* tooltip, bool* v, std::function<void()> cb = nullptr)
{
	(void)tooltip;
	bool changed = custom::Checkbox(label, v);
	if (*v) {
		ImGui::Indent(10.0f);
		ImGui::PushID(label);
		if (cb) cb();
		ImGui::PopID();
		ImGui::Unindent(10.0f);
	}
	return changed;
}

inline bool SliderFloat(const char* label, const char* tooltip, float* v, float v_min, float v_max, const char* format = "%.3f")
{
	(void)tooltip;
	return custom::SliderFloat(label, v, v_min, v_max, format);
}

inline bool SliderInt(const char* label, const char* tooltip, int* v, int v_min, int v_max, const char* format = "%d")
{
	(void)tooltip;
	return custom::SliderInt(label, v, v_min, v_max, format);
}

inline bool Combo(const char* label, const char* tooltip, int* current_item, const char* items_separated_by_zeros)
{
	(void)tooltip;
	std::vector<const char*> items;
	const char* p = items_separated_by_zeros;
	while (*p) {
		items.push_back(p);
		p += strlen(p) + 1;
	}
	return custom::Combo(label, current_item, items.data(), (int)items.size());
}

inline bool ColorEdit4(const char* label, const char* tooltip, float col[4])
{
	return custom::ColorEdit4(label, tooltip, col,
		ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_PickerHueBar |
		ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoInputs);
}

} // namespace edited

namespace ImGui {

inline void Keybox(const char* label, bool* v, int* keybind)
{
	struct bindbox_cursor { int mode = 0; };

	std::string row_label = std::string("keybind_row_") + label;
	std::string bind_label = std::string("keybind_") + label + "##bind";

	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return;

	const ImGuiID id = window->GetID(row_label.c_str());
	static std::map<ImGuiID, bindbox_cursor> anim;
	auto it_anim = anim.find(id);
	if (it_anim == anim.end()) {
		anim.insert({ id, bindbox_cursor() });
		it_anim = anim.find(id);
	}

	const ImGuiStyle& style = GetStyle();
	const ImVec2 pos = window->DC.CursorPos;
	const float w = GetContentRegionMax().x - style.WindowPadding.x;
	const ImRect total_bb(pos, pos + ImVec2(w, 39.f));

	ItemSize(total_bb);
	if (!ItemAdd(total_bb, id, NULL, ImGuiItemFlags_NoNav))
		return;

	PushFont(font::medium_small);
	const ImVec2 label_size = CalcTextSize(label);
	const float label_y = total_bb.GetCenter().y - label_size.y * 0.5f;
	window->DrawList->AddText(ImVec2(total_bb.Min.x, label_y), c::label::active, label);
	PopFont();

	SetCursorScreenPos(total_bb.Min + ImVec2(w - 90.f, 5.f));
	PushItemWidth(85.f);
	custom::Keybind(bind_label.c_str(), keybind, &it_anim->second.mode);
	PopItemWidth();

	if (v)
		*v = (*keybind != 0);
}

} // namespace ImGui
