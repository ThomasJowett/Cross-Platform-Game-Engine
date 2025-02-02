#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"

#include "ViewportCameraController.h"

#include "Interfaces/ICopyable.h"
#include "Interfaces/IUndoable.h"
#include "Interfaces/ISaveable.h"

#include "Asset/Shader.h"

#include "HierarchyPanel.h"
#include "ImGui/ImGuiTilemapEditor.h"
#include "History/HistoryCommands.h"

class ViewportPanel
	:public Layer, public ICopyable, public IUndoable
{
	enum class OperationMode
	{
		Select = 0,
		Move,
		Rotate,
		Scale,
		Universal
	};

	enum class TranslationMode
	{
		Local,
		World
	};

public:
	explicit ViewportPanel(bool* show, Ref<HierarchyPanel> hierarchyPanel, Ref<TilemapEditor> tilemapEditor);
	~ViewportPanel() = default;

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(float deltaTime) override;
	void OnImGuiRender() override;
	void OnEvent(Event& event) override;

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

private:
	void HandleKeyboardInputs();

private:
	bool* m_Show;
	bool m_WindowHovered = false;
	bool m_WindowFocussed = false;
	bool m_ViewportHovered = false;
	bool m_CursorDisabled = false;
	ImVec2 m_ViewportSize;
	Ref<FrameBuffer> m_Framebuffer;
	Ref<FrameBuffer> m_CameraPreview;
	Vector2f m_RelativeMousePosition;
	Vector2f m_MousePositionBeginClick;

	ViewportCameraController m_CameraController;

	OperationMode m_Operation;
	TranslationMode m_Translation;
	bool m_Is2DMode = true;
	char m_GridAxis = 'z';

	bool m_ShowCollision = false;
	bool m_ShowFrameRate = true;
	bool m_ShowGrid = false;
	bool m_ShowStats = false;
	bool m_ShowShadows = true;
	bool m_ShowLighting = true;
	bool m_ShowReflections = true;

	int m_SelectedViewportShading = 0;

	Ref<HierarchyPanel> m_HierarchyPanel;
	Entity m_HoveredEntity;

	int m_PixelData;

	Ref<TilemapEditor> m_TilemapEditor;
	bool m_RightClickMenuOpen = false;

	Ref<Mesh> m_GridMesh;

	Ref<EditComponentCommand<TransformComponent>> m_EditTransformCommand = nullptr;

	bool m_Fullscreen = false;
};