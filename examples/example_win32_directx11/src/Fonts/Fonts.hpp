#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

#include <imgui.h>
#include <d3d11.h>
#include <d3dx11.h>

namespace FWork {
	namespace Fonts {

		inline ImFont* FontAwesomeRegular = nullptr;
		inline ImFont* FontAwesomeSolid = nullptr;
		inline ImFont* FontAwesomeSolid18 = nullptr;
		inline ImFont* FontAwesomeSolidBig = nullptr;

		inline ImFont* InterExtraLight = nullptr;
		inline ImFont* InterLight = nullptr;
		inline ImFont* InterThin = nullptr;
		inline ImFont* InterRegular = nullptr;
		inline ImFont* InterRegular14 = nullptr;
		inline ImFont* InterMedium = nullptr;
		inline ImFont* InterSemiBold = nullptr;
		inline ImFont* InterBold = nullptr;
		inline ImFont* InterBold12 = nullptr;
		inline ImFont* InterExtraBold = nullptr;
		inline ImFont* InterBlack = nullptr;
		inline ImFont* IconWeapon = nullptr;

		inline ImFont* NotoSansRegular = nullptr;
		inline ImFont* NotoSansSymbolsRegular = nullptr;
		inline ImFont* NotoEmojiRegular = nullptr;
		inline ImFont* ArialUnicode = nullptr;
		inline ImFont* LexendRegular = nullptr;

		inline ID3D11ShaderResourceView* Logo;
		void Initialize(ID3D11Device* Device);
		void LoadEspNameFonts();
	}
}