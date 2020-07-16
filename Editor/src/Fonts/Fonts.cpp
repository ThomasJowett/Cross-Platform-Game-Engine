#include "Fonts.h"

#include "imgui/imgui.h"

#include "Fonts/DroidSans.cpp"
#include "Fonts/Consolas.cpp"
#include "Fonts/fa-solid.cpp"
#include "Fonts/fa-webfont.cpp"

#include "IconsFontAwesome5.h"

ImFont* Fonts::Consolas = nullptr;
ImFont* Fonts::DroidSans = nullptr;

void Fonts::LoadFonts(float fontSize)
{
	ImGuiIO& io = ImGui::GetIO();
	DroidSans = io.Fonts->AddFontFromMemoryCompressedTTF(DroidSans_compressed_data, DroidSans_compressed_size, fontSize);

	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.GlyphOffset.y += 1.0f;
	icons_config.OversampleH = icons_config.OversampleV = 1;
	icons_config.SizePixels = 13.0f * 1.0f;

	io.Fonts->AddFontFromMemoryCompressedTTF(fa_solid_compressed_data, fa_solid_compressed_size, fontSize, &icons_config, icons_ranges);
	io.Fonts->AddFontFromMemoryCompressedTTF(fa_webfont_compressed_data, fa_webfont_compressed_size, fontSize, &icons_config, icons_ranges);

	Consolas = io.Fonts->AddFontFromMemoryCompressedTTF(Consolas_compressed_data, Consolas_compressed_size, fontSize);
}
