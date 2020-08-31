#pragma once

#include "Entity.h"

class ScriptableEntity
{
public:
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
private:
	Entity m_Entity;

	friend class Scene;
};