#pragma once

class Vector3f;

namespace ImGui
{
	bool Transform(Vector3f& position, Vector3f& rotation, Vector3f& scale);
}