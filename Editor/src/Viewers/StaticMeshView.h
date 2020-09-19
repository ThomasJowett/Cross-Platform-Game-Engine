#pragma once

#include "Engine.h"

#include "ViewportCameraController.h"

class StaticMeshView :
	public Layer
{
public:
	StaticMeshView(bool* show, std::filesystem::path filepath);
	~StaticMeshView() = default;

	virtual void OnAttach() override;
	virtual void OnImGuiRender() override;
	virtual void OnUpdate(float deltaTime) override;
	virtual void OnFixedUpdate() override;
private:
	bool* m_Show;

	std::filesystem::path m_FilePath;

	std::string m_WindowName;

	Ref<Mesh> m_Mesh;

	Ref<Texture2D> m_Texture;

	bool m_WindowHovered = false;
	bool m_WindowFocussed = false;
	bool m_CursorDisabled;
	ImVec2 m_ViewportSize;
	Ref<FrameBuffer> m_Framebuffer;
	Vector2f m_RelativeMousePosition;

	ShaderLibrary m_ShaderLibrary;
	ViewportCameraController m_CameraController;
};

