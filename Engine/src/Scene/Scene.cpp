#include "stdafx.h"
#include "Scene.h"
#include "Entity.h"

#include "Components/Components.h"
#include "Renderer/Renderer2D.h"
#include "Renderer/Renderer.h"
#include "Renderer/Camera.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/FrameBuffer.h"

#include "Utilities/GeometryGenerator.h"
#include "Utilities/Box2DDebugDraw.h"

#include "cereal/archives/binary.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/types/string.hpp"

#include "Events/SceneEvent.h"

#include "Box2D/Box2D.h"
#include "SceneSerializer.h"
#include "SceneGraph.h"
#include "Scripting/Lua/LuaManager.h"

template<typename Component>
static void CopyComponentIfExists(entt::entity dst, entt::entity src, entt::registry& registry)
{
	if (registry.any_of<Component>(src))
	{
		auto& srcComponent = registry.get<Component>(src);
		registry.emplace_or_replace<Component>(dst, srcComponent);
	}
}

template<typename... Component>
static void CopyEntity(entt::entity dst, entt::entity src, entt::registry& registry)
{
	(CopyComponentIfExists<Component>(dst, src, registry), ...);
}

Scene::Scene(std::filesystem::path filepath)
	:m_Filepath(filepath)
{
	m_SceneName = filepath.filename().string();
	m_SceneName = m_SceneName.substr(0, m_SceneName.find_last_of('.'));
}

/* ------------------------------------------------------------------------------------------------------------------ */

Scene::Scene(std::string name)
	:m_SceneName(name)
{
}

/* ------------------------------------------------------------------------------------------------------------------ */

Scene::~Scene()
{
	LuaManager::CleanUp();
}

/* ------------------------------------------------------------------------------------------------------------------ */

Entity Scene::CreateEntity(const std::string& name)
{
	return CreateEntity(Uuid(), name);
}

/* ------------------------------------------------------------------------------------------------------------------ */

