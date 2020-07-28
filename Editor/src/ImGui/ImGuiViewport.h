#pragma once

#include "include.h"
#include "imgui/imgui.h"

#include "ViewportCameraController.h"

#include "Interfaces/ICopyable.h"
#include "Interfaces/IUndoable.h"
#include "Interfaces/ISaveable.h"

class ImGuiViewportPanel
	:public Layer, public ICopyable, public IUndoable, public ISaveable
{
	enum class  Mode
	{
		Select = 0,
		Move,
		Rotate,
		Scale
	};

public: 
	explicit ImGuiViewportPanel(bool* show);
	~ImGuiViewportPanel() = default;

	void OnAttach() override;
	void OnUpdate(float deltaTime) override;
	void OnFixedUpdate() override;
	void OnImGuiRender() override;

	virtual void Copy() override;
	virtual void Cut() override;
	virtual void Paste() override;
	virtual void Duplicate() override;
	virtual void Delete() override;
	virtual void SelectAll() override;
	virtual bool HasSelection() const override;
	virtual bool IsReadOnly() const override { return false; }

	virtual bool CanUndo() const override;
	virtual bool CanRedo() const override;
	virtual void Undo(int astep = 1) override;
	virtual void Redo(int astep = 1) override;

	virtual void Save() override;
	virtual void SaveAs() override;
private:
	void HandleKeyboardInputs();

private:
	bool* m_Show;
	bool m_WindowHovered = false;
	bool m_CursorDisabled;
	ImVec2 m_ViewportSize;
	Ref<FrameBuffer> m_Framebuffer;
	Vector2f m_RelativeMousePosition;

	ShaderLibrary m_ShaderLibrary;
	ViewportCameraController m_CameraController;
	OrthogrpahicCameraController m_OrthoCamera;
	Ref<VertexArray> m_TorusVertexArray;

	Ref<Mesh> m_Mesh;

	Ref<Texture2D> m_Texture;

	Mode m_Mode;

	Ref<Scene> m_Scene;
};