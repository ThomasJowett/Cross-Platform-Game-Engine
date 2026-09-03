#include "SceneGraph.h"
#include "Components.h"

void SceneGraph::Traverse(entt::registry& registry)
{
	PROFILE_FUNCTION();

	auto nonHierarchyView = registry.view<TransformComponent>(entt::exclude<HierarchyComponent>);

	for (auto entity : nonHierarchyView)
	{
		registry.get<TransformComponent>(entity).SetWorldMatrix(Matrix4x4());
	}

	registry.view<TransformComponent, HierarchyComponent>(/*entt::exclude<CanvasComponent>*/).each(
		[&]([[maybe_unused]] const auto entity, auto& transformComp, auto& hierarchyComp)
		{
			// if the entity is active and a root node
			if (hierarchyComp.isActive && hierarchyComp.parent == entt::null)
			{
				UpdateTransform(&transformComp, &hierarchyComp, registry);
			}
		});
}

bool SceneGraph::IsEffectivelyHidden(entt::registry& registry, entt::entity entity)
{
	while (entity != entt::null)
	{
		if (HiddenComponent* hiddenComp = registry.try_get<HiddenComponent>(entity))
		{
			if (hiddenComp->hidden)
				return true;
		}
		HierarchyComponent* hierarchyComp = registry.try_get<HierarchyComponent>(entity);
		entity = hierarchyComp != nullptr ? hierarchyComp->parent : entt::null;
	}
	return false;
}

void SceneGraph::TraverseUI(entt::registry& registry, uint32_t viewportWidth, uint32_t viewportHeight)
{
	PROFILE_FUNCTION();

	// Every widget is authored against a virtual s_referenceWidth x s_referenceHeight canvas - anchors
	// are resolution-independent fractions, so nothing else needs to know the real viewport size except
	// this one global scale, applied once per widget at render-matrix time (see UpdateUIWidgetTransform).
	float globalScaleX = (float)viewportWidth / (float)WidgetComponent::s_referenceWidth;
	float globalScaleY = (float)viewportHeight / (float)WidgetComponent::s_referenceHeight;

	// Every widget must hang off a CanvasComponent root - a widget with no Canvas ancestor is not traversed.
	auto canvasView = registry.view<CanvasComponent, HierarchyComponent>();
	for (auto canvasEntity : canvasView)
	{
		HierarchyComponent& canvasHierarchyComp = canvasView.get<HierarchyComponent>(canvasEntity);

		entt::entity child = canvasHierarchyComp.firstChild;
		while (child != entt::null)
		{
			WidgetComponent* childWidget = registry.try_get<WidgetComponent>(child);
			HierarchyComponent* childHierarchyComp = registry.try_get<HierarchyComponent>(child);
			if (childWidget != nullptr && childHierarchyComp != nullptr && childHierarchyComp->isActive)
			{
				UpdateUIWidgetTransform(child, childWidget, childHierarchyComp, registry, Matrix4x4(),
					(float)WidgetComponent::s_referenceWidth, (float)WidgetComponent::s_referenceHeight, globalScaleX, globalScaleY, false);
			}
			child = childHierarchyComp != nullptr ? childHierarchyComp->nextSibling : entt::null;
		}
	}
}

// ancestorHidden is carried down from the parent rather than re-walking up per node (which
// SceneGraph::IsEffectivelyHidden does, for the non-tree-walked render loops that need it) - this walk
// already visits every ancestor before its descendants, so propagating the flag is equivalent and cheaper.
static void CollectUIDrawOrderRecursive(entt::registry& registry, entt::entity node, bool ancestorHidden, std::vector<entt::entity>& outOrder)
{
	while (node != entt::null)
	{
		HierarchyComponent* hierarchyComp = registry.try_get<HierarchyComponent>(node);

		bool selfHidden = false;
		if (HiddenComponent* hiddenComp = registry.try_get<HiddenComponent>(node))
			selfHidden = hiddenComp->hidden;
		bool effectivelyHidden = ancestorHidden || selfHidden;

		if (!effectivelyHidden
			&& registry.try_get<WidgetComponent>(node) != nullptr
			&& (hierarchyComp == nullptr || hierarchyComp->isActive))
		{
			outOrder.push_back(node);
		}

		if (hierarchyComp != nullptr)
			CollectUIDrawOrderRecursive(registry, hierarchyComp->firstChild, effectivelyHidden, outOrder);

		node = hierarchyComp != nullptr ? hierarchyComp->nextSibling : entt::null;
	}
}

