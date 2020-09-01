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
	m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
		{
			if (!nsc.Instance)
			{
				nsc.Instance = nsc.InstantiateScript();
				nsc.Instance->m_Entity = Entity{ entity, this };
				nsc.Instance->OnCreate();
			}

			nsc.Instance->OnUpdate(deltaTime);
		});

	m_Registry.view<TransformComponent, CameraComponent>().each(
		[&]([[maybe_unused]] const auto cameraEntity, const auto& transform, const auto& cameraComp)
		{
			if (cameraComp.Primary)
			{
				Renderer::BeginScene(cameraComp.Camera);
			}
		}
	);

	m_Registry.group<SpriteComponent>(entt::get<TransformComponent>).each(
		[](const auto& sprite, const auto& transform)
		{
			Renderer2D::DrawQuad(transform, sprite.Tint);
		});


	m_Registry.group<MeshComponent>(entt::get<TransformComponent>).each(
		[](const auto& mesh, const auto& transform)
		{
			Renderer::Submit(mesh.material, mesh.Geometry, transform);
		});

	Renderer::EndScene();
}

void Scene::OnViewportResize(uint32_t width, uint32_t height)
{
	m_ViewportHeight = height;
	m_ViewportWidth = width;

	// Resize non fixed aspect ratio cameras
	m_Registry.view<CameraComponent>().each(
		[=]([[maybe_unused]] const auto cameraEntity, auto& cameraComp)
		{
			if (!cameraComp.FixedAspectRatio)
			{
				cameraComp.Camera.SetAspectRatio((float)width / (float)height);
			}

		});
}
