#include "HistoryCommands.h"
#include "Scene/SceneSerializer.h"
#include "Scene/SceneGraph.h"

AddEntityCommand::AddEntityCommand(Entity& entity)
	:m_AddedEntity(entity)
{
	if (auto hierarchyComp = m_AddedEntity.TryGetComponent<HierarchyComponent>())
	{
		if (hierarchyComp->parent != entt::null)
		{
			m_ParentEntity = Entity(hierarchyComp->parent, m_AddedEntity.GetScene());
		}
	}
}

void AddEntityCommand::Undo()
{
	SceneGraph::Remove(m_AddedEntity);
}

void AddEntityCommand::Redo()
{
	m_AddedEntity = SceneSerializer::DeserializeEntity(m_AddedEntity.GetScene(), m_EntityPrefab, false);
	if (m_ParentEntity)
		SceneGraph::Reparent(m_AddedEntity, m_ParentEntity);
}

void AddEntityCommand::End()
{
	m_EntityPrefab = SceneSerializer::SerializeEntity(m_AddedEntity);
}

RemoveEntityCommand::RemoveEntityCommand(Entity& entity)
	:m_RemovedEntity(entity)
{
	m_EntityPrefab = SceneSerializer::SerializeEntity(m_RemovedEntity);
	if (auto hierarchyComp = m_RemovedEntity.TryGetComponent<HierarchyComponent>())
	{
		if (hierarchyComp->parent != entt::null)
		{
			m_ParentEntity = Entity(hierarchyComp->parent, m_RemovedEntity.GetScene());
		}
	}
}

void RemoveEntityCommand::Undo()
{
	m_RemovedEntity = SceneSerializer::DeserializeEntity(m_RemovedEntity.GetScene(), m_EntityPrefab, false);
	if (m_ParentEntity)
		SceneGraph::Reparent(m_RemovedEntity, m_ParentEntity);
}

void RemoveEntityCommand::Redo()
{
	SceneGraph::Remove(m_RemovedEntity);
}

void RemoveEntityCommand::End()
{
}

ReparentEntityCommand::ReparentEntityCommand(Entity& entity, Entity newParent)
	:m_Entity(entity), m_NewParent(newParent)
{
	if (auto hierarchyComp = entity.TryGetComponent<HierarchyComponent>())
	{
		if (hierarchyComp->parent != entt::null)
			m_OldParent = Entity(hierarchyComp->parent, m_Entity.GetScene());
		else
			m_OldParent = Entity();
	}
}

void ReparentEntityCommand::Undo()
{
	if (m_OldParent)
		SceneGraph::Reparent(m_Entity, m_OldParent);
	else
		SceneGraph::Unparent(m_Entity);
}

void ReparentEntityCommand::Redo()
{
	if (m_NewParent)
		SceneGraph::Reparent(m_Entity, m_NewParent);
	else
		SceneGraph::Unparent(m_Entity);
}

void ReparentEntityCommand::End()
{
}

MoveSiblingCommand::MoveSiblingCommand(Entity& entity, Entity newAnchor, bool newIsBefore)
	:m_Entity(entity), m_NewAnchor(newAnchor), m_NewIsBefore(newIsBefore)
{
	if (auto hierarchyComp = m_Entity.TryGetComponent<HierarchyComponent>())
	{
		if (hierarchyComp->parent != entt::null)
			m_OldParent = Entity(hierarchyComp->parent, m_Entity.GetScene());
		if (hierarchyComp->previousSibling != entt::null)
			m_OldPreviousSibling = Entity(hierarchyComp->previousSibling, m_Entity.GetScene());
		if (hierarchyComp->nextSibling != entt::null)
			m_OldNextSibling = Entity(hierarchyComp->nextSibling, m_Entity.GetScene());
	}
}

void MoveSiblingCommand::Undo()
{
	if (m_OldPreviousSibling)
		SceneGraph::MoveAfter(m_Entity, m_OldPreviousSibling);
	else if (m_OldNextSibling)
		SceneGraph::MoveBefore(m_Entity, m_OldNextSibling);
	else if (m_OldParent)
		SceneGraph::Reparent(m_Entity, m_OldParent);
	else
		SceneGraph::Unparent(m_Entity);
}

void MoveSiblingCommand::Redo()
{
	if (m_NewIsBefore)
		SceneGraph::MoveBefore(m_Entity, m_NewAnchor);
	else
		SceneGraph::MoveAfter(m_Entity, m_NewAnchor);
}

void MoveSiblingCommand::End()
{
}
