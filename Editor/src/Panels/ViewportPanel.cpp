#include "ViewportPanel.h"

#include "MainDockSpace.h"

#include "IconsFontAwesome5.h"

#include "Engine.h"
#include "FileSystem/FileDialog.h"

#include "HierarchyPanel.h"
#include "Scene/Components/Components.h"

#include "ImGui/ImGuizmo.h"

ViewportPanel::ViewportPanel(bool* show, HierarchyPanel* hierarchyPanel)
	:m_Show(show), Layer("Viewport"), m_HierarchyPanel(hierarchyPanel)
{
	FrameBufferSpecification frameBufferSpecificationEditorCamera = { 1920, 1080 };
	frameBufferSpecificationEditorCamera.Attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER, FrameBufferTextureFormat::Depth };
	m_Framebuffer = FrameBuffer::Create(frameBufferSpecificationEditorCamera);

	FrameBufferSpecification frameBufferSpecificationPreview = { 256, 144 };
	frameBufferSpecificationPreview.Attachments = { FrameBufferTextureFormat::RGBA8 };
	m_CameraPreview = FrameBuffer::Create(frameBufferSpecificationPreview);

	m_Mode = Mode::Select;

	m_Framebuffer->ClearAttachment(1, -1);
}

void ViewportPanel::OnAttach()
{
	//TODO: Render the scene with the camera controller when not in "play" mode.
	//TODO: Load where the camera was in the scene and load that
	m_CameraController.SetPosition({ 0.0, 0.0, 0.0 });
}
void ViewportPanel::OnUpdate(float deltaTime)
{
	PROFILE_FUNCTION();

	if (!SceneManager::IsSceneLoaded())
		return;

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

	FrameBufferSpecification spec = m_Framebuffer->GetSpecification();
	if (((uint32_t)m_ViewportSize.x != spec.Width || (uint32_t)m_ViewportSize.y != spec.Height) && (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f))
	{
		m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
	}

	m_Framebuffer->Bind();
	RenderCommand::Clear();
	m_Framebuffer->ClearAttachment(1, -1);
	SceneManager::CurrentScene()->Render(m_Framebuffer, m_CameraController.GetTransformMatrix(), m_CameraController.GetCamera()->GetProjectionMatrix());

	if (m_RelativeMousePosition.x >= 0.0f && m_RelativeMousePosition.y >= 0.0f
		&& m_RelativeMousePosition.x < m_ViewportSize.x && m_RelativeMousePosition.y < m_ViewportSize.y)
	{
		m_Framebuffer->Bind();
		m_PixelData = m_Framebuffer->ReadPixel(1, (int)m_RelativeMousePosition.x, (int)(m_ViewportSize.y - m_RelativeMousePosition.y));
		//m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, SceneManager::CurrentScene());
		m_Framebuffer->UnBind();
	}

	if ((entt::entity)m_HierarchyPanel->GetSelectedEntity() != entt::null)
	{
		if (m_HierarchyPanel->GetSelectedEntity().HasComponent<CameraComponent>())
		{
			CameraComponent& cameraComp = m_HierarchyPanel->GetSelectedEntity().GetComponent<CameraComponent>();
			TransformComponent& transformComp = m_HierarchyPanel->GetSelectedEntity().GetComponent<TransformComponent>();
			Matrix4x4 view = Matrix4x4::Translate(transformComp.Position) * Matrix4x4::Rotate({ transformComp.Rotation });
			Matrix4x4 projection = cameraComp.Camera.GetProjectionMatrix();
			m_CameraPreview->Bind();
			RenderCommand::Clear();
			SceneManager::CurrentScene()->Render(m_CameraPreview, view, projection);
		}
	}

	Renderer2D::ResetStats();

}

void ViewportPanel::OnFixedUpdate()
{
}

