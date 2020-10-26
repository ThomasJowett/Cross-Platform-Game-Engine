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
	m_Dirty = true;
	return entity;
}

bool Scene::RemoveEntity(const Entity& entity)
{
	if (entity.BelongsToScene(this))
	{
		m_Registry.destroy(entity);
	}
	m_Dirty = true;
	return false;
}

void Scene::OnUpdate(float deltaTime)
{
	m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
		{
			if (nsc.InstantiateScript != nullptr)
			{
				if (!nsc.Instance)
				{
					nsc.Instance = nsc.InstantiateScript(nsc.Name);
					nsc.Instance->m_Entity = Entity{ entity, this };
					nsc.Instance->OnCreate();
				}

				nsc.Instance->OnUpdate(deltaTime);
			}
			else
				ENGINE_ERROR("Native Script component was added but no scriptable entity was bound");
		});

	Matrix4x4 view;
	Matrix4x4 projection;

	m_Registry.view<TransformComponent, CameraComponent>().each(
		[&]([[maybe_unused]] const auto cameraEntity, const auto& transformComp, const auto& cameraComp)
		{
			if (cameraComp.Primary)
			{
				view = Matrix4x4::Translate(transformComp.Position) * Matrix4x4::Rotate({ transformComp.Rotation });
				projection = cameraComp.Camera.GetProjectionMatrix();
			}
		}
	);

	Renderer::BeginScene(view, projection);

	m_Registry.group<StaticMeshComponent>(entt::get<TransformComponent>).each(
		[](const auto& mesh, const auto& transformComp)
		{
			//Matrix4x4 transform = Matrix4x4::Compose(transformComp.Position, transformComp.Rotation, transformComp.Scale);
			Matrix4x4 transform = Matrix4x4::Translate(transformComp.Position) * Matrix4x4::Rotate({ transformComp.Rotation }) * Matrix4x4::Scale(transformComp.Scale);
			Renderer::Submit(mesh.material, mesh.Geometry, transform);
		});

	Renderer::EndScene();

	Renderer2D::BeginScene(view, projection);

	m_Registry.group<SpriteComponent>(entt::get<TransformComponent>).each(
		[](const auto& sprite, const auto& transformComp)
		{
			Matrix4x4 transform = Matrix4x4::Translate(transformComp.Position) * Matrix4x4::Rotate({ transformComp.Rotation }) * Matrix4x4::Scale(transformComp.Scale);
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

void Scene::Save(std::filesystem::path filepath, bool binary)
{
	PROFILE_FUNCTION();

	std::filesystem::path finalPath = filepath;
	finalPath.replace_extension(".scene");

	ENGINE_INFO("Saving Scene {0} to {1}", m_SceneName, finalPath.string());

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
			entt::snapshot(m_Registry).entities(output).component<COMPONENTS>(output);
		}
		file.close();
	}
	else
	{
		std::stringstream ss;
		{
			cereal::JSONOutputArchive output{ ss };
			entt::snapshot{ m_Registry }.entities(output).component<COMPONENTS>(output);
		}

		std::ofstream file(finalPath);
		file << ss.str();
		file.close();
	}

	m_Dirty = false;
}

void Scene::Save(bool binary)
{
	Save(Application::GetOpenDocumentDirectory() / m_Filepath, binary);
}

bool Scene::Load(bool binary)
{
	std::filesystem::path filepath = Application::GetOpenDocumentDirectory() / m_Filepath;

	if (!std::filesystem::exists(filepath))
	{
		ENGINE_ERROR("File not found {0}", filepath);
		return false;
	}

	if (binary)
	{
		std::ifstream file(filepath, std::ios::binary);
		cereal::BinaryInputArchive input(file);
		entt::snapshot_loader(m_Registry).entities(input).component<COMPONENTS>(input);
		file.close();
	}
	else
	{
		try
		{
			std::ifstream file(filepath);
			cereal::JSONInputArchive input(file);
			entt::snapshot_loader(m_Registry).entities(input).component<COMPONENTS>(input);
			file.close();
		}
		catch (const std::exception& ex)
		{
			ENGINE_ERROR("Failed to load scene. Exception thrown: {0}", ex.what());
			return false;
		}
	}

	m_Dirty = false;
	return true;
}