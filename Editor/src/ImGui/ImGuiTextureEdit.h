#pragma once

#include "imgui/imgui.h"

#include "Asset/Texture.h"

namespace ImGui
{
	// filterMethod/wrapMethod belong to the owner, never the shared texture
	bool Texture2DEdit(const char* label, Ref<Texture2D>& texture, Texture::FilterMethod& filterMethod, Texture::WrapMethod& wrapMethod,
		const ImVec2& size = ImVec2(64.0f, 64.0f));
}
