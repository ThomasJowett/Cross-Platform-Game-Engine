#pragma once

#include <filesystem>
#include <sstream>

#include "EnTT/entt.hpp"
#include "Core/UUID.h"
#include "math/Vector2f.h"

class ContactListener2D;
class Entity;
class FrameBuffer;
class Camera;
class b2World;
class b2Body;
class b2Draw;
class Matrix4x4;
struct HitResult2D;

class Scene
{
public:
	explicit Scene(const std::filesystem::path& filepath);
	~Scene();

	Entity CreateEntity(const std::string& name = "");
	Entity CreateEntity(Uuid id, const std::string& name = "");

	void InstantiateScene(const Ref<Scene> prefab, const Vector3f& position);
	Entity InstantiateEntity(const Entity prefab, const Vector3f& position);

	bool RemoveEntity(Entity& entity);

	void DuplicateEntity(Entity entity, Entity parent);

	void OnRuntimeStart();
	void OnRuntimePause();
	void OnRuntimeStop();

	// Render the scene to the render target from the camera transform and projection
	void Render(Ref<FrameBuffer> renderTarget, const Matrix4x4& cameraTransform, const Matrix4x4& projection);

	// Render the scene to the render target from the primary camera entity in the scene
	void Render(Ref<FrameBuffer> renderTarget);

	// Called once per frame
	void OnUpdate(float deltaTime);

	// Called 100 times a second
	void OnFixedUpdate();

	void OnViewportResize(uint32_t width, uint32_t height);

	entt::registry& GetRegistry() { return m_Registry; }

	void Save(bool binary = false);
	void Save(std::filesystem::path filepath, bool binary = false);
	bool Load(bool binary = false);

	void MakeDirty() { m_Dirty = true; }
	bool IsDirty() const { return m_Dirty; }

	bool IsSaving() const { return m_IsSaving; }
	bool IsUpdating() const { return m_IsUpdating; }

	std::filesystem::path GetFilepath() const { return m_Filepath; }
	void SetFilepath(std::filesystem::path filepath);

	Entity GetPrimaryCameraEntity();
	Entity GetEntityByName(const std::string& name);
	Entity GetEntityByPath(const std::string& path);

	void DestroyBody(b2Body* body);
	void SetShowDebug(bool show);

	Vector2f& GetGravity() { return m_Gravity; }
	void SetGravity(const Vector2f& gravity) { m_Gravity = gravity; }

	HitResult2D RayCast2D(Vector2f begin, Vector2f end);

	std::vector<HitResult2D> MultiRayCast2D(Vector2f begin, Vector2f end);

private:
	entt::registry m_Registry;

	std::filesystem::path m_Filepath;

	uint32_t m_ViewportWidth = 0;
	uint32_t m_ViewportHeight = 0;

	bool m_Dirty = false;

	bool m_IsUpdating = false;
	bool m_IsSaving = false;

	b2World* m_Box2DWorld = nullptr;
	b2Draw* m_Box2DDraw = nullptr;

	Scope<ContactListener2D> m_ContactListener;

	Vector2f m_Gravity = { 0.0f, -9.81f };

	std::stringstream m_Snapshot;

	friend class Entity;
	friend class SceneSerializer;
};
