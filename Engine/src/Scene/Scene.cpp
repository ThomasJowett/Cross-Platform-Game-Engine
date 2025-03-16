#include "stdafx.h"
#include "Scene.h"
#include "Entity.h"

#include "Components.h"
#include "Renderer/Renderer2D.h"
#include "Renderer/Renderer.h"
#include "Renderer/Camera.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/FrameBuffer.h"

#include "Utilities/GeometryGenerator.h"
#include "Utilities/Box2DDebugDraw.h"

#include "cereal/archives/binary.hpp"
#include "cereal/types/string.hpp"

#include "Events/SceneEvent.h"
#include "TinyXml2/tinyxml2.h"

#include "SceneSerializer.h"
#include "SceneGraph.h"
#include "Scripting/Lua/LuaManager.h"
#include "Physics/HitResult2D.h"
#include "Physics/Contact2D.h"

struct DestroyMarker {};

template<typename Component>
static void CopyComponentIfExists(entt::entity dst, entt::entity src, entt::registry& registry)
{
	if (registry.any_of<Component>(src))
	{
		Component& srcComponent = registry.get<Component>(src);
		registry.emplace_or_replace<Component>(dst, srcComponent);
	}
}

template<typename... Component>
static void CopyEntity(entt::entity dst, entt::entity src, entt::registry& registry)
{
	(CopyComponentIfExists<Component>(dst, src, registry), ...);
}

Scene::Scene(const std::filesystem::path& filepath)
	:m_Filepath(filepath)
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
	m_Dirty = true;
	return entity;
}

void Scene::InstantiateScene(const Ref<Scene> prefab, const Vector3f& position)
{
	PROFILE_FUNCTION();
	prefab->GetRegistry().each([=](const entt::entity entity) {
		Entity prefabEntity(entity, prefab.get());
		if (HierarchyComponent* hierarchyComp = prefabEntity.TryGetComponent<HierarchyComponent>())
			if (hierarchyComp->parent != entt::null)
				return;

		InstantiateEntity(prefabEntity, position);
		});
}

/* ------------------------------------------------------------------------------------------------------------------ */

Entity Scene::InstantiateEntity(const Entity prefab, const Vector3f& position)
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* pEntityElement = doc.NewElement("Entity");
	doc.InsertFirstChild(pEntityElement);
	SceneSerializer::SerializeEntity(pEntityElement, prefab);
	tinyxml2::XMLPrinter printer;
	doc.Accept(&printer);

	Entity newEntity = SceneSerializer::DeserializeEntity(this, pEntityElement, true);

	if (TransformComponent* transformComp = newEntity.TryGetComponent<TransformComponent>(); transformComp)
		transformComp->position += position;

	if (m_PhysicsEngine2D)
		m_PhysicsEngine2D->InitializeEntity(newEntity);

	if (LuaScriptComponent* scriptComponent = newEntity.TryGetComponent<LuaScriptComponent>())
	{
		std::optional<std::pair<int, std::string>> result = scriptComponent->ParseScript(newEntity);
		if (result.has_value())
		{
			ENGINE_ERROR("Failed to parse lua script {0}({1}): {2}", scriptComponent->absoluteFilepath, result.value().first, result.value().second);
		}
	}
	return Entity();
}

bool Scene::RemoveEntity(Entity& entity)
{
	if (entity.BelongsToScene(this))
	{
		if (m_IsUpdating) {
			if (!m_Registry.any_of<DestroyMarker>(entity))
				m_Registry.emplace<DestroyMarker>(entity);
		}
		else
			SceneGraph::Remove(entity);
		return true;
	}
	return false;
}

/* ------------------------------------------------------------------------------------------------------------------ */

