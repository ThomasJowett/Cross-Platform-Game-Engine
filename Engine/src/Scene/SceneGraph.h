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

	// Every entity carrying a WidgetComponent, in draw order (Canvas -> firstChild -> nextSibling,
	// parent before children, earlier siblings before later ones) - the single canonical z-order both
	// RenderUI (what's on top visually) and HitTestUI (what's on top for input) must agree on.
	static void CollectUIDrawOrder(entt::registry& registry, std::vector<entt::entity>& outOrder);

	// Returns the topmost (last in draw order) interactive widget whose screen-space rect contains
	// mousePosition (in the same canvas-local pixel space as canvasWidth/canvasHeight), or entt::null.
	static entt::entity HitTestUI(entt::registry& registry, Vector2f mousePosition, float canvasWidth, float canvasHeight);

	static void Reparent(Entity entity, Entity parent);
	static void Unparent(Entity entity);
	static void Remove(Entity entity);
	static void MoveBefore(Entity entity, Entity before);
	static void MoveAfter(Entity entity, Entity after);
	static std::vector<Entity> GetChildren(Entity entity);
	static entt::entity FindEntity(const std::vector<std::string>& path, entt::registry& registry);
private:
	static void UpdateTransform(TransformComponent* transformComp, HierarchyComponent* hierarchyComp, entt::registry& registry);

	// All position/size/anchor/margin math happens in a single consistent unit - reference-resolution
	// pixels (WidgetComponent::s_referenceWidth/Height) - all the way down the tree, exactly like
	// WidgetComponent's own Set* setters already assume. globalScaleX/Y (viewport pixels per reference
	// pixel) is threaded through unchanged and applied exactly once, at each widget's own render matrix -
	// never baked into worldOrigin, or it would compound at every level of nesting. A fixedWidth/Height
	// widget divides its own size by the matching factor first, so the render-matrix multiply cancels
	// back out to a literal, resolution-independent screen-pixel size (see UpdateUIWidgetTransform).
	static void UpdateUIWidgetTransform(entt::entity entity, WidgetComponent* widget, HierarchyComponent* hierarchyComp,
		entt::registry& registry, const Matrix4x4& parentWorldOrigin, float parentWidth, float parentHeight,
		float globalScaleX, float globalScaleY, bool rectProvidedByParent);

	static void LayoutStack(const StackLayoutComponent& stack, WidgetComponent* containerWidget, Vector2f containerSize,
		HierarchyComponent* hierarchyComp, entt::registry& registry);
	static void LayoutGrid(const GridLayoutComponent& grid, WidgetComponent* containerWidget, Vector2f containerSize,
		HierarchyComponent* hierarchyComp, entt::registry& registry);
	static void LayoutScrollBox(ScrollBoxComponent& scrollBox, Vector2f containerSize, HierarchyComponent* hierarchyComp,
		entt::registry& registry);
};