void SceneGraph::CollectUIDrawOrder(entt::registry& registry, std::vector<entt::entity>& outOrder)
{
	PROFILE_FUNCTION();

	auto canvasView = registry.view<CanvasComponent, HierarchyComponent>();
	for (auto canvasEntity : canvasView)
	{
		HierarchyComponent& canvasHierarchyComp = canvasView.get<HierarchyComponent>(canvasEntity);
		bool canvasHidden = false;
		if (HiddenComponent* hiddenComp = registry.try_get<HiddenComponent>(canvasEntity))
			canvasHidden = hiddenComp->hidden;
		CollectUIDrawOrderRecursive(registry, canvasHierarchyComp.firstChild, canvasHidden, outOrder);
	}
}

entt::entity SceneGraph::HitTestUI(entt::registry& registry, Vector2f mousePosition, float canvasWidth, float canvasHeight)
{
	PROFILE_FUNCTION();

	// Same fixed-vs-scaled rect math already proven in ViewportPanel.cpp's anchor/margin handles -
	// widget->position/size are reference-resolution pixels, converted to canvas-local screen pixels.
	float globalScaleX = canvasWidth / (float)WidgetComponent::s_referenceWidth;
	float globalScaleY = canvasHeight / (float)WidgetComponent::s_referenceHeight;

	std::vector<entt::entity> drawOrder;
	CollectUIDrawOrder(registry, drawOrder);

	// Walking in draw order and always overwriting on a hit means the last (topmost) match wins,
	// with a single pass and no need to walk in reverse.
	entt::entity result = entt::null;
	for (entt::entity entity : drawOrder)
	{
		// Only entities with an interactive widget type participate - containers (Canvas/Stack/Grid/
		// ScrollBox) draw nothing of their own, so they have no visual footprint to block clicks with.
		if (registry.try_get<ButtonComponent>(entity) == nullptr)
			continue;

		WidgetComponent& widget = registry.get<WidgetComponent>(entity);
		float renderWidth = widget.fixedWidth ? widget.size.x : widget.size.x * globalScaleX;
		float renderHeight = widget.fixedHeight ? widget.size.y : widget.size.y * globalScaleY;
		float left = widget.position.x * globalScaleX;
		float top = widget.position.y * globalScaleY;

		if (mousePosition.x >= left && mousePosition.x <= left + renderWidth
			&& mousePosition.y >= top && mousePosition.y <= top + renderHeight)
		{
			result = entity;
		}
	}
	return result;
}

void SceneGraph::Reparent(Entity entity, Entity parent)
{
	PROFILE_FUNCTION();
	ASSERT(entity.BelongsToScene(parent.GetScene()), "Entities must belong to the same scene");
	entt::registry& registry = entity.GetScene()->GetRegistry();
	Unparent(entity);

	ASSERT(parent.GetHandle() != entt::null, "Parent must not be null");
	ASSERT(entity.IsSceneValid(), "Entity must be valid!");

	//is the parent a child of this entity
	entt::entity parentCheck = parent.GetHandle();
	while (parentCheck != entt::null)
	{
		if (parentCheck == entity.GetHandle())
			return;
		HierarchyComponent* parentCheckHierarchyComp = registry.try_get<HierarchyComponent>(parentCheck);
		if (parentCheckHierarchyComp == nullptr)
			break;
		parentCheck = parentCheckHierarchyComp->parent;
	}

	HierarchyComponent& hierarchyComp = entity.GetOrAddComponent<HierarchyComponent>();

	hierarchyComp.parent = entt::null;
	hierarchyComp.nextSibling = entt::null;
	hierarchyComp.previousSibling = entt::null;
	hierarchyComp.parent = parent.GetHandle();

	HierarchyComponent& parentHierarchyComp = parent.GetOrAddComponent<HierarchyComponent>();
	if (parentHierarchyComp.firstChild == entt::null)
	{
		parentHierarchyComp.firstChild = entity.GetHandle();
	}
	else
	{
		entt::entity previousSibling = parentHierarchyComp.firstChild;
		HierarchyComponent* currentHierachyComp = registry.try_get<HierarchyComponent>(previousSibling);
		while (currentHierachyComp && currentHierachyComp->nextSibling != entt::null)
		{
			previousSibling = currentHierachyComp->nextSibling;
			currentHierachyComp = registry.try_get<HierarchyComponent>(previousSibling);
		}

		if (currentHierachyComp)
			currentHierachyComp->nextSibling = entity.GetHandle();
		hierarchyComp.previousSibling = previousSibling;
	}
}

