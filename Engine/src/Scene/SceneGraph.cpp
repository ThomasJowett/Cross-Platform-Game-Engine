#include "stdafx.h"
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

	registry.view<TransformComponent, HierarchyComponent>().each(
		[&]([[maybe_unused]] const auto entity, auto& transformComp, auto& hierarchyComp)
		{
			// if the entity is active and a root node
			if (hierarchyComp.isActive && hierarchyComp.parent == entt::null)
			{
				UpdateTransform(&transformComp, &hierarchyComp, registry);
			}
		});
}

void SceneGraph::Reparent(Entity entity, Entity parent)
{
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

		if(currentHierachyComp)
			currentHierachyComp->nextSibling = entity.GetHandle();
		hierarchyComp.previousSibling = previousSibling;
	}
}

void SceneGraph::Unparent(Entity entity)
{
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
	registry.destroy(entity);
}

std::vector<Entity> SceneGraph::GetChildren(Entity entity)
{
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