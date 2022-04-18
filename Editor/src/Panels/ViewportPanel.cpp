#include "ViewportPanel.h"

#include "MainDockSpace.h"

#include "IconsFontAwesome5.h"
#include "Fonts/IconsMaterialDesignIcons.h"
#include "IconsMaterialDesign.h"
#include "Fonts/Fonts.h"

#include "Engine.h"
#include "FileSystem/FileDialog.h"
#include "Viewers/ViewerManager.h"

#include "HierarchyPanel.h"
#include "Scene/Components/Components.h"

#include "ImGui/ImGuizmo.h"

ViewportPanel::ViewportPanel(bool* show, HierarchyPanel* hierarchyPanel)
	:m_Show(show), Layer("Viewport"), m_HierarchyPanel(hierarchyPanel),
	m_Operation(OperationMode::Select), m_Translation(TranslationMode::Local)
{
	FrameBufferSpecification frameBufferSpecificationEditorCamera = { 1920, 1080 };
	frameBufferSpecificationEditorCamera.attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER, FrameBufferTextureFormat::Depth };
	m_Framebuffer = FrameBuffer::Create(frameBufferSpecificationEditorCamera);

	FrameBufferSpecification frameBufferSpecificationPreview = { 256, 144 };
	frameBufferSpecificationPreview.attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER };
	m_CameraPreview = FrameBuffer::Create(frameBufferSpecificationPreview);

	m_Framebuffer->ClearAttachment(1, -1);
}

void ViewportPanel::OnAttach()
{
	//TODO: Load where the camera was in the scene and load that
	m_CameraController.SetPosition({ 0.0, 0.0, 0.0 });

	Settings::SetDefaultBool("Viewport", "ShowCollision", false);
	Settings::SetDefaultBool("Viewport", "ShowFps", true);
	Settings::SetDefaultBool("Viewport", "ShowStats", false);
	Settings::SetDefaultBool("Viewport", "ShowGrid", false);
	Settings::SetDefaultBool("Viewport", "ShowShadows", true);
	Settings::SetDefaultBool("Viewport", "ShowLighting", true);
	Settings::SetDefaultBool("Viewport", "ShowReflections", true);
	Settings::SetDefaultBool("Viewport", "Is2D", true);

	m_ShowCollision = Settings::GetBool("Viewport", "ShowCollision");
	m_ShowFrameRate = Settings::GetBool("Viewport", "ShowFps");
	m_ShowStats = Settings::GetBool("Viewport", "ShowStats");
	m_ShowGrid = Settings::GetBool("Viewport", "ShowGrid");
	m_ShowShadows = Settings::GetBool("Viewport", "ShowShadows");
	m_ShowLighting = Settings::GetBool("Viewport", "ShowLighting");
	m_ShowReflections = Settings::GetBool("Viewport", "ShowReflections");
	m_Is2DMode = Settings::GetBool("Viewport", "Is2D");
}