void SceneGraph::Unparent(Entity entity)
{
	PROFILE_FUNCTION();
	entt::registry& registry = entity.GetScene()->GetRegistry();
	HierarchyComponent* hierachyComp = entity.TryGetComponent<HierarchyComponent>();

	if (hierachyComp != nullptr && hierachyComp->parent != entt::null)
	{
		HierarchyComponent* parentHierachyComp = registry.try_get<HierarchyComponent>(hierachyComp->parent);

		if (parentHierachyComp != nullptr)
		{
			// if this is the first child of the parent fix update that link
			if (parentHierachyComp->firstChild == entity.GetHandle())
				parentHierachyComp->firstChild = hierachyComp->nextSibling;

			if (parentHierachyComp->firstChild == entt::null
				&& parentHierachyComp->parent == entt::null
				&& parentHierachyComp->nextSibling == entt::null
				&& parentHierachyComp->previousSibling == entt::null)
			{
				registry.remove<HierarchyComponent>(hierachyComp->parent);
			}
		}

		// update the links of any siblings
		if (hierachyComp->nextSibling != entt::null && hierachyComp->previousSibling != entt::null)
		{
			HierarchyComponent* nextSiblingHierarchyComp = registry.try_get<HierarchyComponent>(hierachyComp->nextSibling);
			HierarchyComponent* previousSiblingHierarchyComp = registry.try_get<HierarchyComponent>(hierachyComp->previousSibling);
			nextSiblingHierarchyComp->previousSibling = hierachyComp->previousSibling;
			previousSiblingHierarchyComp->nextSibling = hierachyComp->nextSibling;
		}
		else if (hierachyComp->nextSibling != entt::null)
		{
			HierarchyComponent* nextSiblingHierarchyComp = registry.try_get<HierarchyComponent>(hierachyComp->nextSibling);
			nextSiblingHierarchyComp->previousSibling = entt::null;
		}
		else if (hierachyComp->previousSibling != entt::null)
		{
			HierarchyComponent* previousSiblingHierarchyComp = registry.try_get<HierarchyComponent>(hierachyComp->previousSibling);
			previousSiblingHierarchyComp->nextSibling = entt::null;
		}

		hierachyComp->parent = entt::null;
		hierachyComp->nextSibling = entt::null;
		hierachyComp->previousSibling = entt::null;

		// if there is no children then the HierarchyComponent is not needed
		if (hierachyComp->firstChild == entt::null)
			entity.RemoveComponent<HierarchyComponent>();
	}
}

void SceneGraph::Remove(Entity entity)
{
	PROFILE_FUNCTION();
	entt::registry& registry = entity.GetScene()->GetRegistry();
	HierarchyComponent* hierarchyComp = entity.TryGetComponent<HierarchyComponent>();

	if (hierarchyComp != nullptr)
	{
		entt::entity child = hierarchyComp->firstChild;
		while (child != entt::null)
		{
			Entity childEntity = { child, entity.GetScene() };
			Remove(childEntity);
			child = hierarchyComp->firstChild;
		}
	}
	Unparent(entity);

	ENGINE_DEBUG("Removed {0}", entity.GetName());
	entity.GetScene()->OnEntityDestroyed(entity);
	registry.destroy(entity);
}

