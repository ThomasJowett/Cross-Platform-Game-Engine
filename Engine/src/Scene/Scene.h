#pragma once

#include <filesystem>
#include <sstream>

#include "EnTT/entt.hpp"
#include "Core/UUID.h"
#include "math/Vector2f.h"

class Entity;
class FrameBuffer;
class Camera;
class b2World;
class b2Body;
class b2Draw;
class Matrix4x4;

class Scene
{
public:
	Scene(std::filesystem::path filepath);
	Scene(std::string name);
	~Scene();

	Entity CreateEntity(const std::string& name = "");
	Entity CreateEntity(Uuid id, const std::string& name = "");

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
	const std::string& GetSceneName() const { return m_SceneName; }
	void SetSceneName(std::string name) { m_SceneName = name; MakeDirty(); }

	virtual void Save(bool binary = false);
	virtual void Save(std::filesystem::path filepath, bool binary = false);
	virtual bool Load(bool binary = false);

	void MakeDirty() { m_Dirty = true; }
	bool IsDirty() { return m_Dirty; }

	bool IsSaving() { return m_IsSaving; }
	bool IsUpdating() { return m_IsUpdating; }

	const std::filesystem::path GetFilepath() const { return m_Filepath; }
	void SetFilepath(std::filesystem::path filepath);

	Entity GetPrimaryCameraEntity();

	void DestroyBody(b2Body* body);
	void SetShowDebug(bool show);

	Vector2f& GetGravity() { return m_Gravity; }
	void SetGravity(const Vector2f& gravity) { m_Gravity = gravity; }

private:
	entt::registry m_Registry;

protected:
	std::string m_SceneName;
	std::filesystem::path m_Filepath;

	uint32_t m_ViewportWidth = 0;
	uint32_t m_ViewportHeight = 0;

	bool m_Dirty = false;

	bool m_IsUpdating = false;
	bool m_IsSaving = false;

	b2World* m_Box2DWorld = nullptr;
	b2Draw* m_Box2DDraw = nullptr;

	Vector2f m_Gravity = { 0.0f, -9.81f };

	std::stringstream m_Snapshot;

	friend class Entity;
	friend class SceneSerializer;
};
