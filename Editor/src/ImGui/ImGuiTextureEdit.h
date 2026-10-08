#pragma once

#include "imgui/imgui.h"

#include "Asset/Texture.h"

namespace ImGui
{
	// The filter/wrap combos edit filterMethod/wrapMethod when given, otherwise the shared texture itself
	bool Texture2DEdit(const char* label, Ref<Texture2D>& texture, const ImVec2& size = ImVec2(64.0f, 64.0f),
		Texture::FilterMethod* filterMethod = nullptr, Texture::WrapMethod* wrapMethod = nullptr);
}