void SceneGraph::MoveBefore(Entity entity, Entity before)
{
	PROFILE_FUNCTION();
	if (entity == before) return;
	entt::registry& registry = entity.GetScene()->GetRegistry();

	// Remove from current parent/sibling list
	Unparent(entity);

	HierarchyComponent& beforeHC = before.GetOrAddComponent<HierarchyComponent>();
	entt::entity parentHandle = beforeHC.parent;
	Entity parent(parentHandle, entity.GetScene());

	// Insert as sibling before 'before'
	HierarchyComponent& entityHC = entity.GetOrAddComponent<HierarchyComponent>();
	entityHC.parent = parentHandle;
	entityHC.nextSibling = before.GetHandle();
	entityHC.previousSibling = beforeHC.previousSibling;

	if (beforeHC.previousSibling != entt::null) {
		HierarchyComponent* prevHC = registry.try_get<HierarchyComponent>(beforeHC.previousSibling);
		if (prevHC) prevHC->nextSibling = entity.GetHandle();
	}
	else if (parentHandle != entt::null) {
		// If 'before' was the first child, update parent's firstChild
		HierarchyComponent* parentHC = registry.try_get<HierarchyComponent>(parentHandle);
		if (parentHC) parentHC->firstChild = entity.GetHandle();
	}

	beforeHC.previousSibling = entity.GetHandle();
}

void SceneGraph::MoveAfter(Entity entity, Entity after)
{
	PROFILE_FUNCTION();
	if (entity == after) return;
	entt::registry& registry = entity.GetScene()->GetRegistry();

	// Remove from current parent/sibling list
	Unparent(entity);

	HierarchyComponent& afterHC = after.GetOrAddComponent<HierarchyComponent>();
	entt::entity parentHandle = afterHC.parent;
	Entity parent(parentHandle, entity.GetScene());

	// Insert as sibling after 'after'
	HierarchyComponent& entityHC = entity.GetOrAddComponent<HierarchyComponent>();
	entityHC.parent = parentHandle;
	entityHC.previousSibling = after.GetHandle();
	entityHC.nextSibling = afterHC.nextSibling;

	if (afterHC.nextSibling != entt::null) {
		HierarchyComponent* nextHC = registry.try_get<HierarchyComponent>(afterHC.nextSibling);
		if (nextHC) nextHC->previousSibling = entity.GetHandle();
	}
	afterHC.nextSibling = entity.GetHandle();
}

std::vector<Entity> SceneGraph::GetChildren(Entity entity)
{
	PROFILE_FUNCTION();
	entt::registry& registry = entity.GetScene()->GetRegistry();
	std::vector<Entity> children;
	HierarchyComponent* hierarchyComp = entity.TryGetComponent<HierarchyComponent>();
	if (hierarchyComp != nullptr)
	{
		entt::entity child = hierarchyComp->firstChild;
		while (child != entt::null && registry.valid(child))
		{
			children.emplace_back(Entity(child, entity.GetScene()));
			hierarchyComp = registry.try_get<HierarchyComponent>(child);
			if (hierarchyComp)
				child = hierarchyComp->nextSibling;
		}
	}
	return children;
}

entt::entity SceneGraph::FindEntity(const std::vector<std::string>& path, entt::registry& registry)
{
	PROFILE_FUNCTION();
	auto view = registry.view<NameComponent, HierarchyComponent>();
	for (auto entity : view)
	{
		auto [nameComp, hierarchyComp] = view.get<NameComponent, HierarchyComponent>(entity);
		if (hierarchyComp.parent == entt::null && nameComp.name == path[0])
		{
			if (path.size() == 1)
				return entity;
			else
			{
				int i = 1;
				entt::entity child = hierarchyComp.firstChild;
				while (child != entt::null && registry.valid(child) && i < path.size())
				{
					nameComp = registry.get<NameComponent>(child);
					hierarchyComp = registry.get<HierarchyComponent>(child);
					if (nameComp.name == path[i])
					{
						i++;
						if (i == path.size())
							return child;
						child = hierarchyComp.firstChild;
					}
					else
						child = hierarchyComp.nextSibling;
				}
			}
		}
	}
	return entt::null;
}