Entity Scene::CreateEntity(Uuid id, const std::string& name)
{
	Entity entity(m_Registry.create(), this, name);
	entity.AddComponent<IDComponent>(id);
	entity.AddComponent<NameComponent>(name.empty() ? "Unnamed Entity" : name);
	entity.AddComponent<TransformComponent>();
	m_Dirty = true;
	return entity;
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool Scene::RemoveEntity(Entity& entity)
{
	if (entity.BelongsToScene(this))
	{
		if (entity.HasComponent<HierarchyComponent>())
		{
			SceneGraph::Remove(entity, m_Registry);
		}
		else
		{
			ENGINE_DEBUG("Removed {0}", entity.GetName());
			m_Registry.destroy(entity);
		}
	}
	m_Dirty = true;
	return false;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Scene::DuplicateEntity(Entity entity, Entity parent)
{
	std::string name = entity.GetName();
	Entity newEntity = CreateEntity(name);

	CopyEntity<COMPONENTS>(newEntity.GetHandle(), entity.GetHandle(), m_Registry);
	if (newEntity.HasComponent<HierarchyComponent>())
	{
		HierarchyComponent& heirarchyComp = newEntity.GetComponent<HierarchyComponent>();

		if (!parent && heirarchyComp.parent != entt::null)
			parent = Entity(heirarchyComp.parent, this);

		heirarchyComp.firstChild = entt::null;
		heirarchyComp.parent = entt::null;
		heirarchyComp.nextSibling = entt::null;
		heirarchyComp.previousSibling = entt::null;
	}

	std::vector<Entity> children = SceneGraph::GetChildren(entity, m_Registry);

	for (auto& child : children)
	{
		DuplicateEntity(child, newEntity);
	}
	if (parent)
	{
		SceneGraph::Reparent(newEntity, parent, m_Registry);
	}
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Scene::OnRuntimeStart()
{
	PROFILE_FUNCTION();

	ENGINE_DEBUG("Runtime Start");
	if (m_Dirty)
		Save();

	std::stringstream().swap(m_Snapshot);
	cereal::JSONOutputArchive output(m_Snapshot);
	entt::snapshot(m_Registry).entities(output).component<COMPONENTS>(output);

	m_Box2DWorld = new b2World({ 0.0f, -9.81f });
	if (m_Box2DDraw)
		m_Box2DWorld->SetDebugDraw(m_Box2DDraw);

	m_Registry.view<TransformComponent, RigidBody2DComponent>().each(
		[&]([[maybe_unused]] const auto physicsEntity, auto& transformComp, auto& rigidBody2DComp)
		{
			Entity entity = { physicsEntity, this };

			// if an entity has physics it must not have a parent for the physics position to work correctly
			if (entity.HasComponent<HierarchyComponent>())
			{
				Vector3f position;
				Vector3f rotation;
				Vector3f scale;
				transformComp.GetWorldMatrix().Decompose(position, rotation, scale);
				SceneGraph::Unparent(entity, m_Registry);
				//transformComp.position = position;
				//transformComp.rotation = rotation;
				//transformComp.scale = scale;
			}

			rigidBody2DComp.Init(entity, m_Box2DWorld);
		});

	m_Registry.view<LuaScriptComponent>().each(
		[=](const auto entity, auto& scriptComponent)
		{
			std::optional<std::pair<int, std::string>> result = scriptComponent.ParseScript(Entity{ entity, this });
			if (result.has_value())
			{
				ENGINE_ERROR("Failed to parse lua script {0}({1}): {2}", scriptComponent.absoluteFilepath, result.value().first, result.value().second);
			}
		});
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Scene::OnRuntimePause()
{
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Scene::OnRuntimeStop()
{
	PROFILE_FUNCTION();

	if (m_Snapshot.rdbuf()->in_avail() != 0)
	{
		ENGINE_DEBUG("Runtime End");
		m_Registry = entt::registry();
		cereal::JSONInputArchive input(m_Snapshot);
		entt::snapshot_loader(m_Registry).entities(input).component<COMPONENTS>(input);
	}
	std::stringstream().swap(m_Snapshot);
	m_Dirty = false;

	LuaManager::CleanUp();

	if (m_Box2DWorld != nullptr) delete m_Box2DWorld;
	m_Box2DWorld = nullptr;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Scene::Render(Ref<FrameBuffer> renderTarget, const Matrix4x4& cameraTransform, const Matrix4x4& projection)
{
	PROFILE_FUNCTION();

	SceneGraph::Traverse(m_Registry);
	if (renderTarget != nullptr)
		renderTarget->Bind();

	Renderer::BeginScene(cameraTransform, projection);

	auto spriteGroup = m_Registry.view<TransformComponent, SpriteComponent>();
	for (auto entity : spriteGroup)
	{
		auto [transformComp, spriteComp] = spriteGroup.get(entity);
		Renderer2D::DrawSprite(transformComp.GetWorldMatrix(), spriteComp, (int)entity);
	}

	auto animatedSpriteGroup = m_Registry.view<TransformComponent, AnimatedSpriteComponent>();
	for (auto entity : animatedSpriteGroup)
	{
		auto [transformComp, spriteComp] = animatedSpriteGroup.get(entity);
		if (spriteComp.tileset)
			Renderer2D::DrawQuad(transformComp.GetWorldMatrix(), spriteComp.tileset->GetSubTexture(), spriteComp.tint, (int)entity);
	}

	auto circleGroup = m_Registry.view<TransformComponent, CircleRendererComponent>();
	for (auto entity : circleGroup)
	{
		auto [transformComp, circleComp] = circleGroup.get(entity);
		Renderer2D::DrawCircle(transformComp.GetWorldMatrix(), circleComp, (int)entity);
	}

	auto staticMeshGroup = m_Registry.view<TransformComponent, StaticMeshComponent>();
	for (auto entity : staticMeshGroup)
	{
		auto [transformComp, staticMeshComp] = staticMeshGroup.get(entity);
		Renderer::Submit(staticMeshComp.material, staticMeshComp.mesh, transformComp.GetWorldMatrix(), (int)entity);
	}

	auto tilemapGroup = m_Registry.view<TransformComponent, TilemapComponent>();
	for (auto entity : tilemapGroup)
	{
		auto [transformComp, tilemapComp] = tilemapGroup.get(entity);
		Renderer2D::DrawTilemap(transformComp.GetWorldMatrix(), tilemapComp, (int)entity);
	}

	if (m_Box2DDraw && m_Box2DWorld)
		m_Box2DWorld->DrawDebugData();

	Renderer::EndScene();

	if (renderTarget != nullptr)
		renderTarget->UnBind();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Scene::Render(Ref<FrameBuffer> renderTarget)
{
	PROFILE_FUNCTION();

	SceneGraph::Traverse(m_Registry);
	Matrix4x4 view;
	Matrix4x4 projection;

	Entity cameraEntity = GetPrimaryCameraEntity();

	if (cameraEntity)
	{
		auto [cameraComp, transformComp] = cameraEntity.GetComponents<CameraComponent, TransformComponent>();
		view = Matrix4x4::Translate(transformComp.GetWorldPosition()) * Matrix4x4::Rotate(Quaternion(transformComp.rotation));
		projection = cameraComp.Camera.GetProjectionMatrix();
	}

	Render(renderTarget, view, projection);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Scene::OnUpdate(float deltaTime)
{
	PROFILE_FUNCTION();

	m_IsUpdating = true;
	m_Registry.view<AnimatedSpriteComponent>().each([=](auto entity, auto& animatedSpriteComp)
		{
			if (animatedSpriteComp.tileset)
				animatedSpriteComp.tileset->Animate(deltaTime);
		});

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

	m_Registry.view<LuaScriptComponent>().each([=](auto entity, auto& luaScriptComp)
		{
			if (!luaScriptComp.created)
			{
				luaScriptComp.OnCreate();
				luaScriptComp.created = true;
			}
			luaScriptComp.OnUpdate(deltaTime);
		});

	m_Registry.view<PrimitiveComponent, StaticMeshComponent>().each([=](auto entity, auto& primitiveComponent, auto& staticMeshComponent)
		{
			if (primitiveComponent.needsUpdating)
			{
				switch (primitiveComponent.type)
				{
				case PrimitiveComponent::Shape::Cube:
					staticMeshComponent.mesh.LoadModel(GeometryGenerator::CreateCube(primitiveComponent.cubeWidth, primitiveComponent.cubeHeight, primitiveComponent.cubeDepth), "Cube");
					break;
				case PrimitiveComponent::Shape::Sphere:
					staticMeshComponent.mesh.LoadModel(GeometryGenerator::CreateSphere(primitiveComponent.sphereRadius, primitiveComponent.sphereLongitudeLines, primitiveComponent.sphereLatitudeLines), "Sphere");
					break;
				case PrimitiveComponent::Shape::Plane:
					staticMeshComponent.mesh.LoadModel(GeometryGenerator::CreateGrid(primitiveComponent.planeWidth, primitiveComponent.planeLength, primitiveComponent.planeLengthLines, primitiveComponent.planeWidthLines, primitiveComponent.planeTileU, primitiveComponent.planeTileV), "Plane");
					break;
				case PrimitiveComponent::Shape::Cylinder:
					staticMeshComponent.mesh.LoadModel(GeometryGenerator::CreateCylinder(primitiveComponent.cylinderBottomRadius, primitiveComponent.cylinderTopRadius, primitiveComponent.cylinderHeight, primitiveComponent.cylinderSliceCount, primitiveComponent.cylinderStackCount), "Cylinder");
					break;
				case PrimitiveComponent::Shape::Cone:
					staticMeshComponent.mesh.LoadModel(GeometryGenerator::CreateCylinder(primitiveComponent.coneBottomRadius, 0.00001f, primitiveComponent.coneHeight, primitiveComponent.coneSliceCount, primitiveComponent.coneStackCount), "Cone");
					break;
				case PrimitiveComponent::Shape::Torus:
					staticMeshComponent.mesh.LoadModel(GeometryGenerator::CreateTorus(primitiveComponent.torusOuterRadius, primitiveComponent.torusInnerRadius, primitiveComponent.torusSliceCount), "Torus");
					break;
				default:
					break;
				}
				primitiveComponent.needsUpdating = false;
			}
		});

	m_IsUpdating = false;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Scene::OnFixedUpdate()
{
	PROFILE_FUNCTION();

	m_IsUpdating = true;

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

				nsc.Instance->OnFixedUpdate();
			}
			else
				ENGINE_ERROR("Native Script component was added but no scriptable entity was bound");
		});

	m_Registry.view<LuaScriptComponent>().each([=](auto entity, auto& luaScriptComp)
		{
			if (!luaScriptComp.created)
			{
				luaScriptComp.OnCreate();
				luaScriptComp.created = true;
			}
			luaScriptComp.OnFixedUpdate();
		});

	// Physics
	const int32_t velocityIterations = 6;
	const int32_t positionIterations = 2;
	if (m_Box2DWorld != nullptr)
	{
		m_Box2DWorld->Step(Application::Get().GetFixedUpdateInterval(), velocityIterations, positionIterations);

		m_Registry.view<TransformComponent, RigidBody2DComponent>().each([=](auto entity, auto& transformComp, auto& rigidBodyComp)
			{
				b2Body* body = (b2Body*)rigidBodyComp.runtimeBody;
				const b2Vec2& position = body->GetPosition();
				transformComp.position.x = position.x;
				transformComp.position.y = position.y;
				transformComp.rotation.z = (float)body->GetAngle();
			});
	}

	m_IsUpdating = false;
}

/* ------------------------------------------------------------------------------------------------------------------ */

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

/* ------------------------------------------------------------------------------------------------------------------ */

void Scene::Save(std::filesystem::path filepath, bool binary)
{
	PROFILE_FUNCTION();

	if (m_Snapshot.rdbuf()->in_avail() != 0)
		return;

	m_IsSaving = true;

	std::filesystem::path finalPath = filepath;
	finalPath.replace_extension(".scene");

	ENGINE_INFO("Saving Scene {0} to {1}", m_SceneName, finalPath.string());

	if (!std::filesystem::exists(finalPath))
	{
		std::filesystem::path filepathStem = finalPath;
		filepathStem.remove_filename();
		if (!std::filesystem::exists(filepathStem))
		{
			CLIENT_INFO("Creating Directory {0}", filepath);
			try
			{
				std::filesystem::create_directories(filepathStem);
			}
			catch (const std::exception& e)
			{
				std::cerr << e.what() << '\n';
			}
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
		SceneSerializer sceneSerializer = SceneSerializer(this);
		if (!sceneSerializer.Serialize(finalPath))
			ENGINE_ERROR("Failed to save scene to {0}.", finalPath.string());
	}

	m_Dirty = false;
	SceneSaved event(finalPath);
	Application::CallEvent(event);
	m_IsSaving = false;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Scene::Save(bool binary)
{
	if (!m_Filepath.empty())
		Save(m_Filepath, binary);
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool Scene::Load(bool binary)
{
	PROFILE_FUNCTION();

	std::filesystem::path filepath = m_Filepath;

	if (!std::filesystem::exists(filepath))
	{
		ENGINE_ERROR("File not found {0}", filepath);
		return false;
	}

	if (binary)
	{
		m_Registry.clear();
		std::ifstream file(filepath, std::ios::binary);
		cereal::BinaryInputArchive input(file);
		entt::snapshot_loader(m_Registry).entities(input).component<COMPONENTS>(input);
		file.close();
	}
	else
	{
		SceneSerializer sceneSerializer = SceneSerializer(this);
		if (!sceneSerializer.Deserialize(filepath))
			ENGINE_ERROR("Failed to load scene. Could not read file {0}", filepath);
	}

	m_Dirty = false;
	SceneLoaded event(filepath);
	Application::CallEvent(event);
	return true;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Scene::SetFilepath(std::filesystem::path filepath)
{
	filepath.replace_extension(".scene");

	if (Application::GetOpenDocument().empty())
	{
		if (std::filesystem::exists(Application::GetWorkingDirectory() / filepath))
		{
			m_Filepath = filepath;
		}
		else if (std::filesystem::exists(Application::GetOpenDocumentDirectory() / filepath))
		{
			m_Filepath = filepath;
		}
	}
}

/* ------------------------------------------------------------------------------------------------------------------ */

Entity Scene::GetPrimaryCameraEntity()
{
	auto view = m_Registry.view<CameraComponent>();
	for (auto entity : view)
	{
		auto& cameraComp = view.get<CameraComponent>(entity);
		if (cameraComp.Primary)
			return Entity{ entity, this };
	}
	return Entity();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Scene::DestroyBody(b2Body* body)
{
	if (m_Box2DWorld)
		m_Box2DWorld->DestroyBody(body);
}

void Scene::SetShowDebug(bool show)
{
	if (show && !m_Box2DDraw)
	{
		m_Box2DDraw = new Box2DDebugDraw();
		m_Box2DDraw->SetFlags(b2Draw::e_shapeBit);
		if (m_Box2DWorld)
			m_Box2DWorld->SetDebugDraw(m_Box2DDraw);
	}
	else if (!show)
	{
		delete m_Box2DDraw;
		m_Box2DDraw = nullptr;
	}
}

/* ------------------------------------------------------------------------------------------------------------------ */
