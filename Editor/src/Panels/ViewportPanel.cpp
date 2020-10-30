#include "ViewportPanel.h"

#include "MainDockSpace.h"

#include "IconsFontAwesome5.h"

#include "Scene/SceneManager.h"
#include "FileSystem/FileDialog.h"

ViewportPanel::ViewportPanel(bool* show)
	:m_Show(show), Layer("Viewport")
{
	m_Framebuffer = FrameBuffer::Create({ 1920, 1080 });

	m_Mode = Mode::Select;
}

void ViewportPanel::OnAttach()
{
	//TODO: Render the scene with the camera controller when not in "play" mode.
	//TODO: Load where the camera was in the scene and load that
	m_CameraController.SetPosition({ 0.0, 0.0, 2.0 });
}

void ViewportPanel::OnUpdate(float deltaTime)
{
	PROFILE_FUNCTION();

	if (!SceneManager::s_CurrentScene)
		return;

	m_Framebuffer->Bind();
	RenderCommand::Clear();

	bool rightMouseDown = Input::IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);

	if (m_CursorDisabled)
		m_CameraController.OnUpdate(deltaTime);

	if (m_WindowHovered && rightMouseDown && !m_CursorDisabled)
	{
		Application::GetWindow().DisableCursor(); //TODO: Fix the issue that disabling the cursor jumps the mouse position
		m_CursorDisabled = true;
	}

	else if (m_CursorDisabled && !rightMouseDown)
	{
		m_CursorDisabled = false;
		Application::GetWindow().EnableCursor();
	}

	Renderer2D::ResetStats();

	SceneManager::s_CurrentScene->OnUpdate(deltaTime);

	m_Framebuffer->UnBind();
}

void ViewportPanel::OnFixedUpdate()
{
	FrameBufferSpecification spec = m_Framebuffer->GetSpecification();
	if (((uint32_t)m_ViewportSize.x != spec.Width || (uint32_t)m_ViewportSize.y != spec.Height) && (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f))
	{
		m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_CameraController.SetAspectRatio(m_ViewportSize.x / m_ViewportSize.y);

		SceneManager::s_CurrentScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
	}
}

void ViewportPanel::OnImGuiRender()
{
	PROFILE_FUNCTION();

	if (!*m_Show || SceneManager::s_CurrentScene == nullptr)
	{
		return;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	ImGui::SetNextWindowSize(ImVec2(800, 800), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(30, 30), ImGuiCond_FirstUseEver);

	ImVec2 pos;
	ImGuiIO& io = ImGui::GetIO();
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar;

	if(SceneManager::s_CurrentScene->IsDirty())
		flags |= ImGuiWindowFlags_UnsavedDocument;

	bool viewShown = ImGui::Begin(ICON_FA_BORDER_ALL" Viewport", m_Show, flags);
	if (viewShown)
	{
		HandleKeyboardInputs();

		m_WindowHovered = ImGui::IsWindowHovered();
		m_WindowFocussed = ImGui::IsWindowFocused();

		if (m_WindowHovered)
		{
			if (!Input::IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_COUNT); //HACK: this is to stop imgui from changing the cursor back to something every frame
				Application::GetWindow().SetCursor(Cursors::CrossHair);
			}
		}

		if (m_WindowFocussed)
		{
			MainDockSpace::SetFocussedWindow(this);
		}

		if (m_WindowHovered && io.MouseWheel != 0.0f)
			m_CameraController.OnMouseWheel(io.MouseWheel);

		ImVec2 panelSize = ImGui::GetContentRegionAvail();
		if (m_ViewportSize.x != panelSize.x || m_ViewportSize.y != panelSize.y)
		{
			m_ViewportSize = panelSize;
		}

		pos = ImGui::GetCursorScreenPos();
		ImVec2 mouse_pos = ImGui::GetMousePos();

		m_RelativeMousePosition = { mouse_pos.x - ImGui::GetWindowPos().x - 1.0f, mouse_pos.y - ImGui::GetWindowPos().y - 8.0f - ImGui::GetFontSize() };

		m_CameraController.OnMouseMotion(m_RelativeMousePosition);

		uint64_t tex = (uint64_t)m_Framebuffer->GetColourAttachment();

		ImGui::Image((void*)tex, m_ViewportSize, ImVec2(0, 1), ImVec2(1, 0));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Asset", ImGuiDragDropFlags_None))
			{
				std::filesystem::path* file = (std::filesystem::path*)payload->Data;

				if (file->extension() == ".staticMesh")
				{
					std::string entityName = file->filename().string();
					entityName = entityName.substr(0, entityName.find_last_of('.'));
					Entity staticMeshEntity = SceneManager::s_CurrentScene->CreateEntity(entityName);

					Mesh mesh(*file);

					m_ShaderLibrary.Load("NormalMap");
					Material material(m_ShaderLibrary.Get("NormalMap"));

					material.AddTexture(Texture2D::Create(Application::GetWorkingDirectory().string() + "\\resources\\UVChecker.png"), 0);

					staticMeshEntity.AddComponent<StaticMeshComponent>(mesh, material);
				}
				CLIENT_DEBUG(file->string());
			}
			ImGui::EndDragDropTarget();
		}
	}
	ImGui::End();
	ImGui::PopStyleVar();

	if (viewShown)
	{
		ImGui::SetNextWindowPos(ImVec2(pos.x + ImGui::GetStyle().ItemSpacing.x, pos.y + ImGui::GetStyle().ItemSpacing.y));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.2f));
		ImGui::Begin("FPS", m_Show, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration
			| ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize
			| ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing
			| ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoInputs);

		ImGui::Text("%.1f", io.Framerate);
		ImGui::End();
		ImGui::PopStyleColor();
	}
}

