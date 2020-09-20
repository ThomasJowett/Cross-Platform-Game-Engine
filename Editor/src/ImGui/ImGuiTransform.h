#pragma once

#include"imgui/imgui.h"

class Matrix4x4;

namespace ImGui
{
	IMGUI_API bool Transform(Matrix4x4& Transform);
}