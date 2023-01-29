#pragma once

#include "Components/Components.h"
#include "Scene/Entity.h"

class SceneGraph
{
public:
	static void Traverse(entt::registry& registry);
	static void Reparent(Entity entity, Entity parent);
	static void Unparent(Entity entity);
	static void Remove(Entity entity);
	static std::vector<Entity> GetChildren(Entity entity);
	static entt::entity FindEntity(const std::vector<std::string>& path, entt::registry& registry);
private:
	static void UpdateTransform(TransformComponent* transformComp, HierarchyComponent* hierarchyComp, entt::registry& registry);
};
