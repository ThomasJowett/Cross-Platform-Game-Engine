#pragma once

#include "Scene.h"

#include "EnTT/entt.hpp"

#include "Components/TransformComponent.h"
#include "Components/NameComponent.h"
#include "Components/IDComponent.h"

class Entity
{
public:
	/**
	 * Construct a new Empty Entity
	 * 
	 */
	Entity() = default;

	/**
	 * Construct a new Entity object
	 * 
	 * @param handle entt id
	 * @param scene the entity belongs to
	 * @param debugName name of the entity used only for debugging
	 */
	Entity(entt::entity handle, Scene* scene, const std::string& debugName = "");

	/**
	 * Construct a new Entity object from another
	 * 
	 * @param other 
	 */
	Entity(const Entity& other) = default;

	/**
	 * Destroy the Entity object
	 * 
	 */
	~Entity() = default;

	/**
	 * Add a component to this entity
	 * 
	 * @tparam T 
	 * @tparam Args 
	 * @param args 
	 * @return T& 
	 */
	template<typename T, typename... Args>
	T& AddComponent(Args&&... args)
	{
		CORE_ASSERT(!HasComponent<T>(), "Entity already has component of type " + std::string(typeid(T).name()));
		m_Scene->MakeDirty();
		return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	T& AddOrReplaceComponent(Args&&... args)
	{
		m_Scene->MakeDirty();
		return m_Scene->m_Registry.emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
	}

	void AddChild(Entity child);

	/**
	 * Get a reference to a Component of this Entity
	 * 
	 * @tparam T type of component
	 * @return T& 
	 */
	template<typename T>
	T& GetComponent()
	{
		CORE_ASSERT(HasComponent<T>(), "Entity does not have component of type " + std::string(typeid(T).name()));
		return m_Scene->m_Registry.get<T>(m_EntityHandle);
	}

	// Get the transform component
	TransformComponent& GetTransform();

	// Get the name
	std::string& GetName();

	// Get the ID component
	Uuid GetID();

	// Get the entt handle
	entt::entity GetHandle();

	// Get the scene this entity belongs to
	Scene* GetScene();

	/**
	 * Find if this Entity has a Component of certain type
	 * 
	 * @tparam T type of Component
	 * @return true Entity has this component
	 * @return false Entity does not have this component
	 */
	template<typename T>
	bool HasComponent()
	{
		return m_Scene->m_Registry.has<T>(m_EntityHandle);
	}

	/**
	 * If the Entity has this component, then remove it
	 * 
	 * @tparam T 
	 */
	template<typename T>
	void RemoveComponent()
	{
		CORE_ASSERT(HasComponent<T>(), "Entity does not have component of type " + std::string(typeid(T).name()));
		m_Scene->m_Registry.remove<T>(m_EntityHandle);
		m_Scene->MakeDirty();
	}

	/**
	 * Does this entity belong to the given scene
	 * 
	 * @param scene 
	 * @return true 
	 * @return false 
	 */
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
