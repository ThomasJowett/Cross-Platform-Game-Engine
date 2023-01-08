#pragma once

class Vector2f;
class Vector3f;

namespace ImGui
{
	bool Vector(const char* label, Vector2f& vector, float resetValue = 0.0f);
	bool Vector(const char* label, Vector3f& vector, float resetValue = 0.0f);
}
