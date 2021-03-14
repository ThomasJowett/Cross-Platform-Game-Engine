#pragma once

#include <filesystem>

#include "EnTT/entt.hpp"
#include "math/Matrix.h"

class Entity;
class FrameBuffer;
class Camera;

class Scene
{
public:
	explicit Scene(std::filesystem::path filepath);
	~Scene();

	Entity CreateEntity(const std::string& name = "");

	bool RemoveEntity(const Entity& entity);

	// Render the scene to the render target from the camera transform and projection
	void Render(Ref<FrameBuffer> renderTarget, const Matrix4x4& cameraTransform, const Matrix4x4& projection);

	// Render the scene to the render target from the primary camera entity in the scene
	void Render(Ref<FrameBuffer> renderTarget);

	void DrawIDBuffer(Ref<FrameBuffer> renderTarget, const Matrix4x4& cameraTransform, const Matrix4x4& projection);

	// Called once per frame
	void OnUpdate(float deltaTime);

	// Called 100 times a second
	void OnFixedUpdate();

	void OnViewportResize(uint32_t width, uint32_t height);

	entt::registry& GetRegistry() { return m_Registry; }
	const std::string& GetSceneName() const { return m_SceneName; }

	virtual void Save(bool binary = false);
	virtual void Save(std::filesystem::path filepath, bool binary = false);
	virtual bool Load(bool binary = false);

	void MakeDirty() { m_Dirty = true; }
	bool IsDirty() { return m_Dirty; }

	bool IsSaving() { return m_IsSaving; }
	bool IsUpdating() { return m_IsUpdating; }

	const std::filesystem::path GetFilepath() const { return m_Filepath; }
	void SetFilepath(std::filesystem::path filepath);

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

	friend class Entity;
};