#include "stdafx.h"
#include "Entity.h"
#include "SceneGraph.h"

Entity::Entity(entt::entity handle, Scene* scene, const std::string& name)
	:m_EntityHandle(handle), m_Scene(scene)
#ifdef DEBUG
	, m_DebugName(name)
#endif // DEBUG
{
	ASSERT(IsValid(), "This entity is not valid!");
}

Entity::Entity(entt::entity handle, Scene* scene)
	:m_EntityHandle(handle), m_Scene(scene)
{
	ASSERT(IsValid(), "This entity is not valid!");

#ifdef DEBUG
	m_DebugName = m_Scene->GetRegistry().get<NameComponent>(handle);
#endif // DEBUG
}

void Entity::AddChild(Entity child)
{
	SceneGraph::Reparent(child, *this, m_Scene->GetRegistry());
}

TransformComponent& Entity::GetTransform()
{
	return GetComponent<TransformComponent>();
}

std::string& Entity::GetName()
{
	return GetComponent<NameComponent>().name;
}

Uuid Entity::GetID()
{
	return GetComponent<IDComponent>().ID;
}

entt::entity Entity::GetHandle()
{
	return m_EntityHandle;
}
