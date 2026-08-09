#pragma once


#include "imgui.h"
#include "./examples/example_win32_directx11/ImAnim/ImVec2Anim.h"
#include "./examples/example_win32_directx11/ImAnim/ImVec4Anim.h"
#include <map>
#include <vector>
#include <string>

#include "./examples/example_win32_directx11/ImGui/stt.hpp"
#include "./examples/example_win32_directx11/ImGui/blur.hpp"


namespace font
{
	inline ImFont* icomoon_logo = nullptr;
	inline ImFont* icomoon_tabs = nullptr;
	inline ImFont* icomoon_page = nullptr;
	inline ImFont* icomoon_childs = nullptr;
	inline ImFont* inter_semibold = nullptr;
	inline ImFont* bold_small = nullptr;
	inline ImFont* medium_small = nullptr;
	inline ImFont* inter_regular = nullptr;
	inline ImFont* s_inter_semibold = nullptr;
	inline ImFont* inter_bold = nullptr;
	inline ImFont* inter_medium = nullptr;
	inline ImFont* inter_medium2 = nullptr;
	inline ImFont* icon_child = nullptr;
	inline ImFont* esp_font = nullptr;
	inline ImFont* description_font = nullptr;
	inline ImFont* regular_m = nullptr;
	inline ImFont* regular_l = nullptr;
	inline ImFont* small_font = nullptr;
	inline ImFont* icon_notify = nullptr;

	inline ImFont* lexend_medium = nullptr;
	inline ImFont* lexend_regular = nullptr;
	inline ImFont* lexend_semibold = nullptr;

	inline ImFont* iconuwu = nullptr;
}

inline ImFont* WeaponsIco2io = nullptr;
inline ImFont* Texesp;

inline ImVec4 ImColorToImVec4(const ImColor& color)
{
	return ImVec4(color.Value.x, color.Value.y, color.Value.z, color.Value.w);
}

namespace utils
{
	inline ImColor GetColorWithAlpha(ImColor color, float alpha)
	{
		return ImColor(color.Value.x, color.Value.y, color.Value.z, alpha);
	}
	inline ImVec2 center_text(ImVec2 min, ImVec2 max, const char* text)
	{
		ImVec2 size = ImGui::CalcTextSize(text);
		ImVec2 pos = ImVec2(
			min.x + (max.x - min.x) * 0.5f - size.x * 0.5f,
			min.y + (max.y - min.y) * 0.5f - size.y * 0.5f
		);
		return pos;
	}


	inline ImColor GetDarkColor(const ImColor& color)
	{

		float r, g, b, a;
		r = color.Value.x;
		g = color.Value.y;
		b = color.Value.z;
		a = 255;

		float darkPercentage = 0.2f;
		float darkR = r * darkPercentage;
		float darkG = g * darkPercentage;
		float darkB = b * darkPercentage;

		return ImColor(darkR, darkG, darkB, a);
	}
	inline ImVec4 ImColorToImVec4(const ImColor& color)
	{
		return ImVec4(color.Value.x, color.Value.y, color.Value.z, color.Value.w);
	}

}

