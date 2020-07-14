#include "stdafx.h"
#include "Scene.h"

#include "math/Matrix.h"

Scene::Scene()
{
	struct TransformComponent
	{
		Matrix4x4 Transform;

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const Matrix4x4& transform)
			:Transform(transform) {}

		operator const Matrix4x4& () { return Transform; }
	};
	entt::entity entity = m_Registry.create();

	m_Registry.emplace<TransformComponent>(entity);

	auto view = m_Registry.view<TransformComponent>();
	for (auto entity : view)
	{

	}
}

Scene::~Scene()
{
}
