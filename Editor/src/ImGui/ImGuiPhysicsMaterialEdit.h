#pragma once

#include "imgui/imgui.h"

#include "Physics/PhysicsMaterial.h"

namespace ImGui
{
IMGUI_API bool PhysMaterialEdit(const char* label, Ref<PhysicsMaterial>& physMaterial, Ref<PhysicsMaterial>& defaultPyhsMaterial);
}
