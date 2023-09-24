#pragma once

#include "Scene.h"
#include "EnTT/entt.hpp"

#include "Components/TransformComponent.h"
#include "Components/NameComponent.h"
#include "Components/IDComponent.h"
#include "Utilities/StringUtils.h"

class Entity
{
public:
	// Construct a new Empty Entity
	Entity() = default;

	// Construct a new Entity object
	Entity(entt::entity handle, Scene* scene, const std::string& debugName);

	// Construct a new Entity object
	Entity(entt::entity handle, Scene* scene);

	// Construct a new Entity object from another
	Entity(const Entity& other) = default;

	// Destroy the Entity object
	~Entity() = default;

	// Add a component to this entity
	template<typename T, typename... Args>
	T& AddComponent(Args&&... args)
	{
		CORE_ASSERT(!HasComponent<T>(), "Entity already has component of type " + std::string(type_name<T>().data()));
		m_Scene->MakeDirty();
		return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
	}

	// Add or replace a component of this entity
	template<typename T, typename... Args>
	T& AddOrReplaceComponent(Args&&... args)
	{
		m_Scene->MakeDirty();
		return m_Scene->m_Registry.emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
	}

	void AddChild(Entity child);

	void Destroy();

	// Get a reference to a Component of this Entity
	template<typename T>
	T& GetComponent()
	{
		CORE_ASSERT(IsSceneValid(), "Entity not valid");
		CORE_ASSERT(HasComponent<T>(), "Entity does not have component of type " + std::string(type_name<T>().data()));
		return m_Scene->m_Registry.get<T>(m_EntityHandle);
	}

	template<typename... T>
	std::tuple<T&...> GetComponents()
	{
		return m_Scene->m_Registry.get<T...>(m_EntityHandle);
	}

	// Get a reference to a Component of this Entity creating if needed
	template<typename T, typename... Args>
	T& GetOrAddComponent(Args&&... args)
	{
		if (!HasComponent<T>())
			m_Scene->MakeDirty();
		return m_Scene->GetRegistry().get_or_emplace<T>(m_EntityHandle, std::forward<args>(args)...);
	}

	// Tries to get the component
	template<typename T>
	T* TryGetComponent()
	{
		ASSERT(IsSceneValid(), "Invalid Entity!")
		return m_Scene->m_Registry.try_get<T>(m_EntityHandle);
	}

	// Get the name
	std::string& GetName();
	void SetName(const std::string_view name);

	// Get the ID component
	Uuid GetID();

	// Get the entt handle
	entt::entity GetHandle() const;

	// Get the scene this entity belongs to
	Scene* GetScene() { return m_Scene; }

	Entity GetParent();
	Entity GetSibling();
	Entity GetChild();

	// Find if this Entity has a Component of certain type
	template<typename... T>
	bool HasComponent() const
	{
		return m_Scene->m_Registry.any_of<T...>(m_EntityHandle);
	}

	// If the Entity has this component, then remove it
	template<typename T>
	void RemoveComponent()
	{
		CORE_ASSERT(HasComponent<T>(), "Entity does not have component of type " + std::string(type_name<T>().data()));
		m_Scene->m_Registry.remove<T>(m_EntityHandle);
		m_Scene->MakeDirty();
	}

	// Does this entity belong to the given scene
	bool BelongsToScene(Scene* scene) const
	{
		return scene == m_Scene && IsSceneValid();
	}

	// Is the scene reference that this entity has a valid reference
	bool IsSceneValid() const
	{
		return m_Scene && m_Scene->m_Registry.valid(m_EntityHandle);
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
#ifdef DEBUG
	std::string m_DebugName;
#endif
};
