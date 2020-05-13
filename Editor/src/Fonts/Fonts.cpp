#include "Fonts.h"

ImFont* Fonts::Consolas = nullptr;
ImFont* Fonts::DroidSans = nullptr;

void Fonts::LoadFonts()
{
	ImGuiIO& io = ImGui::GetIO();
	DroidSans = io.Fonts->AddFontFromMemoryCompressedTTF(DroidSans_compressed_data, DroidSans_compressed_size, 16.0f);
	Consolas = io.Fonts->AddFontFromMemoryCompressedTTF(Consolas_compressed_data, Consolas_compressed_size, 16.0f);
}
