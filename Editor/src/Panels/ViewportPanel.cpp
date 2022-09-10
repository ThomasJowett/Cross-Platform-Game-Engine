#include "ViewportPanel.h"

#include "MainDockSpace.h"

#include "IconsFontAwesome5.h"
#include "Fonts/IconsMaterialDesignIcons.h"
#include "IconsMaterialDesign.h"
#include "Fonts/Fonts.h"

#include "Engine.h"
#include "FileSystem/FileDialog.h"
#include "Viewers/ViewerManager.h"
#include "Importers/ImportManager.h"

#include "HierarchyPanel.h"
#include "Scene/Components/Components.h"

#include "ImGui/ImGuizmo.h"
#include "Utilities/MathUtils.h"

ViewportPanel::ViewportPanel(bool* show, HierarchyPanel* hierarchyPanel, Ref<TilemapEditor> tilemapEditor)
	:m_Show(show), Layer("Viewport"), m_HierarchyPanel(hierarchyPanel), m_TilemapEditor(tilemapEditor),
	m_Operation(OperationMode::Select), m_Translation(TranslationMode::Local)
{
	FrameBufferSpecification frameBufferSpecificationEditorCamera = { 1920, 1080 };
	frameBufferSpecificationEditorCamera.attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER, FrameBufferTextureFormat::Depth };
	m_Framebuffer = FrameBuffer::Create(frameBufferSpecificationEditorCamera);

	FrameBufferSpecification frameBufferSpecificationPreview = { 240, 135 };
	frameBufferSpecificationPreview.attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER, FrameBufferTextureFormat::Depth };
	m_CameraPreview = FrameBuffer::Create(frameBufferSpecificationPreview);

	m_Framebuffer->ClearAttachment(1, -1);

	m_GridMaterial = CreateRef<Material>("Grid", Colours::GREY);
	m_GridMaterial->SetTwoSided(true);
	m_GridMaterial->SetTilingFactor(100.0f);

	m_GridMesh = GeometryGenerator::CreateGrid(1000.0f, 1000.0f, 2, 2, 1.0f, 1.0f);
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
	m_CameraController.SwitchCamera(!m_Is2DMode);
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
		m_CameraPreview->Resize((uint32_t)m_ViewportSize.x / 8.0f, (uint32_t)m_ViewportSize.y / 8.0f);
		SceneManager::CurrentScene()->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
	}

	m_Framebuffer->Bind();
	RenderCommand::Clear();
	m_Framebuffer->ClearAttachment(1, -1);
	m_HoveredEntity = Entity();

	switch (SceneManager::GetSceneState())
	{
		[[fallthrough]];
	case SceneState::Play:
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
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{
				m_Framebuffer->Bind();
				m_PixelData = m_Framebuffer->ReadPixel(1, (int)m_RelativeMousePosition.x, (int)(m_ViewportSize.y - m_RelativeMousePosition.y));
				m_HoveredEntity = m_PixelData == -1 ? Entity() : Entity((entt::entity)m_PixelData, SceneManager::CurrentScene());
				if (!ImGuizmo::IsUsing() && !ImGuizmo::IsOver() && !m_RightClickMenuOpen)
					m_HierarchyPanel->SetSelectedEntity(m_HoveredEntity);
				m_Framebuffer->UnBind();
			}
		}

		SceneManager::CurrentScene()->SetShowDebug(m_ShowCollision);

		Entity selectedEntity;

		if (m_HierarchyPanel->GetSelectedEntity().IsValid())
		{
			selectedEntity = m_HierarchyPanel->GetSelectedEntity();
		}

		// Preview Camera window
		if (selectedEntity && selectedEntity.HasComponent<CameraComponent>())
		{
			TransformComponent& transformComp = selectedEntity.GetComponent<TransformComponent>();
		
			CameraComponent& cameraComp = selectedEntity.GetComponent<CameraComponent>();
			Matrix4x4 view = Matrix4x4::Translate(transformComp.GetWorldPosition()) * Matrix4x4::Rotate({ transformComp.rotation });
			Matrix4x4 projection = cameraComp.Camera.GetProjectionMatrix();
			m_CameraPreview->Bind();
			RenderCommand::Clear();
			SceneManager::CurrentScene()->Render(m_CameraPreview, view, projection);
		}

		// Debug render pass
		m_Framebuffer->Bind();
		Renderer::BeginScene(m_CameraController.GetTransformMatrix(), m_CameraController.GetCamera()->GetProjectionMatrix());

		if (selectedEntity)
		{
			TransformComponent& transformComp = selectedEntity.GetComponent<TransformComponent>();

			if (selectedEntity.HasComponent<CircleCollider2DComponent>())
			{
				CircleCollider2DComponent& circleComp = selectedEntity.GetComponent<CircleCollider2DComponent>();

				float scale = circleComp.radius * std::max(transformComp.scale.x, transformComp.scale.y);

				Matrix4x4 transform = transformComp.GetParentMatrix()
					* Matrix4x4::Translate(transformComp.position)
					* Matrix4x4::Rotate(Vector3f(0.0f, 0.0f, transformComp.rotation.z))
					* Matrix4x4::Translate(Vector3f(circleComp.offset.x, circleComp.offset.y, 0.001f))
					* Matrix4x4::Scale(Vector3f(scale, scale, 1.0f));

				Renderer2D::DrawHairLineCircle(transform, 60, Colours::LIME_GREEN, selectedEntity);
			}

			if (selectedEntity.HasComponent<BoxCollider2DComponent>())
			{
				BoxCollider2DComponent& boxComp = selectedEntity.GetComponent<BoxCollider2DComponent>();

				Matrix4x4 transform = transformComp.GetParentMatrix()
					* Matrix4x4::Translate(transformComp.position)
					* Matrix4x4::Rotate(Vector3f(0.0f, 0.0f, transformComp.rotation.z))
					* Matrix4x4::Translate(Vector3f(boxComp.offset.x, boxComp.offset.y, 0.001f))
					* Matrix4x4::Scale(Vector3f(boxComp.size.x * transformComp.scale.x * 2, boxComp.size.y * transformComp.scale.y * 2, 1.0f));

				Renderer2D::DrawHairLineRect(transform, Colours::LIME_GREEN, selectedEntity);
			}

			if (selectedEntity.HasComponent<PolygonCollider2DComponent>())
			{
				PolygonCollider2DComponent& polygonComp = selectedEntity.GetComponent<PolygonCollider2DComponent>();

				std::vector<Vector3f> vertices;
				vertices.reserve(polygonComp.vertices.size());

				for (Vector2f& vertex : polygonComp.vertices)
				{
					Matrix4x4 transform = transformComp.GetParentMatrix()
						* Matrix4x4::Translate(transformComp.position)
						* Matrix4x4::Rotate(Vector3f(0.0f, 0.0f, transformComp.rotation.z))
						* Matrix4x4::Translate(Vector3f(polygonComp.offset.x + vertex.x * transformComp.scale.x,
							polygonComp.offset.y + vertex.y * transformComp.scale.y, 0.001f));
					vertices.push_back(transform * Vector3f());
				}

				Renderer2D::DrawHairLinePolygon(vertices, Colours::LIME_GREEN, selectedEntity);
			}

			if (selectedEntity.HasComponent<CapsuleCollider2DComponent>())
			{
				CapsuleCollider2DComponent& capsuleComp = selectedEntity.GetComponent<CapsuleCollider2DComponent>();
				// TODO debug draw capsule collider
				if (capsuleComp.direction == CapsuleCollider2DComponent::Direction::Vertical)
				{
					Vector3f worldPosition = transformComp.GetWorldPosition();
					//Renderer2D::DrawHairLine()
				}
			}

			if (selectedEntity.HasComponent<TilemapComponent>())
			{
				TilemapComponent& tilemapComp = selectedEntity.GetComponent<TilemapComponent>();

				switch (tilemapComp.orientation)
				{
				case TilemapComponent::Orientation::orthogonal:

					for (size_t i = 0; i < tilemapComp.tilesHigh + 1; i++)
					{
						Vector3f start(0.0f, -(float)i, 0.001f);
						Vector3f end((float)(tilemapComp.tilesWide), -(float)i, 0.001f);

						start = transformComp.GetWorldMatrix() * start;
						end = transformComp.GetWorldMatrix() * end;

						Renderer2D::DrawHairLine(start, end, Colour(0.2f, 0.2f, 0.2f, 0.5f), selectedEntity);
					}
					for (size_t i = 0; i < tilemapComp.tilesWide + 1; i++)
					{
						Vector3f start((float)i, 0.0f, 0.001f);
						Vector3f end((float)i, -(float)(tilemapComp.tilesHigh), 0.001f);

						start = transformComp.GetWorldMatrix() * start;
						end = transformComp.GetWorldMatrix() * end;

						Renderer2D::DrawHairLine(start, end, Colour(0.2f, 0.2f, 0.2f, 0.5f), selectedEntity);
					}
					break;
				case TilemapComponent::Orientation::isometric:
					break;
				case TilemapComponent::Orientation::hexagonal:
					break;
				case TilemapComponent::Orientation::staggered:
					break;
				default:
					break;
				}

				if (m_TilemapEditor->HasSelection())
				{
					// Calculate which tilemap cell is hovererd
					if (tilemapComp.orientation == TilemapComponent::Orientation::orthogonal)
					{
						Matrix4x4 cameraViewMat = Matrix4x4::Inverse(m_CameraController.GetTransformMatrix());
						Matrix4x4 cameraProjectionMat = m_CameraController.GetCamera()->GetProjectionMatrix();
						Line3D ray = MathUtils::ComputeCameraRay(cameraViewMat, cameraProjectionMat, m_RelativeMousePosition, Vector2f(m_ViewportSize.x, m_ViewportSize.y));

						Vector3f normal(0.0f, 0.0f, 1.0f);
						Matrix4x4 transformMat = transformComp.GetWorldMatrix();

						transformMat.Transpose();

						float translation[3], rotation[3], scale[3];
						ImGuizmo::DecomposeMatrixToComponents(transformMat.m16, translation, rotation, scale);

						Vector3f rotationRad((float)DegToRad(rotation[0]), (float)DegToRad(rotation[1]), (float)DegToRad(rotation[2]));

						Quaternion quat(rotationRad);
						quat.RotateVectorByQuaternion(normal);
						Plane tilemapPlane(transformComp.GetWorldPosition(), normal);

						if (Vector3f position; Plane::PlaneLineIntersection(tilemapPlane, ray, transformComp.GetWorldPosition(), position))
						{
							m_TilemapEditor->OnRender(position, transformComp, tilemapComp);
						}
					}
				}
			}
		}

		SceneManager::CurrentScene()->GetRegistry().view<TransformComponent, CameraComponent>().each(
			[](const auto entity, auto& transformComp, auto& cameraComp)
			{
				Matrix4x4 view = Matrix4x4::Translate(transformComp.GetWorldPosition()) * Matrix4x4::Rotate({ transformComp.rotation });
				Matrix4x4 projection = Matrix4x4::Inverse(cameraComp.Camera.GetProjectionMatrix());
				Vector3f frontTopLeft = Vector3f(-1.0f, 1.0f, -1.0f) * projection*view;
				Vector3f frontTopRight = Vector3f(1.0f, 1.0f, -1.0f) * projection* view;
				Vector3f frontBottomLeft = Vector3f(-1.0f, -1.0f, -1.0f) * projection* view;
				Vector3f frontBottomRight = Vector3f(1.0f, -1.0f, -1.0f) * projection * view;

				Vector3f backTopLeft = Vector3f(-1.0f, 1.0f, 1.0f) * projection * view;
				Vector3f backTopRight = Vector3f(1.0f, 1.0f, 1.0f) * projection * view;
				Vector3f backBottomLeft = Vector3f(-1.0f, -1.0f, 1.0f) * projection * view;
				Vector3f backBottomRight = Vector3f(1.0f, -1.0f, 1.0f) * projection * view;

				Renderer2D::DrawHairLine(frontTopLeft, frontTopRight, Colours::SILVER, (int)entity);
				Renderer2D::DrawHairLine(frontTopRight, frontBottomRight, Colours::SILVER, (int)entity);
				Renderer2D::DrawHairLine(frontBottomRight, frontBottomLeft, Colours::SILVER, (int)entity);
				Renderer2D::DrawHairLine(frontBottomLeft, frontTopLeft, Colours::SILVER, (int)entity);

				Renderer2D::DrawHairLine(backTopLeft, backTopRight, Colours::SILVER, (int)entity);
				Renderer2D::DrawHairLine(backTopRight, backBottomRight, Colours::SILVER, (int)entity);
				Renderer2D::DrawHairLine(backBottomRight, backBottomLeft, Colours::SILVER, (int)entity);
				Renderer2D::DrawHairLine(backBottomLeft, backTopLeft, Colours::SILVER, (int)entity);

				Renderer2D::DrawHairLine(frontTopLeft, backTopLeft, Colours::SILVER, (int)entity);
				Renderer2D::DrawHairLine(frontTopRight, backTopRight, Colours::SILVER, (int)entity);
				Renderer2D::DrawHairLine(frontBottomRight, backBottomRight, Colours::SILVER, (int)entity);
				Renderer2D::DrawHairLine(frontBottomLeft, backBottomLeft, Colours::SILVER, (int)entity);
			});

		Renderer2D::FlushHairLines();

		if (m_ShowGrid)
		{
			if (m_Is2DMode)
			{
				Matrix4x4 gridTransform = Matrix4x4::Translate(Vector3f(m_CameraController.GetPosition().x, m_CameraController.GetPosition().y, 0.001f))
					* Matrix4x4::RotateX((float)PIDIV2);
				Renderer::Submit(m_GridMaterial, m_GridMesh, gridTransform);
			}
			else
			{
				Renderer::Submit(m_GridMaterial, m_GridMesh, Matrix4x4::Translate(Vector3f(m_CameraController.GetPosition().x, 0.0f, m_CameraController.GetPosition().z)));
			}
		}

		Renderer::EndScene();
		m_Framebuffer->UnBind();
		break;
	}
	default:
		break;
	}

	if (SceneManager::GetSceneState() == SceneState::Simulate)
	{
		m_Framebuffer->Bind();
		Renderer2D::BeginScene(m_CameraController.GetTransformMatrix(), m_CameraController.GetCamera()->GetProjectionMatrix());
		SceneManager::CurrentScene()->GetRegistry().view<LuaScriptComponent>().each([](auto entity, auto& luaScriptComp)
			{
				luaScriptComp.OnDebugRender();
			});
		Renderer2D::EndScene();
		m_Framebuffer->UnBind();
	}
	else if (SceneManager::GetSceneState() == SceneState::Play)
	{
		m_Framebuffer->Bind();
		Matrix4x4 view;
		Matrix4x4 projection;
		Entity cameraEntity = SceneManager::CurrentScene()->GetPrimaryCameraEntity();
		if (cameraEntity)
		{
			auto [cameraComp, transformComp] = cameraEntity.GetComponents<CameraComponent, TransformComponent>();
			view = Matrix4x4::Translate(transformComp.GetWorldPosition()) * Matrix4x4::Rotate(Quaternion(transformComp.rotation));
			projection = cameraComp.Camera.GetProjectionMatrix();
		}

		Renderer2D::BeginScene(view, projection);
		SceneManager::CurrentScene()->GetRegistry().view<LuaScriptComponent>().each([](auto entity, auto& luaScriptComp)
			{
				luaScriptComp.OnDebugRender();
			});
		Renderer2D::EndScene();
		m_Framebuffer->UnBind();
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

	ImGuizmo::SetOrthographic(m_Is2DMode);
	ImGuizmo::BeginFrame();

	ImGui::SetNextWindowSize(ImVec2(800, 800), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(30, 30), ImGuiCond_FirstUseEver);

	ImVec2 pos;
	ImGuiIO& io = ImGui::GetIO();
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar;

	if (SceneManager::CurrentScene()->IsDirty())
		flags |= ImGuiWindowFlags_UnsavedDocument;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	bool shown = ImGui::Begin(ICON_FA_BORDER_ALL" Viewport", m_Show, flags);
	if (shown)
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

			// Universal -------------------------------------------------------------------------------
			selected = m_Operation == OperationMode::Universal;
			if (!selected)
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

			if (ImGui::Button(ICON_MDI_CROP_ROTATE))
				m_Operation = OperationMode::Universal;

			if (!selected)
				ImGui::PopStyleColor();
			ImGui::Tooltip("Universal (T)");

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
			ImGui::EndMenuBar();
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
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
		ImGui::PopStyleVar(2);
		m_RightClickMenuOpen = false;
		if (!ImGui::IsMouseDragging(ImGuiMouseButton_Right) && ImGui::GetMouseDragDelta(ImGuiMouseButton_Right).y <= FLT_EPSILON && ImGui::GetMouseDragDelta(ImGuiMouseButton_Right).x <= FLT_EPSILON
			&& ImGui::GetMouseDragDelta(ImGuiMouseButton_Right).y >= -FLT_EPSILON && ImGui::GetMouseDragDelta(ImGuiMouseButton_Right).x >= -FLT_EPSILON)
		{
			if (ImGui::BeginPopupContextItem("Viewport Right Click"))
			{
				m_RightClickMenuOpen = true;
				if (ImGui::MenuItem(ICON_FA_CUT" Cut", "Ctrl + X", nullptr, HasSelection()))
					Cut();
				if (ImGui::MenuItem(ICON_FA_COPY" Copy", "Ctrl + C", nullptr, HasSelection()))
					Copy();
				if (ImGui::MenuItem(ICON_FA_PASTE" Paste", "Ctrl + V", nullptr, ImGui::GetClipboardText() != nullptr))
					Paste();
				if (ImGui::MenuItem(ICON_FA_CLONE" Duplicate", "Ctrl + D", nullptr, HasSelection()))
					Duplicate();
				if (ImGui::MenuItem(ICON_FA_TRASH_ALT" Delete", "Del", nullptr, HasSelection()))
					Delete();
				ImGui::EndPopup();
			}
		}

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

						StaticMeshComponent& staticMeshComp = staticMeshEntity.AddComponent<StaticMeshComponent>();

						staticMeshComp.mesh = AssetManager::GetMesh(*file);
						staticMeshComp.material = CreateRef<Material>("Standard", Colours::WHITE);
						staticMeshComp.material->AddTexture(Texture2D::Create(Application::GetWorkingDirectory() / "resources" / "UVChecker.png"), 0);
					}
					else if (file->extension() == ".tmx")
					{
						ImportManager::ImportAsset(*file, file->stem());
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
			ImGuizmo::SetID(0);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(window_pos.x, window_pos.y, (float)panelSize.x, (float)panelSize.y);

			Matrix4x4 cameraViewMat = Matrix4x4::Inverse(m_CameraController.GetTransformMatrix());
			Matrix4x4 cameraProjectionMat = m_CameraController.GetCamera()->GetProjectionMatrix();
			cameraViewMat.Transpose();

			if (m_HierarchyPanel->GetSelectedEntity().IsValid())
			{
				Entity selectedEntity = m_HierarchyPanel->GetSelectedEntity();
				TransformComponent& transformComp = selectedEntity.GetTransform();

				if (m_HierarchyPanel->IsFocused())
				{
					m_CameraController.LookAt(transformComp.GetWorldPosition(), transformComp.scale.x); // TODO: get the bounds of the object and offset the camera by that amount
					m_HierarchyPanel->HasFocused();
				}

				// Draw a camera preview if the selected entity has a camera
				if (selectedEntity.HasComponent<CameraComponent>())
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
					ImGui::Text(" %s", selectedEntity.GetName().c_str());
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

				float bounds[6];

				if (selectedEntity.HasComponent<SpriteComponent>()
					|| selectedEntity.HasComponent<AnimatedSpriteComponent>()
					|| selectedEntity.HasComponent<CircleRendererComponent>())
				{
					bounds[0] = -0.5f;
					bounds[1] = -0.5f;
					bounds[2] = 0.0f;
					bounds[3] = 0.5f;
					bounds[4] = 0.5f;
					bounds[5] = 0.0f;
				}
				else if (selectedEntity.HasComponent<StaticMeshComponent>())
				{
					//TODO: add a get bounds function method from mesh
					bounds[0] = -0.5f;
					bounds[1] = -0.5f;
					bounds[2] = -0.5f;
					bounds[3] = 0.5f;
					bounds[4] = 0.5f;
					bounds[5] = 0.5f;
				}
				else if (selectedEntity.HasComponent<TilemapComponent>())
				{
					TilemapComponent& tilemapComp = selectedEntity.GetComponent<TilemapComponent>();
					bounds[0] = 0.0f;
					bounds[1] = -(float)tilemapComp.tilesHigh;
					bounds[2] = 0.0f;
					bounds[3] = (float)tilemapComp.tilesWide;
					bounds[4] = 0.0f;
					bounds[5] = 0.0f;
				}
				else if (selectedEntity.HasComponent<BoxCollider2DComponent>())
				{
					BoxCollider2DComponent& boxColliderComp = selectedEntity.GetComponent<BoxCollider2DComponent>();
					TransformComponent& transformComp = selectedEntity.GetComponent<TransformComponent>();

					bounds[0] = -boxColliderComp.size.x + (boxColliderComp.offset.x / transformComp.scale.x);
					bounds[1] = -boxColliderComp.size.y + (boxColliderComp.offset.y / transformComp.scale.y);
					bounds[2] = 0.0f;
					bounds[3] = boxColliderComp.size.x + (boxColliderComp.offset.x / transformComp.scale.x);
					bounds[4] = boxColliderComp.size.y + (boxColliderComp.offset.y / transformComp.scale.y);
					bounds[5] = 0.0f;
				}
				else
				{
					memset(bounds, 0, sizeof(bounds));
				}

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
				case ViewportPanel::OperationMode::Universal:
					gizmoOperation = ImGuizmo::OPERATION::UNIVERSAL;
					break;
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

					Vector3f rotationRad((float)DegToRad(rotation[0]), (float)DegToRad(rotation[1]), (float)DegToRad(rotation[2]));

					CORE_ASSERT(!std::isnan(translation[0]), "Translation is not a number!");

					RigidBody2DComponent* rigidBody2DComp = selectedEntity.TryGetComponent<RigidBody2DComponent>();

					if (SceneManager::GetSceneState() != SceneState::Edit && rigidBody2DComp)
					{
						rigidBody2DComp->SetTransform(Vector2f(translation[0], translation[1]), rotationRad.z);
						transformComp.position.z = translation[2];
						transformComp.rotation.x = rotationRad.x;
						transformComp.rotation.y = rotationRad.y;

						if (SceneManager::GetSceneState() == SceneState::SimulatePause)
						{
							Vector2f position;
							float angle;

							rigidBody2DComp->GetTransform(position, angle);
							transformComp.position.x = position.x;
							transformComp.position.y = position.y;
							transformComp.rotation.z = angle;
						}
					}
					else
					{
						Vector3f deltaRotation = rotationRad - transformComp.rotation;
						if (gizmoOperation == ImGuizmo::OPERATION::TRANSLATE || gizmoOperation == ImGuizmo::OPERATION::UNIVERSAL || m_Operation == OperationMode::Select)
						{
							transformComp.position = Vector3f(translation[0], translation[1], translation[2]);
						}
						if (gizmoOperation == ImGuizmo::OPERATION::ROTATE || gizmoOperation == ImGuizmo::OPERATION::UNIVERSAL)
						{
							transformComp.rotation += deltaRotation;
						}
						if (gizmoOperation == ImGuizmo::OPERATION::SCALE || gizmoOperation == ImGuizmo::OPERATION::UNIVERSAL || m_Operation == OperationMode::Select)
						{
							transformComp.scale = Vector3f(scale[0], scale[1], scale[2]);
						}
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
	if (!shown)
		ImGui::PopStyleVar();
	ImGui::End();
	//ImGui::PopStyleVar();
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
		else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed('T'))
			m_Operation = OperationMode::Universal;
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