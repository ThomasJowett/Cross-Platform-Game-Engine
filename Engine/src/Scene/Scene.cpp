#include "stdafx.h"
#include "Scene.h"
#include "Entity.h"

#include "Components/Components.h"
#include "Renderer/Renderer2D.h"

Scene::Scene()
{
	entt::entity entity = m_Registry.create();
}

Scene::~Scene()
{
}

Entity Scene::CreateEntity(const std::string& name)
{
	Entity entity(m_Registry.create(), this);
	entity.AddComponent<TransformComponent>();
	entity.AddComponent<TagComponent>(name.empty() ? "Unnamed Entity" : name);
	return entity;
}

void Scene::OnUpdate(float deltaTime)
{
	auto group = m_Registry.group<TransformComponent>(entt::get<SpriteComponent>);
	for (auto entity : group)
	{
		auto& [transform, sprite] = group.get<TransformComponent, SpriteComponent>(entity);

		Renderer2D::DrawQuad(transform, sprite.Tint);
	}
}
