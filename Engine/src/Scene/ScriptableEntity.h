#pragma once

#include "Entity.h"

#include "Core/Factory.h"

class ScriptableEntity
{
public:
	ScriptableEntity() = default;
	virtual ~ScriptableEntity() {}

	template<typename T>
	T& GetComponent()
	{
		return m_Entity.GetComponent<T>();
	}
	template<typename T>
	bool HasComponent()
	{
		return m_Entity.HasComponent<T>();
	}
	
	template<typename T>
	void RemoveComponent()
	{
		m_Entity.RemoveComponent<T>();
	}
protected:
	//Called when the entity is created
	virtual void OnCreate() {}

	//Called when the entity is destroyed
	virtual void OnDestroy() {}

	//Called once per frame
	virtual void OnUpdate(float deltaTime) {}

	//Called 100 times a second
	virtual void OnFixedUpdate() {}
private:
	Entity m_Entity;

	friend class Scene;
};

template<typename T> ScriptableEntity* CreateT() { return new T; }

template<typename T>
struct ScriptRegister : Factory<ScriptableEntity>
{
	ScriptRegister(std::string const& name)
	{
		GetMap()->insert(std::make_pair(name, &CreateT<T>));
	}
};