void ViewportPanel::OnDetach()
{
	Settings::SetBool("Viewport", "ShowCollision", m_ShowCollision);
	Settings::SetBool("Viewport", "ShowFps", m_ShowFrameRate);
	Settings::SetBool("Viewport", "ShowStats", m_ShowStats);
	Settings::SetBool("Viewport", "ShowGrid", m_ShowGrid);
	Settings::SetBool("Viewport", "ShowShadows", m_ShowShadows);
	Settings::SetBool("Viewport", "ShowLighting", m_ShowLighting);
	Settings::SetBool("Viewport", "ShowReflections", m_ShowReflections);
	Settings::SetBool("Viewport", "Is2D", m_Is2DMode);
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
	if (((uint32_t)m_ViewportSize.x != spec.width || (uint32_t)m_ViewportSize.y != spec.height) && (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f))
	{
		m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		SceneManager::CurrentScene()->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
	}

	m_Framebuffer->Bind();
	RenderCommand::Clear();
	m_Framebuffer->ClearAttachment(1, -1);
	m_HoveredEntity = Entity();

	switch (SceneManager::GetSceneState())
	{
	case SceneState::Play:
		SceneManager::CurrentScene()->Render(m_Framebuffer);
		break;
	case SceneState::Pause:
		SceneManager::CurrentScene()->Render(m_Framebuffer);
		break;
		[[fallthrough]];
	case SceneState::SimulatePause:
		[[fallthrough]];
	case SceneState::Edit:
	case SceneState::Simulate:
	{
		SceneManager::CurrentScene()->Render(m_Framebuffer, m_CameraController.GetTransformMatrix(), m_CameraController.GetCamera()->GetProjectionMatrix());

		if (m_RelativeMousePosition.x >= 0.0f && m_RelativeMousePosition.y > 0.0f
			&& m_RelativeMousePosition.x < m_ViewportSize.x && m_RelativeMousePosition.y < m_ViewportSize.y)
		{
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				m_Framebuffer->Bind();
				m_PixelData = m_Framebuffer->ReadPixel(1, (int)m_RelativeMousePosition.x, (int)(m_ViewportSize.y - m_RelativeMousePosition.y));
				m_HoveredEntity = m_PixelData == -1 ? Entity() : Entity((entt::entity)m_PixelData, SceneManager::CurrentScene());
				if (!ImGuizmo::IsUsing() && !ImGuizmo::IsOver())
					m_HierarchyPanel->SetSelectedEntity(m_HoveredEntity);
				m_Framebuffer->UnBind();
			}
		}

		SceneManager::CurrentScene()->SetShowDebug(m_ShowCollision);

		// Debug render pass
		m_Framebuffer->Bind();
		Renderer2D::BeginScene(m_CameraController.GetTransformMatrix(), m_CameraController.GetCamera()->GetProjectionMatrix());
		if ((entt::entity)m_HierarchyPanel->GetSelectedEntity() != entt::null)
		{
			Entity selectedEntity = m_HierarchyPanel->GetSelectedEntity();
			TransformComponent& transformComp = selectedEntity.GetComponent<TransformComponent>();

			if (selectedEntity.HasComponent<CameraComponent>())
			{
				CameraComponent& cameraComp = selectedEntity.GetComponent<CameraComponent>();
				Matrix4x4 view = Matrix4x4::Translate(transformComp.GetWorldPosition()) * Matrix4x4::Rotate({ transformComp.rotation });
				Matrix4x4 projection = cameraComp.Camera.GetProjectionMatrix();
				m_CameraPreview->Bind();
				RenderCommand::Clear();
				SceneManager::CurrentScene()->Render(m_CameraPreview, view, projection);
			}
			if (selectedEntity.HasComponent<CircleCollider2DComponent>())
			{
				CircleCollider2DComponent& circleComp = selectedEntity.GetComponent<CircleCollider2DComponent>();

				float scale = circleComp.Radius * std::max(transformComp.scale.x, transformComp.scale.y);

				Matrix4x4 transform = transformComp.GetParentMatrix()
					* Matrix4x4::Translate(transformComp.position)
					* Matrix4x4::Rotate(Vector3f(0.0f, 0.0f, transformComp.rotation.z))
					* Matrix4x4::Translate(Vector3f(circleComp.Offset.x, circleComp.Offset.y, 0.001f))
					* Matrix4x4::Scale(Vector3f(scale, scale, 1.0f));

				Renderer2D::DrawHairLineCircle(transform, 60, Colours::LIME_GREEN, selectedEntity);
			}

			if (selectedEntity.HasComponent<BoxCollider2DComponent>())
			{
				BoxCollider2DComponent& boxComp = selectedEntity.GetComponent<BoxCollider2DComponent>();

				Matrix4x4 transform = transformComp.GetParentMatrix()
					* Matrix4x4::Translate(transformComp.position)
					* Matrix4x4::Rotate(Vector3f(0.0f, 0.0f, transformComp.rotation.z))
					* Matrix4x4::Translate(Vector3f(boxComp.Offset.x, boxComp.Offset.y, 0.001f))
					* Matrix4x4::Scale(Vector3f(boxComp.Size.x * transformComp.scale.x * 2, boxComp.Size.y * transformComp.scale.y * 2, 1.0f));

				Renderer2D::DrawHairLineRect(transform, Colours::LIME_GREEN, selectedEntity);
			}
		}
		Renderer2D::EndScene();
		m_Framebuffer->UnBind();
		break;
	}
	default:
		break;
	}
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
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar;

	if (SceneManager::CurrentScene()->IsDirty())
		flags |= ImGuiWindowFlags_UnsavedDocument;

	bool viewShown = ImGui::Begin(ICON_FA_BORDER_ALL" Viewport", m_Show, flags);
	if (viewShown)
	{
		if (ImGui::BeginMenuBar())
		{
			ImGui::PopStyleVar();
			bool selected = false;

			// Select ------------------------------------------------------------------------------
			selected = m_Operation == OperationMode::Select;
			if (!selected)
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

			if (ImGui::Button(ICON_FA_MOUSE_POINTER))
				m_Operation = OperationMode::Select;

			if (!selected)
				ImGui::PopStyleColor();
			ImGui::Tooltip("Select (Q)");

			// Move --------------------------------------------------------------------------------
			selected = m_Operation == OperationMode::Move;
			if (!selected)
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

			if (ImGui::Button(ICON_FA_ARROWS_ALT))
				m_Operation = OperationMode::Move;

			if (!selected)
				ImGui::PopStyleColor();
			ImGui::Tooltip("Move (W)");

			// Rotate ------------------------------------------------------------------------------
			selected = m_Operation == OperationMode::Rotate;
			if (!selected)
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

			if (ImGui::Button(ICON_MDI_ROTATE_ORBIT))
				m_Operation = OperationMode::Rotate;

			if (!selected)
				ImGui::PopStyleColor();
			ImGui::Tooltip("Rotate (E)");

			// Scale -------------------------------------------------------------------------------
			selected = m_Operation == OperationMode::Scale;
			if (!selected)
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

			if (ImGui::Button(ICON_MDI_RESIZE))
				m_Operation = OperationMode::Scale;

			if (!selected)
				ImGui::PopStyleColor();
			ImGui::Tooltip("Scale (R)");

			ImGui::Separator();
			if (m_Translation == TranslationMode::Local)
			{
				if (ImGui::Button(ICON_FA_CUBE))
					m_Translation = TranslationMode::World;
			}
			else
			{
				if (ImGui::Button(ICON_FA_GLOBE_EUROPE))
					m_Translation = TranslationMode::Local;
			}
			ImGui::Separator();

			selected = m_Is2DMode;
			if (!selected)
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			if (ImGui::Button("2D"))
			{
				m_Is2DMode = !m_Is2DMode;
				if (m_Is2DMode)
					m_GridAxis = 'z';
				else
					m_GridAxis = 'y';
				m_CameraController.SwitchCamera(!m_Is2DMode);
			}
			if (!selected)
				ImGui::PopStyleColor();

			ImGui::Separator();
			
			if (ImGui::BeginMenu("Show"))
			{
				ImGui::MenuItem("Collision", "", &m_ShowCollision);
				ImGui::MenuItem("FPS", "", &m_ShowFrameRate);
				ImGui::MenuItem("Statistics", "", &m_ShowStats);
				ImGui::MenuItem("Grid", "", &m_ShowGrid);
				ImGui::MenuItem("Shadows", "", &m_ShowShadows, false);
				ImGui::MenuItem("Lighting", "", &m_ShowLighting, false);
				ImGui::MenuItem("Reflections", "", &m_ShowReflections, false);

				ImGui::EndMenu();
			}
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::EndMenuBar();
		}
		ImVec2 topLeft = ImGui::GetCursorPos();
		m_WindowHovered = ImGui::IsWindowHovered();
		m_WindowFocussed = ImGui::IsWindowFocused();

		HandleKeyboardInputs();

		if (m_WindowHovered)
		{
			if (!Input::IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
			{
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
			SceneManager::CurrentScene()->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		pos = ImGui::GetCursorScreenPos();
		ImVec2 mouse_pos = ImGui::GetMousePos();

		uint64_t tex = (uint64_t)m_Framebuffer->GetColourAttachment();

		ImGui::Image((void*)tex, m_ViewportSize, ImVec2(0, 1), ImVec2(1, 0));
		ImVec2 window_pos = ImGui::GetItemRectMin();

		m_RelativeMousePosition = { mouse_pos.x - window_pos.x, mouse_pos.y - window_pos.y };
		m_CameraController.OnMouseMotion(m_RelativeMousePosition);

		if (SceneManager::GetSceneState() == SceneState::Edit)
		{
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Asset", ImGuiDragDropFlags_None))
				{
					std::filesystem::path* file = (std::filesystem::path*)payload->Data;

					if (ViewerManager::GetFileType(*file) == FileType::MESH)
					{
						std::string entityName = file->filename().string();
						entityName = entityName.substr(0, entityName.find_last_of('.'));
						Entity staticMeshEntity = SceneManager::CurrentScene()->CreateEntity(entityName);

						Mesh mesh(*file);

						m_ShaderLibrary.Load("Standard");
						Material material(m_ShaderLibrary.Get("Standard"));

						material.AddTexture(Texture2D::Create(Application::GetWorkingDirectory() / "resources" / "UVChecker.png"), 0);

						staticMeshEntity.AddComponent<StaticMeshComponent>(mesh, material);
					}
					else if (file->extension() == ".tmx")
					{
						Tilemap tilemap;
						tilemap.Load(*file);

						tilemap.Save(file->replace_filename("Test Copy.tmx"));
					}
					else if (file->extension() == ".scene")
					{
						SceneManager::ChangeScene(*file);
					}

					CLIENT_DEBUG(file->string());
				}
				ImGui::EndDragDropTarget();
			}
		}



		if (SceneManager::GetSceneState() != SceneState::Play && SceneManager::GetSceneState() != SceneState::Pause)
		{
			ImGuizmo::SetOrthographic(m_Is2DMode);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(window_pos.x, window_pos.y, (float)panelSize.x, (float)panelSize.y);

			Matrix4x4 cameraViewMat = Matrix4x4::Inverse(m_CameraController.GetTransformMatrix());
			Matrix4x4 cameraProjectionMat = m_CameraController.GetCamera()->GetProjectionMatrix();
			cameraViewMat.Transpose();
			cameraProjectionMat.Transpose();

			if (m_ShowGrid)
			{
				Matrix4x4 gridTransform;
				// TODO: draw a better grid instead of an imgui overlay
				//gridTransform = Matrix4x4::RotateY((float)DegToRad(angle));
				/*if (m_GridAxis == 'x')
					gridTransform = Matrix4x4::RotateZ((float)DegToRad(angle));
				if (m_GridAxis == 'z')
					gridTransform = Matrix4x4::RotateX((float)DegToRad(angle));
				gridTransform.Transpose();*/
				ImGuizmo::DrawGrid(cameraViewMat.m16, cameraProjectionMat.m16, gridTransform.m16, 100.0f);
			}

			if ((entt::entity)m_HierarchyPanel->GetSelectedEntity() != entt::null)
			{
				Entity selectedEntity = m_HierarchyPanel->GetSelectedEntity();
				TransformComponent& transformComp = selectedEntity.GetTransform();

				if (m_HierarchyPanel->IsFocused())
				{
					m_CameraController.LookAt(transformComp.GetWorldPosition(), transformComp.scale.x); // TODO: get the bounds of the object and offset the camera by that amount
					m_HierarchyPanel->HasFocused();
				}

				// Draw a camera preview if the selected entity has a camera
				if (m_HierarchyPanel->GetSelectedEntity().HasComponent<CameraComponent>())
				{
					ImVec2 cameraPreviewPosition = ImVec2(pos.x - ImGui::GetStyle().ItemSpacing.x - 1 + m_ViewportSize.x - m_CameraPreview->GetSpecification().width,
						pos.y - ImGui::GetStyle().ItemSpacing.y + m_ViewportSize.y - m_CameraPreview->GetSpecification().height - 24.0f);

					ImU32 color = ((ImU32)(ImGui::GetStyle().Colors[ImGuiCol_TitleBg].x * 255.0f)) |
						((ImU32)(ImGui::GetStyle().Colors[ImGuiCol_TitleBg].y * 255.0f) << 8) |
						((ImU32)(ImGui::GetStyle().Colors[ImGuiCol_TitleBg].z * 255.0f) << 16) |
						255 << 24;

					ImGui::GetWindowDrawList()->AddRectFilled(cameraPreviewPosition,
						ImVec2(cameraPreviewPosition.x + m_CameraPreview->GetSpecification().width + 2,
							cameraPreviewPosition.y + m_CameraPreview->GetSpecification().height + 24.0f),
						color, ImGui::GetStyle().WindowRounding);

					uint64_t cameraTex = (uint64_t)m_CameraPreview->GetColourAttachment();
					float cameraCursorPosition = topLeft.x - ImGui::GetStyle().ItemSpacing.x + m_ViewportSize.x - m_CameraPreview->GetSpecification().width;
					ImGui::SetCursorPos(ImVec2(cameraCursorPosition, topLeft.y - ImGui::GetStyle().ItemSpacing.y + m_ViewportSize.y - m_CameraPreview->GetSpecification().height - 21.0f));
					ImGui::Text(" %s", m_HierarchyPanel->GetSelectedEntity().GetName().c_str());
					ImGui::SetCursorPos(ImVec2(cameraCursorPosition, ImGui::GetCursorPosY()));
					ImGui::Image((void*)cameraTex, ImVec2((float)m_CameraPreview->GetSpecification().width, (float)m_CameraPreview->GetSpecification().height), ImVec2(0, 1), ImVec2(1, 0));
				}

				// Gizmos

				Matrix4x4 transformMat = transformComp.GetWorldMatrix();

				transformMat.Transpose();

				float translation[3], rotation[3], scale[3];

				ImGuizmo::DecomposeMatrixToComponents(transformMat.m16, translation, rotation, scale);

				bool snap = Input::IsKeyPressed(KEY_LEFT_CONTROL);
				float snapValue = 0.5f;
				if (m_Operation == OperationMode::Rotate)
					snapValue = 10.0f;

				//TODO: add a get bounds function method from sprite, camera and mesh
				float bounds[6] = { -0.5f, -0.5f, -0.0f, 0.5f, 0.5f, 0.0f };

				float snapValues[3] = { snapValue, snapValue, snapValue };

				ImGuizmo::OPERATION gizmoOperation = ImGuizmo::BOUNDS;
				switch (m_Operation)
				{
				case ViewportPanel::OperationMode::Select:
					gizmoOperation = ImGuizmo::BOUNDS;
					break;
				case ViewportPanel::OperationMode::Move:
					gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
					break;
				case ViewportPanel::OperationMode::Rotate:
					gizmoOperation = ImGuizmo::OPERATION::ROTATE;
					break;
				case ViewportPanel::OperationMode::Scale:
					gizmoOperation = ImGuizmo::OPERATION::SCALE;
					break;
					//case ViewportPanel::OperationMode::Universal:
					//	gizmoOperation = ImGuizmo::OPERATION::UNIVERSAL;
					//	break;
				default:
					break;
				}

				ImGuizmo::MODE gizmoMode = ImGuizmo::LOCAL;
				switch (m_Translation)
				{
				case ViewportPanel::TranslationMode::Local:
					gizmoMode = ImGuizmo::LOCAL;
					break;
				case ViewportPanel::TranslationMode::World:
					gizmoMode = ImGuizmo::WORLD;
					break;
				default:
					break;
				}

				ImGuizmo::Manipulate(cameraViewMat.m16, cameraProjectionMat.m16,
					gizmoOperation, gizmoMode,
					transformMat.m16,
					NULL, snap ? &snapValues[0] : NULL,
					gizmoOperation == ImGuizmo::BOUNDS ? bounds : NULL,
					snap ? snapValues : NULL);

				if (ImGuizmo::IsUsing())
				{
					transformMat.Transpose();
					Matrix4x4 parentMatrix = Matrix4x4::Inverse(transformComp.GetParentMatrix());
					transformMat = parentMatrix * transformMat;
					transformMat.Transpose();

					ImGuizmo::DecomposeMatrixToComponents(transformMat.m16, translation, rotation, scale);

					CORE_ASSERT(!std::isnan(translation[0]), "Translation is not a number!");

					Vector3f deltaRotation = Vector3f(rotation[0], rotation[1], rotation[2]) - transformComp.rotation;
					if (gizmoOperation == ImGuizmo::OPERATION::TRANSLATE)
					{
						transformComp.position = Vector3f(translation[0], translation[1], translation[2]);
					}
					if (gizmoOperation == ImGuizmo::OPERATION::ROTATE)
					{
						transformComp.rotation += deltaRotation;
					}
					if (gizmoOperation == ImGuizmo::OPERATION::SCALE)
					{
						transformComp.scale = Vector3f(scale[0], scale[1], scale[2]);
					}
				}
			}
		}

		ImVec2 statsBoxPosition = ImVec2(topLeft.x + ImGui::GetStyle().ItemSpacing.x, topLeft.y + ImGui::GetStyle().ItemSpacing.y);
		ImGui::SetCursorPos(statsBoxPosition);
		if (m_ShowFrameRate)
		{
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x + ImGui::GetStyle().ItemSpacing.x, pos.y + ImGui::GetStyle().ItemSpacing.y), ImVec2(pos.x + 50, pos.y + 24), IM_COL32(0, 0, 0, 30), 3.0f);
			ImGui::Text("%.1f", io.Framerate);
		}

		if (m_ShowStats)
		{
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x + ImGui::GetStyle().ItemSpacing.x, pos.y + ImGui::GetStyle().ItemSpacing.y), ImVec2(pos.x + 250, pos.y + (24 * 4)), IM_COL32(0, 0, 0, 30), 3.0f);
			ImGui::Text("Draw Calls: %i", Renderer2D::GetStats().drawCalls);
			ImGui::Text("Quad Count: %i", Renderer2D::GetStats().quadCount);
			ImGui::Text("Line Count: %i", Renderer2D::GetStats().lineCount);
			ImGui::Text("Hair Line Count: %i", Renderer2D::GetStats().hairLineCount);
		}
		Renderer2D::ResetStats();
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void ViewportPanel::Copy()
{
	m_HierarchyPanel->Copy();
}

void ViewportPanel::Cut()
{
	m_HierarchyPanel->Cut();
}

void ViewportPanel::Paste()
{
	m_HierarchyPanel->Paste();
}

void ViewportPanel::Duplicate()
{
	m_HierarchyPanel->Duplicate();
}

void ViewportPanel::Delete()
{
	m_HierarchyPanel->Delete();
}

void ViewportPanel::SelectAll()
{
	m_HierarchyPanel->SelectAll();
}

bool ViewportPanel::HasSelection() const
{
	return m_HierarchyPanel->GetSelectedEntity();
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

	if (m_WindowHovered && !ImGui::IsAnyMouseDown())
	{
		if (!ctrl && !shift && !alt && ImGui::IsKeyPressed('Q'))
			m_Operation = OperationMode::Select;
		else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed('W'))
			m_Operation = OperationMode::Move;
		else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed('E'))
			m_Operation = OperationMode::Rotate;
		else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed('R'))
			m_Operation = OperationMode::Scale;
		//else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed('T'))
		//	m_Operation = OperationMode::Universal;
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
