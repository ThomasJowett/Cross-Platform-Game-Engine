#include "stdafx.h"
#include "Scene.h"

#include "math/Matrix.h"

#include "Components/Components.h"
#include "Renderer/Renderer2D.h"

Scene::Scene()
{
	entt::entity entity = m_Registry.create();
}

Scene::~Scene()
{
}

entt::entity Scene::CreateEntity()
{
	return entt::entity();
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