Entity Scene::DuplicateEntity(Entity entity, Entity parent)
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

	std::vector<Entity> children = SceneGraph::GetChildren(entity);

	for (auto& child : children)
	{
		DuplicateEntity(child, newEntity);
	}
	if (parent)
	{
		SceneGraph::Reparent(newEntity, parent);
	}

	return newEntity;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Scene::OnRuntimeStart()
{
	PROFILE_FUNCTION();

	ENGINE_DEBUG("Runtime Start");
	if (m_Dirty)
		Save();

	std::stringstream().swap(m_Snapshot);
	cereal::BinaryOutputArchive output(m_Snapshot);
	entt::snapshot(m_Registry).entities(output).component<COMPONENTS>(output);

	m_Registry.view<LuaScriptComponent>().each(
		[this](const auto entity, auto& scriptComponent)
		{
			std::optional<std::pair<int, std::string>> result = scriptComponent.ParseScript(Entity{ entity, this });
			if (result.has_value())
			{
				ENGINE_ERROR("Failed to parse lua script {0}({1}): {2}", scriptComponent.absoluteFilepath, result.value().first, result.value().second);
			}
			else
			{
				scriptComponent.OnCreate();
				scriptComponent.created = true;
			}
		});

	m_PhysicsEngine2D = CreateScope<PhysicsEngine2D>(m_Gravity, this);

	if (m_DrawDebug)
		m_PhysicsEngine2D->ShowDebugDraw(m_DrawDebug);

	m_AudioEngine = CreateRef<ma_engine>();
	if (ma_engine_init(nullptr, m_AudioEngine.get()) != MA_SUCCESS) {
		ENGINE_CRITICAL("Failed to initialize MiniAudio engine");
	}

	m_Registry.view<AudioSourceComponent>().each(
		[this](const auto entity, auto& audioSourceComponent)
		{
			if (audioSourceComponent.audioClip)
			{
				audioSourceComponent.sound = CreateRef<ma_sound>();
				ma_uint32 flags = audioSourceComponent.stream ? MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_STREAM : MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_DECODE;
				if (audioSourceComponent.loop)
					flags |= MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_LOOPING;
				if (ma_sound_init_from_file(
					m_AudioEngine.get(), audioSourceComponent.audioClip->GetFilepath().string().c_str(),
					flags, NULL, NULL, audioSourceComponent.sound.get()) != MA_SUCCESS)
				{
					ENGINE_ERROR("Failed to load audio file: {0}", audioSourceComponent.audioClip->GetFilepath());
				}
				else
				{
					ma_sound_set_volume(audioSourceComponent.sound.get(), audioSourceComponent.volume);
					ma_sound_set_pitch(audioSourceComponent.sound.get(), audioSourceComponent.pitch);

					if (auto* transformComponent = m_Registry.try_get<TransformComponent>(entity))
					{
						auto position = transformComponent->GetWorldPosition();
						ma_sound_set_position(audioSourceComponent.sound.get(), position.x, position.y, position.z);
					}
				}

				audioSourceComponent.play = true;
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

	m_Registry.view<AudioSourceComponent>().each(
		[this](const auto entity, auto& audioSourceComponent)
		{
			if (audioSourceComponent.audioClip)
			{
				ma_sound_uninit(audioSourceComponent.sound.get());
			}
		});

	ma_engine_uninit(m_AudioEngine.get());
	m_AudioEngine.reset();

	m_PhysicsEngine2D.reset();

	LuaManager::CleanUp();

	if (m_Snapshot.rdbuf()->in_avail() != 0)
	{
		ENGINE_DEBUG("Runtime End");
		m_Registry = entt::registry();
		cereal::BinaryInputArchive input(m_Snapshot);
		entt::snapshot_loader(m_Registry).entities(input).component<COMPONENTS>(input);
	}
	std::stringstream().swap(m_Snapshot);
	m_Dirty = false;
	AssetManager::CleanUp();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Scene::Render(Ref<FrameBuffer> renderTarget, const Matrix4x4& cameraTransform, const Matrix4x4& projection)
{
	PROFILE_FUNCTION();

	auto billboardView = m_Registry.view<TransformComponent, BillboardComponent>();
	for (auto entity : billboardView)
	{
		auto&& [transformComp, billboardComp] = billboardView.get(entity);
		Matrix4x4 transform = Matrix4x4();

		Vector3f cameraRight = Vector3f(cameraTransform(0, 0), cameraTransform(1, 0), cameraTransform(2, 0));
		Vector3f billboardRight = Vector3f(1.0f, 0.0f, 0.0f);

		float angleY = acos(Vector3f::Dot(cameraRight, billboardRight));
		if (Vector3f::Cross(cameraRight, billboardRight).y > 0.0f)
			angleY = -angleY;
		transform = Matrix4x4::RotateY(angleY);

		if (billboardComp.orientation == BillboardComponent::Orientation::Camera)
		{
			Vector3f cameraUp = Vector3f(cameraTransform(0, 1), cameraTransform(1, 1), cameraTransform(2, 1));
			Vector3f billboardUp = Vector3f(0.0f, 1.0f, 0.0f);
			float angleX = acos(Vector3f::Dot(cameraUp, billboardUp));
			if ((Vector3f::Cross(cameraUp, billboardUp).x > 0.0f) != (angleY > PIDIV2 || angleY < -PIDIV2))
				angleX = -angleX;
			transform = transform * Matrix4x4::RotateX(angleX);
		}
		Quaternion rot = transform.ExtractRotation();
		transformComp.rotation = rot.EulerAngles();
	}

	SceneGraph::Traverse(m_Registry);
	if (renderTarget != nullptr)
		renderTarget->Bind();

	Renderer::BeginScene(cameraTransform, projection);

	auto spriteGroup = m_Registry.view<TransformComponent, SpriteComponent>();
	for (auto entity : spriteGroup)
	{
		auto&& [transformComp, spriteComp] = spriteGroup.get(entity);
		Renderer2D::DrawSprite(transformComp.GetWorldMatrix(), spriteComp, (int)entity);
	}

	auto animatedSpriteGroup = m_Registry.view<TransformComponent, AnimatedSpriteComponent>();
	for (auto entity : animatedSpriteGroup)
	{
		auto&& [transformComp, spriteComp] = animatedSpriteGroup.get(entity);
		if (spriteComp.spriteSheet && spriteComp.spriteSheet->GetSubTexture()) {
			spriteComp.spriteSheet->GetSubTexture()->SetCurrentCell(spriteComp.currentFrame);
			Renderer2D::DrawQuad(transformComp.GetWorldMatrix(), spriteComp.spriteSheet->GetSubTexture(), spriteComp.tint, (int)entity);
		}
	}

	auto circleGroup = m_Registry.view<TransformComponent, CircleRendererComponent>();
	for (auto entity : circleGroup)
	{
		auto&& [transformComp, circleComp] = circleGroup.get(entity);
		Renderer2D::DrawCircle(transformComp.GetWorldMatrix(), circleComp, (int)entity);
	}

	auto textGroup = m_Registry.view<TransformComponent, TextComponent>();
	for (auto entity : textGroup)
	{
		auto&& [transformComp, textComp] = textGroup.get(entity);
		Renderer2D::DrawString(textComp.text, textComp.font, textComp.maxWidth, transformComp.GetWorldMatrix(), textComp.colour, (int)entity);
	}

	auto staticMeshGroup = m_Registry.view<TransformComponent, StaticMeshComponent>();
	for (auto entity : staticMeshGroup)
	{
		auto&& [transformComp, staticMeshComp] = staticMeshGroup.get(entity);
		if (staticMeshComp.mesh)
		{
			Renderer::Submit(staticMeshComp.mesh->GetMesh(), staticMeshComp.materialOverrides, transformComp.GetWorldMatrix(), (int)entity);
			//const std::vector<Ref<Mesh>>& meshes = staticMeshComp.mesh->GetMeshes();
			//if (staticMeshComp.materialOverrides.size() != meshes.size())
			//	staticMeshComp.materialOverrides.resize(meshes.size());
			//for (size_t i = 0; i < meshes.size(); i++)
			//{
			//	if (!staticMeshComp.materialOverrides[i].empty())
			//		Renderer::Submit(AssetManager::GetAsset<Material>(staticMeshComp.materialOverrides[i]), meshes[i]->GetVertexArray(), transformComp.GetWorldMatrix(), (int)entity);
			//	else
			//		Renderer::Submit(meshes[i]->GetMaterial(), meshes[i]->GetVertexArray(), transformComp.GetWorldMatrix(), (int)entity);
			//}
		}
	}

	auto primitiveGroup = m_Registry.view<TransformComponent, PrimitiveComponent>();
	for (auto entity : primitiveGroup)
	{
		auto&& [transformComp, primitiveComp] = primitiveGroup.get(entity);
		Renderer::Submit(primitiveComp.mesh, primitiveComp.material, transformComp.GetWorldMatrix(), (int)entity);
	}

	auto tilemapGroup = m_Registry.view<TransformComponent, TilemapComponent>();
	for (auto entity : tilemapGroup)
	{
		auto&& [transformComp, tilemapComp] = tilemapGroup.get(entity);
		if (tilemapComp.tileset && tilemapComp.mesh)
		{
			Renderer::Submit(tilemapComp.mesh, transformComp.GetWorldMatrix(), (int)entity);
		}
	}

	if (m_PhysicsEngine2D)
		m_PhysicsEngine2D->OnRender();

	Renderer::EndScene();

	if (renderTarget != nullptr)
		RenderCommand::ClearDepth();

	SceneGraph::TraverseUI(m_Registry, renderTarget->GetSpecification().width, renderTarget->GetSpecification().height);

	float halfWidth = renderTarget->GetSpecification().width / 2.0f;
	float halfHeight = renderTarget->GetSpecification().height / 2.0f;
	Renderer::BeginScene(Matrix4x4::Translate(Vector3f(halfWidth, -halfHeight, 0.0f)), Matrix4x4::OrthographicRH(-halfWidth, halfWidth, -halfHeight, halfHeight, -1, 1.0f));

	auto buttonGroup = m_Registry.view<WidgetComponent, ButtonComponent>();
	for (auto entity : buttonGroup)
	{
		auto&& [widgetComp, buttonComp] = buttonGroup.get(entity);

		switch (widgetComp.state)
		{
		case WidgetComponent::WidgetState::normal:
			Renderer2D::DrawQuad(widgetComp.GetTransformMatrix(), buttonComp.normalTexture, buttonComp.normalTint, 1.0f, (int)entity);
			break;
		case WidgetComponent::WidgetState::hovered:
			Renderer2D::DrawQuad(widgetComp.GetTransformMatrix(), buttonComp.hoveredTexture, buttonComp.hoveredTint, 1.0f, (int)entity);
			break;
		case WidgetComponent::WidgetState::clicked:
			Renderer2D::DrawQuad(widgetComp.GetTransformMatrix(), buttonComp.clickedTexture, buttonComp.clickedTint, 1.0f, (int)entity);
			break;
		default:
			break;
		}

		//TODO: check button state
	}
	Renderer::EndScene();

	if (renderTarget != nullptr)
		renderTarget->UnBind();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Scene::Render(Ref<FrameBuffer> renderTarget)
{
	PROFILE_FUNCTION();

	Matrix4x4 view;
	Matrix4x4 projection;

	Entity cameraEntity = GetPrimaryCameraEntity();

	if (cameraEntity)
	{
		auto [cameraComp, transformComp] = cameraEntity.GetComponents<CameraComponent, TransformComponent>();
		view = Matrix4x4::Translate(transformComp.GetWorldPosition()) * Matrix4x4::Rotate(Quaternion(transformComp.rotation));
		projection = cameraComp.camera.GetProjectionMatrix();
	}

	Render(renderTarget, view, projection);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Scene::OnUpdate(float deltaTime)
{
	PROFILE_FUNCTION();

	m_IsUpdating = true;
	m_Registry.view<AnimatedSpriteComponent>(entt::exclude<DestroyMarker>).each([deltaTime](auto entity, auto& animatedSpriteComp)
		{
			if (animatedSpriteComp.spriteSheet)
				animatedSpriteComp.Animate(deltaTime);
		});

	m_Registry.view<LuaScriptComponent>(entt::exclude<DestroyMarker>).each([deltaTime](auto entity, auto& luaScriptComp)
		{
			if (!luaScriptComp.created)
			{
				luaScriptComp.OnCreate();
				luaScriptComp.created = true;
			}
			luaScriptComp.OnUpdate(deltaTime);
		});

	m_Registry.view<PrimitiveComponent>(entt::exclude<DestroyMarker>).each([](auto entity, auto& primitiveComponent)
		{
			if (primitiveComponent.needsUpdating)
			{
				primitiveComponent.SetType(primitiveComponent.type);
			}
		});

	m_Registry.view<BehaviourTreeComponent>(entt::exclude<DestroyMarker>).each([deltaTime](auto entity, auto& behaviourTreeComponent)
		{
			if (behaviourTreeComponent.behaviourTree)
				behaviourTreeComponent.behaviourTree->update(deltaTime);
		});

	m_Registry.view<AudioSourceComponent>(entt::exclude<DestroyMarker>).each([this](auto entity, auto& audioSourceComponent)
		{
			if (audioSourceComponent.play) {
				if (!ma_sound_is_playing(audioSourceComponent.sound.get()))
				{
					ma_sound_stop(audioSourceComponent.sound.get());
				}
				ma_sound_start(audioSourceComponent.sound.get());
				audioSourceComponent.play = false;
			}
		});

	m_IsUpdating = false;

	auto destroyView = m_Registry.view<DestroyMarker>();

	for (auto entity : destroyView)
	{
		Entity e = Entity(entity, this);
		m_PhysicsEngine2D->DestroyEntity(e);

		SceneGraph::Remove(e);
	}
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Scene::OnFixedUpdate()
{
	PROFILE_FUNCTION();

	m_IsUpdating = true;

	// Physics
	m_PhysicsEngine2D->OnFixedUpdate();

	m_Registry.view<LuaScriptComponent>(entt::exclude<DestroyMarker>).each([=](auto entity, auto& luaScriptComp)
		{
			if (!luaScriptComp.created)
			{
				luaScriptComp.OnCreate();
				luaScriptComp.created = true;
			}
			luaScriptComp.OnFixedUpdate();
			if (luaScriptComp.IsContactListener() && !m_PhysicsEngine2D->GetContactListener()->m_Contacts.empty())
			{
				for (auto fixture : luaScriptComp.GetFixtures())
				{
					for (Contact2D& contact : m_PhysicsEngine2D->GetContactListener()->m_Contacts)
					{
						if (contact.fixtureA == fixture || contact.fixtureB == fixture)
						{
							if (fixture == contact.fixtureB && !contact.triggeredA)
							{
								luaScriptComp.OnBeginContact(contact.fixtureA, contact.localNormal, contact.localPoint);
								contact.triggeredA = true;
							}
							else if (fixture == contact.fixtureA && !contact.triggeredB)
							{
								luaScriptComp.OnBeginContact(contact.fixtureB, contact.localNormal, contact.localPoint);
								contact.triggeredB = true;
							}

							if (contact.old)
							{
								luaScriptComp.OnEndContact(fixture == contact.fixtureA ? contact.fixtureB : contact.fixtureA);
							}
						}
					}
				}
			}
		});

	if (m_PhysicsEngine2D != nullptr)
	{
		m_PhysicsEngine2D->RemoveOldContacts();
		m_Registry.view<TransformComponent, RigidBody2DComponent>().each([=](auto entity, auto& transformComp, auto& rigidBodyComp)
			{
				rigidBodyComp.runtimeBody->SetTransform(b2Vec2(transformComp.position.x, transformComp.position.y), transformComp.rotation.z);
			});

		m_Registry.view<TransformComponent, BoxCollider2DComponent>(entt::exclude<RigidBody2DComponent>).each([=](auto entity, auto& transformComp, auto& colliderComp)
			{
				colliderComp.runtimeBody->SetTransform(b2Vec2(transformComp.position.x, transformComp.position.y), transformComp.rotation.z);
			});

		m_Registry.view<TransformComponent, CircleCollider2DComponent>(entt::exclude<RigidBody2DComponent>).each([=](auto entity, auto& transformComp, auto& colliderComp)
			{
				colliderComp.runtimeBody->SetTransform(b2Vec2(transformComp.position.x, transformComp.position.y), transformComp.rotation.z);
			});

		m_Registry.view<TransformComponent, PolygonCollider2DComponent>(entt::exclude<RigidBody2DComponent>).each([=](auto entity, auto& transformComp, auto& colliderComp)
			{
				colliderComp.runtimeBody->SetTransform(b2Vec2(transformComp.position.x, transformComp.position.y), transformComp.rotation.z);
			});

		m_Registry.view<TransformComponent, CapsuleCollider2DComponent>(entt::exclude<RigidBody2DComponent>).each([=](auto entity, auto& transformComp, auto& colliderComp)
			{
				colliderComp.runtimeBody->SetTransform(b2Vec2(transformComp.position.x, transformComp.position.y), transformComp.rotation.z);
			});

		m_Registry.view<TransformComponent, TilemapComponent>(entt::exclude<RigidBody2DComponent>).each([=](auto entity, auto& transformComp, auto& colliderComp)
			{
				if (colliderComp.runtimeBody)
					colliderComp.runtimeBody->SetTransform(b2Vec2(transformComp.position.x, transformComp.position.y), transformComp.rotation.z);
			});
	}

	auto destroyView = m_Registry.view<DestroyMarker>();

	for (auto entity : destroyView)
	{
		Entity e = Entity(entity, this);
		m_PhysicsEngine2D->DestroyEntity(e);

		SceneGraph::Remove(e);
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
			if (!cameraComp.fixedAspectRatio)
			{
				cameraComp.camera.SetAspectRatio((float)width / (float)height);
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

	ENGINE_INFO("Saving Scene to {0}", finalPath.string());

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
	SceneSavedEvent event(finalPath);
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
	SceneLoadedEvent event(filepath);
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
		CameraComponent const& cameraComp = view.get<CameraComponent>(entity);
		if (cameraComp.primary)
			return Entity{ entity, this };
	}
	return Entity();
}

/* ------------------------------------------------------------------------------------------------------------------ */

Entity Scene::GetEntityByName(const std::string& name)
{
	auto view = m_Registry.view<NameComponent>();
	for (auto entity : view)
	{
		auto [nameComp] = view.get(entity);
		if (name == nameComp.name)
			return Entity(entity, this);
	}
	return Entity();
}

/* ------------------------------------------------------------------------------------------------------------------ */

Entity Scene::GetEntityByPath(const std::string& path)
{
	std::vector<std::string> splitPath = SplitString(path, '/');

	if (splitPath.size() == 1)
		return GetEntityByName(splitPath[0]);
	else
	{
		entt::entity entity = SceneGraph::FindEntity(splitPath, m_Registry);
		if (entity != entt::null)
			return Entity(entity, this);
		else
			return Entity();
	}
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Scene::SetShowDebug(bool show)
{
	if (m_PhysicsEngine2D)
		m_PhysicsEngine2D->ShowDebugDraw(show);
	m_DrawDebug = show;
}

/* ------------------------------------------------------------------------------------------------------------------ */

HitResult2D Scene::RayCast2D(Vector2f begin, Vector2f end)
{
	if (m_PhysicsEngine2D)
		return m_PhysicsEngine2D->RayCast(begin, end);
	return HitResult2D();
}

/* ------------------------------------------------------------------------------------------------------------------ */

std::vector<HitResult2D> Scene::MultiRayCast2D(Vector2f begin, Vector2f end)
{
	if (m_PhysicsEngine2D)
		return m_PhysicsEngine2D->MultiRayCast2D(begin, end);
	return std::vector<HitResult2D>();
}

/* ------------------------------------------------------------------------------------------------------------------ */