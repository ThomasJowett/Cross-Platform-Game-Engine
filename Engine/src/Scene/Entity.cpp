#include "stdafx.h"
#include "Entity.h"

Entity::Entity(entt::entity handle, Scene* scene, const std::string& name)
	:m_EntityHandle(handle), m_Scene(scene),m_DebugName(name)
{
}

TransformComponent& Entity::GetTransform()
{
	return GetComponent<TransformComponent>();
}

std::string& Entity::GetTag()
{
	return GetComponent<TagComponent>().Tag;
}

Uuid Entity::GetID()
{
	return GetComponent<IDComponent>().ID;
}

entt::entity Entity::GetHandle()
{
	return m_EntityHandle;
}
