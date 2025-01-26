#pragma once
#include "HistoryManager.h"
#include "Scene/Entity.h"

class AddEntityCommand : public HistoryRecord
{
public:
	AddEntityCommand(Entity& entity);
	virtual void Undo() override;
	virtual void Redo() override;
	virtual void End() override;
private:
	Entity m_AddedEntity;
	Entity m_ParentEntity;
	std::string m_EntityPrefab;
};

class RemoveEntityCommand : public HistoryRecord
{
public:
	RemoveEntityCommand(Entity& entity);

	// Inherited via HistoryRecord
	virtual void Undo() override;
	virtual void Redo() override;
	virtual void End() override;

private:
	Entity m_RemovedEntity;
	Entity m_ParentEntity;
	std::string m_EntityPrefab;
};

class ReparentEntityCommand : public HistoryRecord
{
public:
	ReparentEntityCommand(Entity& entity, Entity newParent);

	// Inherited via HistoryRecord
	virtual void Undo() override;
	virtual void Redo() override;
	virtual void End() override;

private:
	Entity m_Entity;
	Entity m_NewParent;
	Entity m_OldParent;
};

template<typename T>
class AddComponentCommand : public HistoryRecord
{
public:
	AddComponentCommand(Entity& entity)
		:m_Entity(entity)
	{
	}

	// Inherited via HistoryRecord
	virtual void Undo() override
	{
		m_Entity.RemoveComponent<T>();
	}
	virtual void Redo() override
	{
		m_Entity.AddOrReplaceComponent<T>(m_NewComponent);
	}
	virtual void End() override
	{
		m_NewComponent = m_Entity.GetComponent<T>();
	}
private:
	Entity m_Entity;
	T m_NewComponent;
};

template<typename T>
class RemoveComponentCommand : public HistoryRecord
{
public:
	RemoveComponentCommand(Entity& entity)
		:m_Entity(entity)
	{
		m_RemovedComponent = m_Entity.GetComponent<T>();
	}

	// Inherited via HistoryRecord
	virtual void Undo() override
	{
		m_Entity.AddOrReplaceComponent<T>(m_RemovedComponent);
	}
	virtual void Redo() override
	{
		m_Entity.RemoveComponent<T>();
	}
	virtual void End() override
	{
	}

private:
	Entity m_Entity;
	T m_RemovedComponent;
};

template<typename T>
class EditComponentCommand : public HistoryRecord
{
	Entity m_Entity;
	T m_OldComponent;
	T m_NewComponent;

public:
	EditComponentCommand(Entity& entity)
		: m_Entity(entity)
	{
		// Store the original state of the component
		m_OldComponent = m_Entity.GetComponent<T>();
	}

	void End() {
		// Store the new state of the component
		m_NewComponent = m_Entity.GetComponent<T>();
	}

	void Undo() {
		// Replace the new entity with the old
		m_Entity.AddOrReplaceComponent<T>(m_OldComponent);
	}

	void Redo() {
		m_Entity.AddOrReplaceComponent<T>(m_NewComponent);
	}
};
