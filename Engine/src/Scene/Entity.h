#pragma once

#include "Scene.h"

#include "EnTT/entt.hpp"

class Entity
{
public:
	Entity() = default;
	Entity(entt::entity handle, Scene* scene, const std::string& debugName = "");
	Entity(const Entity& other) = default;
	~Entity() = default;

	template<typename T, typename... Args>
	T& AddComponent(Args&&... args)
	{
		CORE_ASSERT(!HasComponent<T>(), "Entity already has this component");
		return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
	}

	template<typename T>
	T& GetComponent()
	{
		CORE_ASSERT(HasComponent<T>(), "Entity does not have this component");
		return m_Scene->m_Registry.get<T>(m_EntityHandle);
	}
	template<typename T>
	bool HasComponent()
	{
		return m_Scene->m_Registry.has<T>(m_EntityHandle);
	}

	template<typename T>
	void RemoveComponent()
	{
		CORE_ASSERT(HasComponent<T>(), "Entity does not have this component");
		m_Scene->m_Registry.remove<T>(m_EntityHandle);
	}

	bool BelongsToScene(Scene* scene) const
	{
		return scene == m_Scene;
	}

	operator bool() const { return m_EntityHandle != entt::null; }
	operator uint32_t() const { return (uint32_t)m_EntityHandle; }
	operator entt::entity() const { return m_EntityHandle; }

	bool operator==(const Entity& other)const 
	{ 
		return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene; 
	}

	bool operator!=(const Entity& other)const
	{
		return m_EntityHandle != other.m_EntityHandle || m_Scene != other.m_Scene;
	}
private:
	entt::entity m_EntityHandle{ entt::null };
	Scene* m_Scene = nullptr;
	std::string m_DebugName;
};