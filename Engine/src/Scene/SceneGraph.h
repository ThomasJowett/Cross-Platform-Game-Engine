#pragma once

#include "Components.h"
#include "Scene/Entity.h"

class SceneGraph
{
public:
	static void Traverse(entt::registry& registry);
	static void TraverseUI(entt::registry& registry, uint32_t viewportWidth, uint32_t viewportHeight);

	// True if this entity or any ancestor carries a HiddenComponent - HiddenComponent is never added
	// onto descendants, so this walks up the parent chain rather than checking a single entity.
	static bool IsEffectivelyHidden(entt::registry& registry, entt::entity entity);
	static void Reparent(Entity entity, Entity parent);
	static void Unparent(Entity entity);
	static void Remove(Entity entity);
	static void MoveBefore(Entity entity, Entity before);
	static void MoveAfter(Entity entity, Entity after);
	static std::vector<Entity> GetChildren(Entity entity);
	static entt::entity FindEntity(const std::vector<std::string>& path, entt::registry& registry);
private:
	static void UpdateTransform(TransformComponent* transformComp, HierarchyComponent* hierarchyComp, entt::registry& registry);

	static void UpdateUIWidgetTransform(WidgetComponent* transformComp, HierarchyComponent* hierachyComp, entt::registry& registry);
};
