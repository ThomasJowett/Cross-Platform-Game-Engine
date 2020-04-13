#include "stdafx.h"

#include "ImGuiUtilites.h"

void ImGui::Image(Ref<Texture> texture, const ImVec2& size, const ImVec4& tint_col, const ImVec4& border_col)
{
	ImGuiIO& io = ImGui::GetIO();
	ImTextureID my_tex_id = (void*)(intptr_t)texture->GetRendererID();
	ImGui::Image(my_tex_id, size, ImVec2(0, 1), ImVec2(1, 0), tint_col, border_col);
}

IMGUI_API bool ImGui::ImageButton(Ref<Texture> texture, const ImVec2& size, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
{
	ImGuiIO& io = ImGui::GetIO();
	ImTextureID my_tex_id = (void*)(intptr_t)texture->GetRendererID();
	return ImGui::ImageButton(my_tex_id, size, ImVec2(0, 1), ImVec2(1, 0), frame_padding, bg_col, tint_col);
}
