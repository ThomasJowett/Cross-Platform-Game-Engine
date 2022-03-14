#include "stdafx.h"
#include "SceneGraph.h"
#include "Components/Components.h"

void SceneGraph::Traverse(entt::registry& registry)
{
	PROFILE_FUNCTION();

	auto nonHierarchyView = registry.view<TransformComponent>(entt::exclude<HierarchyComponent>);

	for (auto entity : nonHierarchyView)
	{
		registry.get<TransformComponent>(entity).SetWorldMatrix(Matrix4x4());
	}

	registry.view<TransformComponent, HierarchyComponent>().each(
		[&]([[maybe_unused]] const auto Entity, auto& transformComp, auto& hierarchyComp)
		{
			// if the entity is active and a root node
			if (hierarchyComp.isActive && hierarchyComp.parent == entt::null)
			{
				UpdateTransform(&transformComp, &hierarchyComp, registry);
			}
		});
}

void SceneGraph::Reparent(Entity entity, Entity parent, entt::registry& registry)
{
	HierarchyComponent& hierarchyComp = entity.GetOrAddComponent<HierarchyComponent>();

	hierarchyComp.parent = entt::null;
	hierarchyComp.nextSibling = entt::null;
	hierarchyComp.previousSibling = entt::null;

	if(parent.GetHandle() != entt::null)
	{
		hierarchyComp.parent = parent.GetHandle();

		HierarchyComponent& parentHierarchyComp = parent.GetOrAddComponent<HierarchyComponent>();
		if(parentHierarchyComp.firstChild == entt::null)
		{
			parentHierarchyComp.firstChild = entity.GetHandle();
		}
		else
		{
			entt::entity previousSibling = parentHierarchyComp.firstChild;
			HierarchyComponent* currentHierachyComp = registry.try_get<HierarchyComponent>(previousSibling);
			while (currentHierachyComp != nullptr && currentHierachyComp->nextSibling != entt::null)
			{
				previousSibling = currentHierachyComp->nextSibling;
				currentHierachyComp = registry.try_get<HierarchyComponent>(previousSibling);
			}
			
			currentHierachyComp->nextSibling = entity.GetHandle();
			hierarchyComp.previousSibling = previousSibling;
		}
	}
}

void SceneGraph::UpdateTransform(TransformComponent* transformComp, HierarchyComponent* hierarchyComp, entt::registry& registry)
{
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
