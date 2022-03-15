#include "stdafx.h"
#include "Entity.h"

Entity::Entity(entt::entity handle, Scene* scene, const std::string& name)
	:m_EntityHandle(handle), m_Scene(scene), m_DebugName(name)
{
}

Entity::Entity(entt::entity handle, Scene* scene)
	:m_EntityHandle(handle), m_Scene(scene)
{
	m_DebugName = m_Scene->GetRegistry().get<NameComponent>(handle);
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