void SceneGraph::UpdateTransform(TransformComponent* transformComp, HierarchyComponent* hierarchyComp, entt::registry& registry)
{
	PROFILE_FUNCTION();
	if (hierarchyComp->parent != entt::null)
	{
		TransformComponent* parentTransformComp = registry.try_get<TransformComponent>(hierarchyComp->parent);
		transformComp->SetWorldMatrix(parentTransformComp->GetWorldMatrix());
	}
	else
	{
		transformComp->SetWorldMatrix(Matrix4x4());
	}

	// Update the first child
	if (hierarchyComp->firstChild != entt::null)
	{
		TransformComponent* childTransformComp = registry.try_get<TransformComponent>(hierarchyComp->firstChild);
		HierarchyComponent* childHierarchyComp = registry.try_get<HierarchyComponent>(hierarchyComp->firstChild);
		if (childHierarchyComp != nullptr && childTransformComp != nullptr)
		{
			UpdateTransform(childTransformComp, childHierarchyComp, registry);
		}
	}

	// Update the next sibling
	if (hierarchyComp->nextSibling != entt::null)
	{
		TransformComponent* siblingTransformComp = registry.try_get<TransformComponent>(hierarchyComp->nextSibling);
		HierarchyComponent* siblingHierarchyComp = registry.try_get<HierarchyComponent>(hierarchyComp->nextSibling);
		if (siblingHierarchyComp != nullptr && siblingTransformComp != nullptr)
		{
			UpdateTransform(siblingTransformComp, siblingHierarchyComp, registry);
		}
	}
}

void SceneGraph::UpdateUIWidgetTransform(entt::entity entity, WidgetComponent* widget, HierarchyComponent* hierarchyComp,
	entt::registry& registry, const Matrix4x4& parentWorldOrigin, float parentWidth, float parentHeight,
	float globalScaleX, float globalScaleY, bool rectProvidedByParent)
{
	PROFILE_FUNCTION();

	Vector2f localPos, localSize;

	if (rectProvidedByParent)
	{
		// A Stack/Grid/ScrollBox ancestor already wrote position/size for this widget this frame -
		// anchors are ignored while a widget is under container-managed layout.
		localPos = widget->position;
		localSize = widget->size;
	}
	else
	{
		// parentWidth/Height, and every value here, are reference-resolution pixels - anchors need no
		// scaling (they're fractions), and margins/sizes are authored directly in reference pixels too,
		// exactly matching WidgetComponent's own Set* setters. The only viewport-size-aware step in this
		// whole tree is globalScale, applied once below when building the render matrix.
		float leftAnchorPx = parentWidth * widget->anchorLeft;
		float rightAnchorPx = parentWidth * widget->anchorRight;
		float topAnchorPx = parentHeight * widget->anchorTop;
		float bottomAnchorPx = parentHeight * widget->anchorBottom;

		localPos.x = leftAnchorPx + widget->marginLeft;
		localPos.y = topAnchorPx + widget->marginTop;

		localSize.x = widget->fixedWidth ? widget->size.x : (rightAnchorPx + widget->marginRight) - localPos.x;
		localSize.y = widget->fixedHeight ? widget->size.y : (bottomAnchorPx + widget->marginBottom) - localPos.y;

		widget->position = localPos;
		widget->size = localSize;
	}

	// Origin (translate+rotate, no scale) is what gets passed to children - keeps this widget's
	// pixel-size Scale out of the matrix children compose their own translation against. Stays in
	// reference-resolution space the whole way down; globalScale is applied fresh below, never baked
	// into worldOrigin, or it would compound once per level of nesting.
	//
	// Fixed width/height means literally fixed - the same real screen-pixel size at any resolution -
	// not a reference-resolution size that still scales with the viewport. Pre-dividing by globalScale
	// here cancels out the multiply below, so the rendered size ends up exactly widget->size unscaled.
	// (Position still scales normally, so the widget stays correctly anchored either way.)
	Vector3f renderSize(
		widget->fixedWidth ? localSize.x / globalScaleX : localSize.x,
		widget->fixedHeight ? localSize.y / globalScaleY : localSize.y,
		1.0f);
	Vector3f halfRenderSize(renderSize.x * 0.5f, -renderSize.y * 0.5f, 0.0f);
	Matrix4x4 localOrigin = Matrix4x4::Translate(Vector3f(localPos.x, -localPos.y, 0.0f)) * Matrix4x4::RotateZ(widget->rotation);
	Matrix4x4 worldOrigin = parentWorldOrigin * localOrigin;
	Matrix4x4 globalScale = Matrix4x4::Scale(Vector3f(globalScaleX, globalScaleY, 1.0f));
	Matrix4x4 renderMatrix = globalScale * worldOrigin * Matrix4x4::Translate(halfRenderSize) * Matrix4x4::Scale(renderSize);

	widget->SetWorldOriginMatrix(worldOrigin);
	widget->SetTransformMatrix(renderMatrix);

	bool childrenRectProvided = false;
	if (StackLayoutComponent* stack = registry.try_get<StackLayoutComponent>(entity))
	{
		LayoutStack(*stack, widget, localSize, hierarchyComp, registry);
		childrenRectProvided = true;
	}
	else if (GridLayoutComponent* grid = registry.try_get<GridLayoutComponent>(entity))
	{
		LayoutGrid(*grid, widget, localSize, hierarchyComp, registry);
		childrenRectProvided = true;
	}
	else if (ScrollBoxComponent* scrollBox = registry.try_get<ScrollBoxComponent>(entity))
	{
		LayoutScrollBox(*scrollBox, localSize, hierarchyComp, registry);
		childrenRectProvided = true;
	}

	entt::entity child = hierarchyComp->firstChild;
	while (child != entt::null)
	{
		WidgetComponent* childWidget = registry.try_get<WidgetComponent>(child);
		HierarchyComponent* childHierarchyComp = registry.try_get<HierarchyComponent>(child);
		if (childWidget != nullptr && childHierarchyComp != nullptr && childHierarchyComp->isActive)
		{
			UpdateUIWidgetTransform(child, childWidget, childHierarchyComp, registry, worldOrigin,
				localSize.x, localSize.y, globalScaleX, globalScaleY, childrenRectProvided);
		}
		child = childHierarchyComp != nullptr ? childHierarchyComp->nextSibling : entt::null;
	}
}