inline namespace c
{
	static struct glow_draws
	{
		ImDrawList* draw_list;
		ImGuiID window_id;
	};

	inline float shader_alpha = 1.f;

	inline std::vector<glow_draws> glow_context;
	inline ImDrawList* glow_text_drawlist;

	// Основные цвета
	inline ImColor main_color(255, 0, 0);
	inline ImColor accent_color(255, 0, 0);
	inline ImColor dark_color(28, 30, 34); // Основной темный фон
	inline ImColor second_color(23, 23, 31); // Вторичный фон
	inline ImColor background_color(24, 24, 29, 240); // Фон окна
	inline ImColor stroke_color(48, 48, 58, 65); // Границы и линии
	inline ImColor window_bg_color(12, 10, 21, 245); // Основной фон окна

	inline ImColor black_color(0, 0, 0, 200);

	// Акценты и разделители
	inline ImVec4 accent = (ImVec4)main_color;
	inline ImVec4 separator = ImColor(45, 45, 50); // Линии между секциями
	inline float fTabOffset = 0.f;
	inline bool bTabState = false;
	inline int iTabTarget = 0;

	inline namespace anim
	{
		inline float speed = 6.0f;
	}

	inline namespace bg
	{
		inline ImVec4 background = ImColor(24, 24, 29); // Темный фон
		inline ImVec2 size = ImVec2(750, 525);
		inline float rounding = 16.f; // Скругление
	}

	inline namespace child
	{
		inline ImVec4 title = ImColor(32, 32, 40); // Заголовок темный
		inline ImVec4 background = ImColor(13, 13, 16, 210); // Фон ребенка
		inline ImVec4 stroke = ImColor(45, 45, 50); // Границы
		inline float rounding = 5.f; // Скругление

		inline ImVec4 outline = ImColor(75, 75, 95, 140);              // ↓
		inline ImVec4 background2 = ImColor(20, 20, 25, 195);          // ↓
	}

	namespace page
	{
		inline ImVec4 background_active = ImColor(36, 36, 46); // Активная страница
		inline ImVec4 background = ImColor(28, 30, 34); // Фон страницы

		inline ImVec4 text_hov = ImColor(255, 255, 255); // Белый при наведении
		inline ImVec4 text = ImColor(200, 200, 200); // Основной текст

		inline float rounding = 4.f; // Скругление секции
	}

	inline namespace elements
	{
		inline float rounding = 4.f;
	}

	inline namespace checkbox
	{
		inline ImVec4 mark = ImColor(255, 69, 0); // Красная галочка
	}

	inline namespace text
	{
		inline namespace label
		{
			inline ImColor active = ImColor(255, 255, 255); // Активный белый текст
			inline ImColor hovered = ImColor(200, 200, 200); // При наведении
			inline ImColor regular = ImColor(111, 111, 111); // По умолчанию
		}

		inline namespace description
		{
			inline ImColor active = ImColor(200, 200, 200, 102);
			inline ImColor hovered = ImColor(200, 200, 200, 63);
			inline ImColor regular = ImColor(200, 200, 200, 40);
		}

		inline ImVec4 text_active = ImColor(255, 255, 255);
		inline ImVec4 text_hov = ImColor(255, 69, 0); // Красный акцент при наведении
		inline ImVec4 text = ImColor(200, 200, 200);
	}
}

// Тема (переключение)
inline bool bTheme = true;
inline float themeTransition = 0.0f;
inline float transitionSpeed = 0.05f;

inline ImColor ImLerpColor(const ImColor& col1, const ImColor& col2, float t) {
	return ImColor(
		ImLerp(col1.Value.x, col2.Value.x, t),
		ImLerp(col1.Value.y, col2.Value.y, t),
		ImLerp(col1.Value.z, col2.Value.z, t),
		ImLerp(col1.Value.w, col2.Value.w, t)
	);
}

