#pragma once

#include "include.h"
#include "imgui/imgui.h"

#include "ViewportCameraController.h"

#include "Interfaces/ICopyable.h"
#include "Interfaces/IUndoable.h"

class ImGuiViewportPanel
	:public Layer, public ICopyable, public IUndoable
{
public: 
	explicit ImGuiViewportPanel(bool* show);
	~ImGuiViewportPanel() = default;

	void OnAttach() override;
	void OnUpdate(float deltaTime) override;
	void OnFixedUpdate() override;
	void OnImGuiRender() override;

	void Copy() override;
	void Cut() override;
	void Paste() override;
	void Duplicate() override;
	void Delete() override;

	bool CanUndo() const override;
	bool CanRedo() const override;
	void Undo(int astep = 1) override;
	void Redo(int astep = 1) override;
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