void SceneGraph::LayoutStack(const StackLayoutComponent& stack, WidgetComponent* containerWidget, Vector2f containerSize,
	HierarchyComponent* hierarchyComp, entt::registry& registry)
{
	PROFILE_FUNCTION();

	float padLeft = stack.paddingLeft;
	float padTop = stack.paddingTop;
	float padRight = stack.paddingRight;
	float padBottom = stack.paddingBottom;
	float spacingPx = stack.spacing;
	float crossSize = stack.horizontal ? (containerSize.y - padTop - padBottom) : (containerSize.x - padLeft - padRight);

	float cursor = stack.horizontal ? padLeft : padTop;
	bool first = true;

	entt::entity child = hierarchyComp->firstChild;
	while (child != entt::null)
	{
		WidgetComponent* childWidget = registry.try_get<WidgetComponent>(child);
		HierarchyComponent* childHierarchyComp = registry.try_get<HierarchyComponent>(child);
		if (childWidget != nullptr && childHierarchyComp != nullptr && childHierarchyComp->isActive)
		{
			if (!first)
				cursor += spacingPx;
			first = false;

			Vector2f desired = childWidget->size;
			if (stack.horizontal)
			{
				childWidget->position = Vector2f(cursor, padTop);
				childWidget->size = Vector2f(desired.x, stack.stretchCrossAxis ? crossSize : desired.y);
				cursor += childWidget->size.x;
			}
			else
			{
				childWidget->position = Vector2f(padLeft, cursor);
				childWidget->size = Vector2f(stack.stretchCrossAxis ? crossSize : desired.x, desired.y);
				cursor += childWidget->size.y;
			}
		}
		child = childHierarchyComp != nullptr ? childHierarchyComp->nextSibling : entt::null;
	}

	// Auto-size the stack's own main axis to fit its content (matching Godot's VBox/HBoxContainer) -
	// besides being generally expected, this is what lets a Stack inside a ScrollBox report a meaningful
	// contentSize (see LayoutScrollBox) instead of being stuck at whatever size it last had.
	if (stack.horizontal)
		containerWidget->size.x = cursor + padRight;
	else
		containerWidget->size.y = cursor + padBottom;
}