// Обновление темы
inline void UpdateTheme()
{

	float change_anim_speed = ImGui::GetIO().DeltaTime * 10.f;

	shader_alpha = 1.f;

	c::accent_color = utils::GetDarkColor(c::main_color);

	if (bTheme) { // Тёмная тема

		ImGui::GetStyle().Colors[ImGuiCol_Separator] = c::stroke_color;

		//c::main_color = ImLerpColor(c::main_color, ImColor(0, 150, 255), change_anim_speed);
		c::dark_color = ImLerpColor(c::dark_color, ImColor(15, 17, 22), change_anim_speed);
		c::second_color = ImLerpColor(c::second_color, ImColor(23, 23, 27, 70), change_anim_speed);
		c::background_color = ImLerpColor(c::background_color, ImColor(20, 22, 26, 250), change_anim_speed);
		c::stroke_color = ImLerpColor(c::stroke_color, ImColor(180, 180, 180, 12), change_anim_speed);
		c::window_bg_color = ImLerpColor(c::window_bg_color, ImColor(6, 6, 6, 210), change_anim_speed);

		c::child::title = ImLerpColor(c::child::title, ImColor(21, 21, 27), change_anim_speed);
		c::child::background = ImLerpColor(c::child::background, ImColor(11, 11, 14, 55), change_anim_speed);
		c::child::stroke = ImLerpColor(c::child::stroke, ImColor(180, 180, 180, 22), change_anim_speed);

		c::page::background_active = ImLerpColor(c::page::background_active, ImColor(35, 38, 46), change_anim_speed);
		c::page::background = ImLerpColor(c::page::background, ImColor(28, 30, 36), change_anim_speed);

		c::text::label::active = ImLerpColor(c::text::label::active, ImColor(255, 255, 255), change_anim_speed);
		c::text::label::hovered = ImLerpColor(c::text::label::hovered, ImColor(200, 220, 250), change_anim_speed);
		c::text::label::regular = ImLerpColor(c::text::label::regular, ImColor(170, 175, 180), change_anim_speed);

		c::text::description::active = ImLerpColor(c::text::description::active, ImColor(200, 200, 200, 150), change_anim_speed);
		c::text::description::hovered = ImLerpColor(c::text::description::hovered, ImColor(200, 200, 200, 100), change_anim_speed);
		c::text::description::regular = ImLerpColor(c::text::description::regular, ImColor(150, 150, 150, 80), change_anim_speed);

		c::text::text_active = ImLerpColor(c::text::text_active, ImColor(255, 255, 255), change_anim_speed);
		c::text::text_hov = ImLerpColor(c::text::text_hov, ImColor(255, 255, 255), change_anim_speed);
		c::text::text = ImLerpColor(c::text::text, ImColor(170, 170, 170), change_anim_speed);
	}
	else {
		ImGui::GetStyle().Colors[ImGuiCol_Separator] = c::stroke_color;

		//c::main_color = ImLerpColor(c::main_color, ImColor(0, 110, 200), change_anim_speed); // Более мягкий синий акцент
		c::dark_color = ImLerpColor(c::dark_color, ImColor(230, 230, 230), change_anim_speed); // Серый фон вместо резкого белого
		c::second_color = ImLerpColor(c::second_color, ImColor(190, 190, 190, 70), change_anim_speed);
		c::background_color = ImLerpColor(c::background_color, ImColor(245, 245, 245, 220), change_anim_speed); // Мягкий фон вместо яркого белого
		c::stroke_color = ImLerpColor(c::stroke_color, ImColor(0, 0, 0, 50), change_anim_speed);
		c::window_bg_color = ImLerpColor(c::window_bg_color, ImColor(250, 250, 250, 160), change_anim_speed); // Уменьшена прозрачность

		c::bg::background = ImLerpColor(c::bg::background, ImColor(235, 235, 235), change_anim_speed);
		c::child::title = ImLerpColor(c::child::title, ImColor(200, 200, 200), change_anim_speed);
		c::child::background = ImLerpColor(c::child::background, ImColor(250, 250, 250, 10), change_anim_speed);
		c::child::stroke = ImLerpColor(c::child::stroke, ImColor(0, 0, 0, 40), change_anim_speed);

		c::page::background_active = ImLerpColor(c::page::background_active, ImColor(210, 210, 210), change_anim_speed);
		c::page::background = ImLerpColor(c::page::background, ImColor(240, 240, 240), change_anim_speed);

		c::text::label::active = ImLerpColor(c::text::label::active, ImColor(255, 255, 255), change_anim_speed); // Более мягкий чёрный
		c::text::label::hovered = ImLerpColor(c::text::label::hovered, ImColor(45, 45, 45), change_anim_speed);
		c::text::label::regular = ImLerpColor(c::text::label::regular, ImColor(75, 75, 75), change_anim_speed);

		c::text::description::active = ImLerpColor(c::text::description::active, ImColor(100, 100, 100, 100), change_anim_speed);
		c::text::description::hovered = ImLerpColor(c::text::description::hovered, ImColor(100, 100, 100, 70), change_anim_speed);
		c::text::description::regular = ImLerpColor(c::text::description::regular, ImColor(100, 100, 100, 45), change_anim_speed);

		c::text::text_active = ImLerpColor(c::text::text_active, ImColor(20, 20, 20), change_anim_speed);
		c::text::text_hov = ImLerpColor(c::text::text_hov, ImColor(0, 100, 190), change_anim_speed); // Более мягкий синий акцент
		c::text::text = ImLerpColor(c::text::text, ImColor(80, 80, 80), change_anim_speed);
	}

}
