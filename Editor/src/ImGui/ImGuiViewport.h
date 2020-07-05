#pragma once

#include "include.h"
#include "imgui/imgui.h"

#include "ViewportCameraController.h"

class ImGuiViewportPanel
	:public Layer
{
public: 
	explicit ImGuiViewportPanel(bool* show);
	~ImGuiViewportPanel() = default;

	void OnAttach() override;
	void OnUpdate(float deltaTime) override;
	void OnFixedUpdate() override;
	void OnImGuiRender() override;

	void Copy();
	void Cut();
	void Paste();
	void Duplicate();
	void Delete();

	bool CanUndo() const;
	bool CanRedo() const;
	void Undo(int astep = 1);
	void Redo(int astep = 1);
private:
	void HandleKeyboardInputs();

private:
	bool* m_Show;
	bool m_WindowHovered = false;
	ImVec2 m_ViewportSize;
	Ref<FrameBuffer> m_Framebuffer;
	Vector2f m_RelativeMousePosition;

	ShaderLibrary m_ShaderLibrary;
	ViewportCameraController m_CameraController;
	Ref<VertexArray> m_TorusVertexArray;

	Ref<Texture2D> m_Texture;
};