void SceneGraph::LayoutGrid(const GridLayoutComponent& grid, WidgetComponent* containerWidget, Vector2f containerSize,
	HierarchyComponent* hierarchyComp, entt::registry& registry)
{
	PROFILE_FUNCTION();

	int columns = std::max(1, grid.columns);
	float padLeft = grid.paddingLeft;
	float padTop = grid.paddingTop;
	float padRight = grid.paddingRight;
	float padBottom = grid.paddingBottom;
	float gapX = grid.cellSpacing.x;
	float gapY = grid.cellSpacing.y;
	float cellWidth = (containerSize.x - padLeft - padRight - (columns - 1) * gapX) / columns;

	std::vector<entt::entity> children;
	entt::entity c = hierarchyComp->firstChild;
	while (c != entt::null)
	{
		HierarchyComponent* childHierarchyComp = registry.try_get<HierarchyComponent>(c);
		if (registry.try_get<WidgetComponent>(c) != nullptr && childHierarchyComp != nullptr && childHierarchyComp->isActive)
			children.push_back(c);
		c = childHierarchyComp != nullptr ? childHierarchyComp->nextSibling : entt::null;
	}

	if (children.empty())
	{
		containerWidget->size.y = padTop + padBottom;
		return;
	}

	int rows = (int)((children.size() + columns - 1) / columns);
	std::vector<float> rowHeights(rows, grid.fixedRowHeight > 0.0f ? grid.fixedRowHeight : 0.0f);
	if (grid.fixedRowHeight <= 0.0f)
	{
		for (size_t i = 0; i < children.size(); i++)
		{
			WidgetComponent* childWidget = registry.try_get<WidgetComponent>(children[i]);
			rowHeights[i / columns] = std::max(rowHeights[i / columns], childWidget->size.y);
		}
	}

	float y = padTop;
	for (int row = 0; row < rows; row++)
	{
		float x = padLeft;
		for (int col = 0; col < columns; col++)
		{
			size_t idx = (size_t)row * columns + col;
			if (idx >= children.size())
				break;

			WidgetComponent* childWidget = registry.try_get<WidgetComponent>(children[idx]);
			childWidget->position = Vector2f(x, y);
			if (grid.uniformCellSize)
				childWidget->size = Vector2f(cellWidth, rowHeights[row]);
			x += cellWidth + gapX;
		}
		y += rowHeights[row] + gapY;
	}

	// Auto-size the grid's own height to fit its rows (see LayoutStack for why) - width stays whatever
	// the grid's own anchors/fixedWidth already gave it, since column width is derived from that.
	containerWidget->size.y = y - gapY + padBottom;
}

void SceneGraph::LayoutScrollBox(ScrollBoxComponent& scrollBox, Vector2f containerSize, HierarchyComponent* hierarchyComp,
	entt::registry& registry)
{
	PROFILE_FUNCTION();

	float maxScrollX = std::max(0.0f, scrollBox.contentSize.x - containerSize.x);
	float maxScrollY = std::max(0.0f, scrollBox.contentSize.y - containerSize.y);
	scrollBox.scrollOffset.x = scrollBox.horizontalScroll ? std::clamp(scrollBox.scrollOffset.x, 0.0f, maxScrollX) : 0.0f;
	scrollBox.scrollOffset.y = scrollBox.verticalScroll ? std::clamp(scrollBox.scrollOffset.y, 0.0f, maxScrollY) : 0.0f;

	entt::entity child = hierarchyComp->firstChild;
	if (child != entt::null)
	{
		if (WidgetComponent* childWidget = registry.try_get<WidgetComponent>(child))
		{
			childWidget->position = Vector2f(-scrollBox.scrollOffset.x, -scrollBox.scrollOffset.y);
			scrollBox.contentSize = childWidget->size;
		}
	}
}
