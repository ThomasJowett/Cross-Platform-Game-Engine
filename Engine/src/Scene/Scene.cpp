#include "stdafx.h"
#include "Scene.h"
#include "Entity.h"

#include "Components/Components.h"
#include "Renderer/Renderer2D.h"
#include "Renderer/Renderer.h"
#include "Renderer/Camera.h"

#include "cereal/archives/binary.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/types/string.hpp"

Scene::Scene(std::filesystem::path filepath)
	:m_Filepath(filepath)
{
	m_SceneName = filepath.filename().string();
	m_SceneName = m_SceneName.substr(0, m_SceneName.find_last_of('.'));
}

Scene::~Scene()
{
}

Entity Scene::CreateEntity(const std::string& name)
{
	Entity entity(m_Registry.create(), this, name);
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

	Camera orthoCamera;
	Camera perspectiveCamera;

	Matrix4x4 view;
	Matrix4x4 projection;

	m_Registry.view<TransformComponent, CameraComponent>().each(
		[&]([[maybe_unused]] const auto cameraEntity, const auto& transform, const auto& cameraComp)
		{
			if (cameraComp.Primary)
			{
				view = transform;
				projection = cameraComp.Camera.GetProjectionMatrix();
			}
		}
	);

	Renderer::BeginScene(view, projection);

	m_Registry.group<StaticMeshComponent>(entt::get<TransformComponent>).each(
		[](const auto& mesh, const auto& transform)
		{
			Renderer::Submit(mesh.material, mesh.Geometry, transform);
		});

	Renderer::EndScene();

	Renderer2D::BeginScene(orthoCamera);

	m_Registry.group<SpriteComponent>(entt::get<TransformComponent>).each(
		[](const auto& sprite, const auto& transform)
		{
			Renderer2D::DrawQuad(transform, sprite.Tint);
		});

	Renderer2D::EndScene();
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

void Scene::Serialise(std::filesystem::path filepath, bool binary)
{
	PROFILE_FUNCTION();

	ENGINE_INFO("Saving Scene {0} to {1}", m_SceneName, m_Filepath.string());

	std::filesystem::path finalPath = filepath;
	finalPath.replace_extension(".scene");

	if (!std::filesystem::exists(finalPath))
	{
		if (!std::filesystem::exists(filepath.remove_filename()))
		{
			CLIENT_INFO("Creating Directory {0}", filepath);
			std::filesystem::create_directory(filepath.remove_filename());
		}
	}

	if (binary)
	{
		std::ofstream file(finalPath, std::ios::binary);
		
		{
			cereal::BinaryOutputArchive output(file);
			output(*this);
			entt::snapshot(m_Registry).entities(output).component<TransformComponent, TagComponent, CameraComponent, SpriteComponent>(output);
		}
		file.close();
	}
	else
	{
		std::stringstream ss;
		{
			cereal::JSONOutputArchive output{ ss };
			output(*this);
			entt::snapshot{ m_Registry }.entities(output).component<TransformComponent, TagComponent, CameraComponent, SpriteComponent>(output);
		}

		std::ofstream file(finalPath);
		file << ss.str();
		file.close();
	}

	m_Dirty = false;
}

void Scene::Serialise(bool binary)
{
	Serialise(m_Filepath, binary);
}

void Scene::Deserialise(bool binary)
{
	if (!std::filesystem::exists(m_Filepath))
	{
		ENGINE_ERROR("File not found {0}", m_Filepath);
		return;
	}
	
	if (binary)
	{
		std::ifstream file(m_Filepath, std::ios::binary);
		cereal::BinaryInputArchive input(file);
		input(*this);
		entt::snapshot_loader(m_Registry).entities(input).component<TransformComponent, TagComponent, CameraComponent, SpriteComponent>(input);
		file.close();
	}
	else
	{
		std::ifstream file(m_Filepath);
		cereal::JSONInputArchive input(file);
		input(*this);
		entt::snapshot_loader(m_Registry).entities(input).component<TransformComponent, TagComponent, CameraComponent, SpriteComponent>(input);
		file.close();
	}

	m_Dirty = false;
}
