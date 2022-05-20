#pragma once

#include "Components/Components.h"
#include "Scene/Entity.h"

class SceneGraph
{
public:
	static void Traverse(entt::registry& registry);
	static void Reparent(Entity entity, Entity parent, entt::registry& registry);
	static void Unparent(Entity entity, entt::registry& registry);
	static void Remove(Entity entity, entt::registry& registry);
	static std::vector<Entity> GetChildren(Entity entity, entt::registry& registry);
private:
	static void UpdateTransform(TransformComponent* transformComp, HierarchyComponent* hierarchyComp, entt::registry& registry);
};