void ViewportPanel::Copy()
{
	// TODO: viewport copy
	CLIENT_DEBUG("Copied");
}

void ViewportPanel::Cut()
{
	// TODO: viewport cut
	CLIENT_DEBUG("Cut");

	SceneManager::s_CurrentScene->MakeDirty();
}

void ViewportPanel::Paste()
{
	//TODO: viewport paste
	CLIENT_DEBUG("Pasted {0}", std::string(ImGui::GetClipboardText()));

	SceneManager::s_CurrentScene->MakeDirty();
}

void ViewportPanel::Duplicate()
{
	//TODO: viewport duplicate
	CLIENT_DEBUG("Duplicated");

	SceneManager::s_CurrentScene->MakeDirty();
}

void ViewportPanel::Delete()
{
	//TODO: viewport delete
	CLIENT_DEBUG("Deleted");

	SceneManager::s_CurrentScene->MakeDirty();
}

void ViewportPanel::SelectAll()
{
}

bool ViewportPanel::HasSelection() const
{
	return false;
}

bool ViewportPanel::CanUndo() const
{
	//TODO: viewport undo list
	return false;
}

bool ViewportPanel::CanRedo() const
{
	return false;
}

void ViewportPanel::Undo(int astep)
{
	CLIENT_DEBUG("Undid");

	SceneManager::s_CurrentScene->MakeDirty();
}

void ViewportPanel::Redo(int astep)
{
	CLIENT_DEBUG("Redid");

	SceneManager::s_CurrentScene->MakeDirty();
}

void ViewportPanel::Save()
{
	//if (!m_SceneDirty)
	//	return;
	CLIENT_DEBUG("Saving...");

	SceneManager::s_CurrentScene->Save(false);
}

void ViewportPanel::SaveAs()
{
	CLIENT_DEBUG("Saving As...");

	SceneManager::s_CurrentScene->Save(SaveAsDialog(L"Save Scene As...", L"Scene (.scene)\0*.scene\0"), false);
}

void ViewportPanel::HandleKeyboardInputs()
{
	ImGuiIO& io = ImGui::GetIO();
	auto shift = io.KeyShift;
	auto ctrl = io.ConfigMacOSXBehaviors ? io.KeySuper : io.KeyCtrl;
	auto alt = io.ConfigMacOSXBehaviors ? io.KeyCtrl : io.KeyAlt;

	if (ImGui::IsWindowFocused())
	{
		io.WantCaptureKeyboard = true;
		io.WantCaptureMouse = true;

		if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Z)))
			Undo();
		else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Y)))
			Redo();
		else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
			Delete();
		else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_C)))
			Copy();
		else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_V)))
			Paste();
		else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_X)))
			Cut();
		else if (ctrl && !shift && !alt && ImGui::IsKeyPressed('D'))
			Duplicate();
		else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_A)))
			SelectAll();
		else if (ctrl && !shift && !alt && ImGui::IsKeyPressed('S'))
			Save();
		else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed('Q'))
			m_Mode = Mode::Select;
		else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed('W'))
			m_Mode = Mode::Move;
		else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed('E'))
			m_Mode = Mode::Rotate;
		else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed('R'))
			m_Mode = Mode::Scale;
	}
}
