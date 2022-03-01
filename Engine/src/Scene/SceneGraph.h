#pragma once

#include "Components/Components.h"

class SceneGraph
{
public:
	static void Traverse(entt::registry& registry);
	static void Reparent();

private:
	static void UpdateTransform(TransformComponent* transformComp, HierarchyComponent* hierarchyComp, entt::registry& registry);
};
