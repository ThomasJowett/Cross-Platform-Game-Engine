#pragma once

#include <filesystem>

#include "Core/Layer.h"
#include "Renderer/Mesh.h"
#include "Asset/Material.h"
#include "Asset/StaticMesh.h"

#include "Renderer/FrameBuffer.h"
#include "imgui/imgui.h"

#include "Interfaces/ISaveable.h"
#include "OrbitCameraController.h"
#include "ViewerManager.h"

class StaticMeshView :
	public View, public ISaveable
{
public:
	StaticMeshView(bool* show, std::filesystem::path filepath);
	~StaticMeshView() = default;

	virtual void OnAttach() override;
	virtual void OnImGuiRender() override;
	virtual void OnUpdate(float deltaTime) override;
	virtual void OnFixedUpdate() override;
private:
	void DrawDetailsPanel();
	void DrawViewport();

	// Inherited via ISaveable
	virtual void Save() override;
	virtual void SaveAs() override;
	virtual bool NeedsSaving() override { return m_Dirty; }

private:
	bool* m_Show;

	std::filesystem::path m_FilePath;

	Ref<StaticMesh> m_Mesh;

	// Edited copy of the mesh's default materials, only written back on save
	std::vector<Ref<Material>> m_Materials;

	bool m_Dirty = false;

	ImVec2 m_ViewportSize;
	Ref<FrameBuffer> m_Framebuffer;

	OrbitCameraController m_CameraController;
	BoundingBox m_Bounds;
	bool m_NeedsFraming = true;

	bool m_MouseLooking = false;
	Vector2f m_LastMousePosition;
	int m_IgnoreMouseFrames = 0;

	Ref<Mesh> m_GridMesh;
	Ref<Material> m_GridMaterial;
	bool m_ShowGrid = true;
};
