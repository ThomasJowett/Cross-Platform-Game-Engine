#include "stdafx.h"
#include "Entity.h"

Entity::Entity(entt::entity handle, Scene* scene, const std::string& name)
	:m_EntityHandle(handle), m_Scene(scene),m_DebugName(name)
{
}
