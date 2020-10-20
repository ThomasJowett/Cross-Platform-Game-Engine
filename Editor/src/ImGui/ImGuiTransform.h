#pragma once

#include"imgui/imgui.h"

class Vector3f;

namespace ImGui
{
	IMGUI_API bool Transform(Vector3f& position, Vector3f& rotation, Vector3f& scale);
}