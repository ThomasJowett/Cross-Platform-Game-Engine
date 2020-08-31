#include "stdafx.h"
#include "Scene.h"
#include "Entity.h"

#include "Components/Components.h"
#include "Renderer/Renderer2D.h"
#include "Renderer/Renderer.h"

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
	{
		m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
			{
				if (!nsc.Instance)
				{
					nsc.InstantiateFunction();
					nsc.Instance->m_Entity = Entity{ entity, this };
					nsc.OnCreateFunction(nsc.Instance);
				}

				nsc.OnUpdateFunction(nsc.Instance, deltaTime);
			});
	}
	auto CameraView = m_Registry.view<TransformComponent, CameraComponent>();
	for (auto entity : CameraView)
	{
		auto& [transform, camera] = CameraView.get<TransformComponent, CameraComponent>(entity);

		if (camera.Primary)
		{
			Renderer::BeginScene(camera.Camera);
			break;
		}
	}

	{
		auto group = m_Registry.group<SpriteComponent>(entt::get<TransformComponent>);
		for (auto entity : group)
		{
			const auto& [transform, sprite] = group.get<TransformComponent, SpriteComponent>(entity);
	
			Renderer2D::DrawQuad(transform, sprite.Tint);
		}
	}

	{
		auto group = m_Registry.group<MeshComponent>(entt::get<TransformComponent>);
		for (auto entity : group)
		{
			const auto& [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);

			Renderer::Submit(mesh.material, mesh.Geometry.GetVertexArray(), transform);
		}
	}

	Renderer::EndScene();
}

void Scene::OnViewportResize(uint32_t width, uint32_t height)
{
	m_ViewportHeight = height;
	m_ViewportWidth = width;

	// Resize non fixed aspect ratio cameras
	auto view = m_Registry.view<CameraComponent>();
	for (auto entity : view)
	{
		CameraComponent& camera = view.get<CameraComponent>(entity);
		if (!camera.FixedAspectRatio)
		{
			camera.Camera.SetAspectRatio((float)width/ (float)height);
		}
		
	}
}
