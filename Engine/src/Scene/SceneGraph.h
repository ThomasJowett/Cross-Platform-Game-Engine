#pragma once

#include "Components.h"
#include "Scene/Entity.h"

class SceneGraph
{
public:
	static void Traverse(entt::registry& registry);
	static void TraverseUI(entt::registry& registry, uint32_t viewportWidth, uint32_t viewportHeight);
	static void Reparent(Entity entity, Entity parent);
	static void Unparent(Entity entity);
	static void Remove(Entity entity);
	static std::vector<Entity> GetChildren(Entity entity);
	static entt::entity FindEntity(const std::vector<std::string>& path, entt::registry& registry);
private:
	static void UpdateTransform(TransformComponent* transformComp, HierarchyComponent* hierarchyComp, entt::registry& registry);

	static void UpdateUIWidgetTransform(WidgetComponent* transformComp, HierarchyComponent* hierachyComp, entt::registry& registry);
};