void ViewportPanel::OnImGuiRender()
{
	PROFILE_FUNCTION();

	if (!*m_Show || !SceneManager::IsSceneLoaded())
	{
		return;
	}

	ImGuizmo::BeginFrame();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	ImGui::SetNextWindowSize(ImVec2(800, 800), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(30, 30), ImGuiCond_FirstUseEver);

	ImVec2 pos;
	ImGuiIO& io = ImGui::GetIO();
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar;

	if (SceneManager::CurrentScene()->IsDirty())
		flags |= ImGuiWindowFlags_UnsavedDocument;

	bool viewShown = ImGui::Begin(ICON_FA_BORDER_ALL" Viewport", m_Show, flags);
	if (viewShown)
	{
		m_WindowHovered = ImGui::IsWindowHovered();
		m_WindowFocussed = ImGui::IsWindowFocused();

		HandleKeyboardInputs();

		if (m_WindowHovered)
		{
			if (!Input::IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_COUNT); //HACK: this is to stop imgui from changing the cursor back to something every frame
				Application::GetWindow().SetCursor(Cursors::CrossHair);


			}

			if (Input::IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE) || Input::IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
			{
				ImGui::SetWindowFocus();
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
			m_CameraController.SetViewPortSize(Vector2f(m_ViewportSize.x, m_ViewportSize.y));
			m_CameraController.SetAspectRatio(m_ViewportSize.x / m_ViewportSize.y);
		}

		pos = ImGui::GetCursorScreenPos();
		ImVec2 mouse_pos = ImGui::GetMousePos();

		uint64_t tex = (uint64_t)m_Framebuffer->GetColourAttachment();

		ImGui::Image((void*)tex, m_ViewportSize, ImVec2(0, 1), ImVec2(1, 0));
		ImVec2 window_pos = ImGui::GetItemRectMin();

		m_RelativeMousePosition = { mouse_pos.x - window_pos.x, mouse_pos.y - window_pos.y };
		m_CameraController.OnMouseMotion(m_RelativeMousePosition);

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Asset", ImGuiDragDropFlags_None))
			{
				std::filesystem::path* file = (std::filesystem::path*)payload->Data;

				if (file->extension() == ".staticMesh")
				{
					std::string entityName = file->filename().string();
					entityName = entityName.substr(0, entityName.find_last_of('.'));
					Entity staticMeshEntity = SceneManager::CurrentScene()->CreateEntity(entityName);

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

		if ((entt::entity)m_HierarchyPanel->GetSelectedEntity() != entt::null)
		{
			if (m_HierarchyPanel->GetSelectedEntity().HasComponent<CameraComponent>())
			{
				ImGui::SetNextWindowPos(ImVec2(pos.x - ImGui::GetStyle().ItemSpacing.x + m_ViewportSize.x - m_CameraPreview->GetSpecification().Width,
					pos.y - ImGui::GetStyle().ItemSpacing.y + m_ViewportSize.y - m_CameraPreview->GetSpecification().Height - 24.0f));
				ImGui::Begin(m_HierarchyPanel->GetSelectedEntity().GetComponent<TagComponent>().Tag.c_str(), NULL,
					ImGuiWindowFlags_NoDocking | ImGuiTabBarFlags_NoTooltip
					| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize
					| ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing
					| ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoInputs);
				uint64_t cameraTex = (uint64_t)m_CameraPreview->GetColourAttachment();
				ImGui::Image((void*)cameraTex, ImVec2((float)m_CameraPreview->GetSpecification().Width, (float)m_CameraPreview->GetSpecification().Height), ImVec2(0, 1), ImVec2(1, 0));
				ImGui::End();
			}
		}

		// Gizmos
		Entity selectedEntity = m_HierarchyPanel->GetSelectedEntity();

		if (selectedEntity)
		{
			ImGuizmo::SetOrthographic(true);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(window_pos.x, window_pos.y, (float)panelSize.x, (float)panelSize.y);
			Matrix4x4 cameraViewMat = Matrix4x4::Inverse(m_CameraController.GetTransformMatrix());
			Matrix4x4 cameraProjectionMat = m_CameraController.GetCamera()->GetProjectionMatrix();

			TransformComponent& transformComp = selectedEntity.GetComponent<TransformComponent>();
			Matrix4x4 transformMat = Matrix4x4::Translate(transformComp.Position)
				* Matrix4x4::Rotate(Quaternion(transformComp.Rotation))
				* Matrix4x4::Scale(transformComp.Scale);

			cameraViewMat.Transpose();
			cameraProjectionMat.Transpose();
			transformMat.Transpose();

			float cameraView[16] =
			{ cameraViewMat[0][0], cameraViewMat[0][1],cameraViewMat[0][2],cameraViewMat[0][3],
			  cameraViewMat[1][0], cameraViewMat[1][1],cameraViewMat[1][2],cameraViewMat[1][3],
			  cameraViewMat[2][0], cameraViewMat[2][1],cameraViewMat[2][2],cameraViewMat[2][3],
			  cameraViewMat[3][0], cameraViewMat[3][1],cameraViewMat[3][2],cameraViewMat[3][3] };

			float cameraProjection[16] =
			{ cameraProjectionMat[0][0], cameraProjectionMat[0][1],cameraProjectionMat[0][2],cameraProjectionMat[0][3],
			  cameraProjectionMat[1][0], cameraProjectionMat[1][1],cameraProjectionMat[1][2],cameraProjectionMat[1][3],
			  cameraProjectionMat[2][0], cameraProjectionMat[2][1],cameraProjectionMat[2][2],cameraProjectionMat[2][3],
			  cameraProjectionMat[3][0], cameraProjectionMat[3][1],cameraProjectionMat[3][2],cameraProjectionMat[3][3] };

			float transform[16] =
			{
			  transformMat[0][0], transformMat[0][1],transformMat[0][2],transformMat[0][3],
			  transformMat[1][0], transformMat[1][1],transformMat[1][2],transformMat[1][3],
			  transformMat[2][0], transformMat[2][1],transformMat[2][2],transformMat[2][3],
			  transformMat[3][0], transformMat[3][1],transformMat[3][2],transformMat[3][3] };


			bool snap = Input::IsKeyPressed(KEY_LEFT_CONTROL);
			float snapValue = 0.5f;
			if (m_Mode == Mode::Rotate)
				snapValue = 10.0f;

			//TODO: add a get bounds function method from sprite, camera and mesh
			float bounds[6] = { -0.5f, -0.5f, -0.0f, 0.5f, 0.5f, 0.0f };

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::OPERATION gizmoMode = ImGuizmo::BOUNDS;
			switch (m_Mode)
			{
			case ViewportPanel::Mode::Select:
				gizmoMode = ImGuizmo::BOUNDS;
				break;
			case ViewportPanel::Mode::Move:
				gizmoMode = ImGuizmo::OPERATION::TRANSLATE;
				break;
			case ViewportPanel::Mode::Rotate:
				gizmoMode = ImGuizmo::OPERATION::ROTATE;
				break;
			case ViewportPanel::Mode::Scale:
				gizmoMode = ImGuizmo::OPERATION::SCALE;
				break;
			default:
				break;
			}

			CORE_ASSERT(!std::isnan(transform[0]), "Transform is not a number!");
			ImGuizmo::Manipulate(cameraView, cameraProjection, gizmoMode, ImGuizmo::LOCAL, transform, NULL, snap ? &snapValues[0] : NULL, gizmoMode == ImGuizmo::BOUNDS ? bounds : NULL, snap ? snapValues : NULL);

			CORE_ASSERT(!std::isnan(transform[0]), "Transform is not a number!");

			if (ImGuizmo::IsUsing())
			{
				float translation[3], rotation[3], scale[3];

				ImGuizmo::DecomposeMatrixToComponents(transform, translation, rotation, scale);

				CORE_ASSERT(!std::isnan(translation[0]), "Translation is not a number!");

				Vector3f deltaRotation = Vector3f(rotation[0], rotation[1], rotation[2]) - transformComp.Rotation;
				if (gizmoMode == ImGuizmo::OPERATION::TRANSLATE)
				{
					transformComp.Position = Vector3f(translation[0], translation[1], translation[2]);
				}
				if (gizmoMode == ImGuizmo::OPERATION::ROTATE)
				{
					transformComp.Rotation += deltaRotation;
				}
				if (gizmoMode == ImGuizmo::OPERATION::SCALE)
				{
					transformComp.Scale = Vector3f(scale[0], scale[1], scale[2]);
				}
			}
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
		ImGui::Text("%i", m_PixelData);
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

	SceneManager::CurrentScene()->MakeDirty();
}

void ViewportPanel::Paste()
{
	//TODO: viewport paste
	CLIENT_DEBUG("Pasted {0}", std::string(ImGui::GetClipboardText()));

	SceneManager::CurrentScene()->MakeDirty();
}

void ViewportPanel::Duplicate()
{
	//TODO: viewport duplicate
	CLIENT_DEBUG("Duplicated");

	SceneManager::CurrentScene()->MakeDirty();
}

void ViewportPanel::Delete()
{
	//TODO: viewport delete
	CLIENT_DEBUG("Deleted");

	SceneManager::CurrentScene()->MakeDirty();
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

	SceneManager::CurrentScene()->MakeDirty();
}

void ViewportPanel::Redo(int astep)
{
	CLIENT_DEBUG("Redid");

	SceneManager::CurrentScene()->MakeDirty();
}

void ViewportPanel::Save()
{
	//if (!m_SceneDirty)
	//	return;
	CLIENT_DEBUG("Saving...");

	SceneManager::CurrentScene()->Save(false);
}

void ViewportPanel::SaveAs()
{
	CLIENT_DEBUG("Saving As...");
	std::optional<std::wstring> scenePath = FileDialog::SaveAs(L"Save Scene As...", L"Scene (.scene)\0*.scene\0");
	if (scenePath)
		SceneManager::CurrentScene()->Save(scenePath.value(), false);
}

void ViewportPanel::HandleKeyboardInputs()
{
	ImGuiIO& io = ImGui::GetIO();
	auto shift = io.KeyShift;
	auto ctrl = io.ConfigMacOSXBehaviors ? io.KeySuper : io.KeyCtrl;
	auto alt = io.ConfigMacOSXBehaviors ? io.KeyCtrl : io.KeyAlt;

	if (m_WindowHovered)
	{
		if (!ctrl && !shift && !alt && ImGui::IsKeyPressed('Q'))
			m_Mode = Mode::Select;
		else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed('W'))
			m_Mode = Mode::Move;
		else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed('E'))
			m_Mode = Mode::Rotate;
		else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed('R'))
			m_Mode = Mode::Scale;
	}

	if (m_WindowFocussed)
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
	}

}
