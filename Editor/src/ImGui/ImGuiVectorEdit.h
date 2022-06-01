#pragma once

#include "imgui/imgui.h"

class Vector2f;
class Vector3f;

namespace ImGui
{
	IMGUI_API bool Vector(const char* label, Vector2f& vector, float resetValue = 0.0f);
	IMGUI_API bool Vector(const char* label, Vector3f& vector, float resetValue = 0.0f);
}
