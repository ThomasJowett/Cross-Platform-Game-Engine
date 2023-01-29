#include "stdafx.h"
#include "Entity.h"
#include "SceneGraph.h"

Entity::Entity(entt::entity handle, Scene* scene, const std::string& name)
	:m_EntityHandle(handle), m_Scene(scene)
#ifdef DEBUG
	, m_DebugName(name)
#endif // DEBUG
{
	ASSERT(IsSceneValid(), "This entity is not valid!")
}

Entity::Entity(entt::entity handle, Scene* scene)
	:m_EntityHandle(handle), m_Scene(scene)
{
	ASSERT(IsSceneValid(), "This entity is not valid!")

#ifdef DEBUG
		m_DebugName = m_Scene->GetRegistry().get<NameComponent>(handle);
#endif // DEBUG
}

void Entity::AddChild(Entity child)
{
	SceneGraph::Reparent(child, *this);
}

void Entity::Destroy()
{
	m_Scene->RemoveEntity(*this);
}

TransformComponent& Entity::GetTransform()
{
	return GetComponent<TransformComponent>();
}

std::string& Entity::GetName()
{
	return GetComponent<NameComponent>().name;
}

void Entity::SetName(const std::string_view name)
{
	GetComponent<NameComponent>().name = name;
}

Uuid Entity::GetID()
{
	return GetComponent<IDComponent>().ID;
}

entt::entity Entity::GetHandle() const
{
	return m_EntityHandle;
}

Entity Entity::GetParent()
{
	PROFILE_FUNCTION();
	HierarchyComponent* hierarchyComp = TryGetComponent<HierarchyComponent>();
	if (hierarchyComp && hierarchyComp->parent != entt::null)
		return Entity(hierarchyComp->parent, m_Scene);
	return Entity();
}

Entity Entity::GetSibling()
{
	PROFILE_FUNCTION();
	HierarchyComponent* hierarchyComp = TryGetComponent<HierarchyComponent>();
	if (hierarchyComp && hierarchyComp->nextSibling != entt::null)
		return Entity(hierarchyComp->nextSibling, m_Scene);
	return Entity();
}

Entity Entity::GetChild()
{
	PROFILE_FUNCTION();
	HierarchyComponent* hierarchyComp = TryGetComponent<HierarchyComponent>();
	if (hierarchyComp && hierarchyComp->firstChild != entt::null)
		return Entity(hierarchyComp->firstChild, m_Scene);
	return Entity();
}
