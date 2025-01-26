#include "ViewportPanel.h"

#include "MainDockSpace.h"

#include "IconsFontAwesome6.h"
#include "Fonts/IconsMaterialDesignIcons.h"
#include "IconsMaterialDesign.h"
#include "Fonts/Fonts.h"

#include "Engine.h"
#include "FileSystem/FileDialog.h"
#include "Viewers/ViewerManager.h"
#include "Importers/ImportManager.h"

#include "HierarchyPanel.h"
#include "Scene/Components.h"

#include "ImGui/ImGuizmo.h"
#include "Utilities/MathUtils.h"

#include "Events/SceneEvent.h"

ViewportPanel::ViewportPanel(bool* show, Ref<HierarchyPanel> hierarchyPanel, Ref<TilemapEditor> tilemapEditor)
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

	Ref<Material> gridMaterial = CreateRef<Material>("Grid", Colours::GREY);
	gridMaterial->SetTwoSided(true);
	gridMaterial->SetTilingFactor(100.0f);
	gridMaterial->SetTransparency(true);

	m_GridMesh = GeometryGenerator::CreateGrid(1000.0f, 1000.0f, 2, 2, 1.0f, 1.0f);
	m_GridMesh->SetMaterials({ gridMaterial });
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

	if (!*m_Show)
		return;

	if (!SceneManager::IsSceneLoaded())
		return;

	bool rightMouseDown = Input::IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
	bool leftMouseDown = Input::IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);

	m_CameraController.OnUpdate(deltaTime, m_ViewportHovered);

	if (m_ViewportHovered && rightMouseDown && !m_CursorDisabled)
	{
		Application::GetWindow()->DisableCursor(); //TODO: Fix the issue that disabling the cursor jumps the mouse position
		m_CursorDisabled = true;
	}

	else if (m_CursorDisabled && !rightMouseDown)
	{
		m_CursorDisabled = false;
		Application::GetWindow()->EnableCursor();
	}

	FrameBufferSpecification spec = m_Framebuffer->GetSpecification();
	if (((uint32_t)m_ViewportSize.x != spec.width || (uint32_t)m_ViewportSize.y != spec.height) && (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f))
	{
		m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_CameraPreview->Resize((uint32_t)(m_ViewportSize.x / 8.0f), (uint32_t)(m_ViewportSize.y / 8.0f));
		SceneManager::CurrentScene()->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
	}

	m_Framebuffer->Bind();
	RenderCommand::Clear();
	m_Framebuffer->ClearAttachment(1, -1);
	m_HoveredEntity = Entity();

	SceneState sceneState = SceneManager::GetSceneState();

	if (sceneState == SceneState::Play || sceneState == SceneState::Pause)
	{
		SceneManager::CurrentScene()->Render(m_Framebuffer);
	}
	else if (sceneState == SceneState::SimulatePause || sceneState == SceneState::Edit || sceneState == SceneState::Simulate)
	{
		SceneManager::CurrentScene()->Render(m_Framebuffer, m_CameraController.GetTransformMatrix(), m_CameraController.GetCamera()->GetProjectionMatrix());

		if (m_ViewportHovered && !m_TilemapEditor->IsHovered())
		{
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseReleased(ImGuiMouseButton_Right))
			{
				m_Framebuffer->Bind();
				m_PixelData = m_Framebuffer->ReadPixel(1, (int)m_RelativeMousePosition.x, (int)(m_ViewportSize.y - m_RelativeMousePosition.y));
				m_HoveredEntity = m_PixelData == -1 ? Entity() : Entity((entt::entity)m_PixelData, SceneManager::CurrentScene());
				if (!ImGuizmo::IsUsing() && !ImGuizmo::IsOver() && !m_RightClickMenuOpen
					&& m_RelativeMousePosition == m_MousePositionBeginClick)
					m_HierarchyPanel->SetSelectedEntity(m_HoveredEntity);
				m_Framebuffer->UnBind();
			}
		}

		Entity selectedEntity;

		if (m_HierarchyPanel->GetSelectedEntity().IsSceneValid())
		{
			selectedEntity = m_HierarchyPanel->GetSelectedEntity();
		}

		// Preview Camera window
		if (selectedEntity && selectedEntity.HasComponent<CameraComponent>())
		{
			TransformComponent& transformComp = selectedEntity.GetComponent<TransformComponent>();

			CameraComponent& cameraComp = selectedEntity.GetComponent<CameraComponent>();
			Matrix4x4 view = Matrix4x4::Translate(transformComp.GetWorldPosition()) * Matrix4x4::Rotate({ transformComp.rotation });
			Matrix4x4 projection = cameraComp.camera.GetProjectionMatrix();
			m_CameraPreview->Bind();
			RenderCommand::Clear();
			SceneManager::CurrentScene()->Render(m_CameraPreview, view, projection);
		}

		// Debug render pass
		m_Framebuffer->Bind();
		Renderer::BeginScene(m_CameraController.GetTransformMatrix(), m_CameraController.GetCamera()->GetProjectionMatrix());

		if (selectedEntity && selectedEntity.HasComponent<TransformComponent>())
		{
			TransformComponent& transformComp = selectedEntity.GetComponent<TransformComponent>();

			if (selectedEntity.HasComponent<CircleCollider2DComponent>())
			{
				CircleCollider2DComponent& circleComp = selectedEntity.GetComponent<CircleCollider2DComponent>();

				float scale = circleComp.radius * std::max(transformComp.scale.x, transformComp.scale.y);

				Matrix4x4 transform = transformComp.GetParentMatrix()
					* Matrix4x4::Translate(transformComp.position)
					* Matrix4x4::RotateZ(transformComp.rotation.z)
					* Matrix4x4::Translate(Vector3f(circleComp.offset.x, circleComp.offset.y, 0.001f))
					* Matrix4x4::Scale(Vector3f(scale, scale, 1.0f));

				Renderer2D::DrawHairLineCircle(transform, 60, Colours::LIME_GREEN, selectedEntity);
			}

			if (selectedEntity.HasComponent<BoxCollider2DComponent>())
			{
				BoxCollider2DComponent& boxComp = selectedEntity.GetComponent<BoxCollider2DComponent>();

				Matrix4x4 transform = transformComp.GetParentMatrix()
					* Matrix4x4::Translate(transformComp.position)
					* Matrix4x4::RotateZ(transformComp.rotation.z)
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
						* Matrix4x4::RotateZ(transformComp.rotation.z)
						* Matrix4x4::Translate(Vector3f(polygonComp.offset.x + vertex.x * transformComp.scale.x,
							polygonComp.offset.y + vertex.y * transformComp.scale.y, 0.001f));
					vertices.push_back(transform * Vector3f());
				}

				Renderer2D::DrawHairLinePolygon(vertices, Colours::LIME_GREEN, selectedEntity);
			}

			if (selectedEntity.HasComponent<CapsuleCollider2DComponent>())
			{
				CapsuleCollider2DComponent& capsuleComp = selectedEntity.GetComponent<CapsuleCollider2DComponent>();
				Matrix4x4 transform = transformComp.GetParentMatrix()
					* Matrix4x4::Translate(transformComp.position)
					* Matrix4x4::RotateZ(transformComp.rotation.z)
					* Matrix4x4::Translate(Vector3f(capsuleComp.offset.x, capsuleComp.offset.y, 0.001f));
				float scaledradius;
				float halfHeight;
				if (capsuleComp.direction == CapsuleCollider2DComponent::Direction::Horizontal)
				{
					scaledradius = abs(capsuleComp.radius * transformComp.scale.y);
					halfHeight = abs((capsuleComp.height * transformComp.scale.x) / 2.0f);
					transform = transform * Matrix4x4::RotateZ((float)PIDIV2);
				}
				else
				{
					scaledradius = abs(capsuleComp.radius * transformComp.scale.x);
					halfHeight = abs((capsuleComp.height * transformComp.scale.y) / 2.0f);
				}

				if (halfHeight > scaledradius)
				{
					Matrix4x4 topArcTransform = transform
						* Matrix4x4::Translate(Vector3f(0.0f, halfHeight - scaledradius, 0.0f))
						* Matrix4x4::Scale(Vector3f(scaledradius, scaledradius, 1.0f));
					Renderer2D::DrawHairLineArc(topArcTransform, (float)DegToRad(270), (float)DegToRad(90), 30U, Colours::LIME_GREEN, selectedEntity);

					Matrix4x4 bottomArcTransform = transform
						* Matrix4x4::Translate(Vector3f(0.0f, -(halfHeight - scaledradius), 0.0f))
						* Matrix4x4::Scale(Vector3f(scaledradius, scaledradius, 1.0f));
					Renderer2D::DrawHairLineArc(bottomArcTransform, (float)DegToRad(90), (float)DegToRad(270), 30U, Colours::LIME_GREEN, selectedEntity);

					Vector3f leftStart = transform * Vector3f(-scaledradius, -(halfHeight - scaledradius), 0.0f);
					Vector3f leftEnd = transform * Vector3f(-scaledradius, halfHeight - scaledradius, 0.0f);
					Renderer2D::DrawHairLine(leftStart, leftEnd, Colours::LIME_GREEN, selectedEntity);

					Vector3f rightStart = transform * Vector3f(scaledradius, -(halfHeight - scaledradius), 0.0f);
					Vector3f rightEnd = transform * Vector3f(scaledradius, halfHeight - scaledradius, 0.0f);
					Renderer2D::DrawHairLine(rightStart, rightEnd, Colours::LIME_GREEN, selectedEntity);
				}
				else
				{
					Matrix4x4 circleTransform = transform * Matrix4x4::Scale(Vector3f(scaledradius, scaledradius, 1.0f));
					Renderer2D::DrawHairLineCircle(circleTransform, 60U, Colours::LIME_GREEN, selectedEntity);
				}
			}

			if (selectedEntity.HasComponent<TilemapComponent>())
			{
				TilemapComponent& tilemapComp = selectedEntity.GetComponent<TilemapComponent>();

				Colour gridLineColour(0.2f, 0.2f, 0.2f, 0.5f);
				switch (tilemapComp.orientation)
				{
				case TilemapComponent::Orientation::orthogonal:

					for (size_t i = 0; i < tilemapComp.tilesHigh + 1; i++)
					{
						Vector3f start(0.0f, -(float)i, 0.001f);
						Vector3f end((float)(tilemapComp.tilesWide), -(float)i, 0.001f);

						start = transformComp.GetWorldMatrix() * start;
						end = transformComp.GetWorldMatrix() * end;

						Renderer2D::DrawHairLine(start, end, gridLineColour, selectedEntity);
					}
					for (size_t i = 0; i < tilemapComp.tilesWide + 1; i++)
					{
						Vector3f start((float)i, 0.0f, 0.001f);
						Vector3f end((float)i, -(float)(tilemapComp.tilesHigh), 0.001f);

						start = transformComp.GetWorldMatrix() * start;
						end = transformComp.GetWorldMatrix() * end;

						Renderer2D::DrawHairLine(start, end, gridLineColour, selectedEntity);
					}
					break;
				case TilemapComponent::Orientation::isometric:
					for (uint32_t i = 0; i < tilemapComp.tilesHigh + 1; i++)
					{
						Vector2f startIso = tilemapComp.IsoToWorld(0, i);
						Vector2f endIso = tilemapComp.IsoToWorld(tilemapComp.tilesWide, i);
						Vector3f start(startIso.x, startIso.y, 0.001f);
						Vector3f end(endIso.x, endIso.y, 0.001f);

						start = transformComp.GetWorldMatrix() * start;
						end = transformComp.GetWorldMatrix() * end;

						Renderer2D::DrawHairLine(start, end, gridLineColour, selectedEntity);
					}
					for (uint32_t i = 0; i < tilemapComp.tilesWide + 1; i++)
					{
						Vector2f startIso = tilemapComp.IsoToWorld(i, 0);
						Vector2f endIso = tilemapComp.IsoToWorld(i, tilemapComp.tilesHigh);
						Vector3f start(startIso.x, startIso.y, 0.001f);
						Vector3f end(endIso.x, endIso.y, 0.001f);

						start = transformComp.GetWorldMatrix() * start;
						end = transformComp.GetWorldMatrix() * end;

						Renderer2D::DrawHairLine(start, end, gridLineColour, selectedEntity);
					}
					break;
				case TilemapComponent::Orientation::hexagonal:
				{
					float pixelsPerUnit = (float)SceneManager::CurrentScene()->GetPixelsPerUnit();
					float halfWidth = (float)tilemapComp.tileWidth / pixelsPerUnit / 2.0f;
					float halfHeight = (float)tilemapComp.tileHeight / pixelsPerUnit / 2.0f;
					float quarterWidth = halfWidth * 0.5f;

					std::vector<Vector2f> cornerOffsets = {
						{-halfWidth, 0.0f},                // Left
						{-quarterWidth, -halfHeight},      // Bottom-left
						{quarterWidth, -halfHeight},       // Bottom-right
						{halfWidth, 0.0f},                 // Right
						{quarterWidth, halfHeight},        // Top-right
						{-quarterWidth, halfHeight}        // Top-left
					};

					/*      5 ----- 4
						   /         \
						  /           \
						0 ------------ 3
						  \           /
						   \         /
						    1 ----- 2
					*/

					for (uint32_t r = 0; r < tilemapComp.tilesHigh; ++r)
					{
						for (uint32_t q = 0; q < tilemapComp.tilesWide; ++q)
						{
							Vector2f centerHex = tilemapComp.HexToWorld(q, r);

							std::vector<Vector3f> transformedCorners(cornerOffsets.size());

							for (size_t i = 0; i < cornerOffsets.size(); ++i)
							{
								Vector2f corner = centerHex + cornerOffsets[i];
								transformedCorners[i] = transformComp.GetWorldMatrix() * Vector3f(corner.x, corner.y, 0.001f);
							}

							for (size_t i : {0, 1, 2})
							{
								Renderer2D::DrawHairLine(transformedCorners[i], transformedCorners[i + 1], gridLineColour, selectedEntity);
							}

							if (q == tilemapComp.tilesWide - 1 || (q % 2 == 0 && r == 0))
							{
								Renderer2D::DrawHairLine(transformedCorners[3], transformedCorners[4], gridLineColour, selectedEntity);
							}

							if (r == 0)
							{
								Renderer2D::DrawHairLine(transformedCorners[4], transformedCorners[5], gridLineColour, selectedEntity);
							}

							if (q == 0 || (q % 2 == 0 && r == 0))
							{
								Renderer2D::DrawHairLine(transformedCorners[0], transformedCorners[5], gridLineColour, selectedEntity);
							}

						}
					}
					break;
				}
				case TilemapComponent::Orientation::staggered:
					//TODO: draw lines on staggered grid
					break;
				default:
					break;
				}

				m_TilemapEditor->SetTilemapEntity(selectedEntity);

				if (m_TilemapEditor->IsShown() && m_WindowHovered)
				{
					// Calculate which tilemap cell is hovered
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
						m_TilemapEditor->OnRender(position);
					}
				}
			}
			else
			{
				m_TilemapEditor->Hide();
			}
		}
		else
		{
			m_TilemapEditor->Hide();
		}

		SceneManager::CurrentScene()->GetRegistry().view<TransformComponent, CameraComponent>().each(
			[](const auto entity, auto& transformComp, auto& cameraComp)
			{
				Matrix4x4 view = Matrix4x4::Translate(transformComp.GetWorldPosition()) * Matrix4x4::Rotate({ transformComp.rotation });
				Matrix4x4 projection = Matrix4x4::Inverse(cameraComp.camera.GetProjectionMatrix());
				projection.Transpose();

				Vector4f frontTopLeft_4 = Vector4f(-1.0f, 1.0f, -1.0f, 1.0f) * projection * view;
				Vector4f frontTopRight_4 = Vector4f(1.0f, 1.0f, -1.0f, 1.0f) * projection * view;
				Vector4f frontBottomLeft_4 = Vector4f(-1.0f, -1.0f, -1.0f, 1.0f) * projection * view;
				Vector4f frontBottomRight_4 = Vector4f(1.0f, -1.0f, -1.0f, 1.0f) * projection * view;

				Vector4f backTopLeft_4 = Vector4f(-1.0f, 1.0f, 1.0f, 1.0f) * projection * view;
				Vector4f backTopRight_4 = Vector4f(1.0f, 1.0f, 1.0f, 1.0f) * projection * view;
				Vector4f backBottomLeft_4 = Vector4f(-1.0f, -1.0f, 1.0f, 1.0f) * projection * view;
				Vector4f backBottomRight_4 = Vector4f(1.0f, -1.0f, 1.0f, 1.0f) * projection * view;

				Vector3f frontTopLeft = frontTopLeft_4.xyz() / frontTopLeft_4.w;
				Vector3f frontTopRight = frontTopRight_4.xyz() / frontTopRight_4.w;
				Vector3f frontBottomLeft = frontBottomLeft_4.xyz() / frontBottomLeft_4.w;
				Vector3f frontBottomRight = frontBottomRight_4.xyz() / frontBottomRight_4.w;

				Vector3f backTopLeft = backTopLeft_4.xyz() / backTopLeft_4.w;
				Vector3f backTopRight = backTopRight_4.xyz() / backTopRight_4.w;
				Vector3f backBottomLeft = backBottomLeft_4.xyz() / backBottomLeft_4.w;
				Vector3f backBottomRight = backBottomRight_4.xyz() / backBottomRight_4.w;

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

		SceneManager::CurrentScene()->GetRegistry().view<TransformComponent, PointLightComponent>().each(
			[](const auto entity, auto& transformComp, auto& pointLightComp)
			{
				Matrix4x4 scale = Matrix4x4::Scale(Vector3f(pointLightComp.range, pointLightComp.range, pointLightComp.range));
				Renderer2D::DrawHairLineCircle(Matrix4x4::Translate(transformComp.GetWorldPosition())
					* scale, 60, pointLightComp.colour, (int)entity);

				Renderer2D::DrawHairLineCircle(Matrix4x4::Translate(transformComp.GetWorldPosition())
					* Matrix4x4::RotateX((float)PIDIV2) * scale, 60, pointLightComp.colour, (int)entity);

				Renderer2D::DrawHairLineCircle(Matrix4x4::Translate(transformComp.GetWorldPosition())
					* Matrix4x4::RotateY((float)PIDIV2) * scale, 60, pointLightComp.colour, (int)entity);
			});

		Renderer2D::FlushHairLines();

		if (m_ShowGrid)
		{
			if (m_Is2DMode)
			{
				Matrix4x4 gridTransform = Matrix4x4::Translate(Vector3f(m_CameraController.GetPosition().x, m_CameraController.GetPosition().y, 0.001f))
					* Matrix4x4::RotateX((float)PIDIV2);
				Renderer::Submit(m_GridMesh, gridTransform);
			}
			else
			{
				Renderer::Submit(m_GridMesh, Matrix4x4::Translate(Vector3f(m_CameraController.GetPosition().x, 0.0f, m_CameraController.GetPosition().z)));
			}
		}

		Renderer::EndScene();
		m_Framebuffer->UnBind();
	}

	if (sceneState == SceneState::Simulate)
	{
		m_TilemapEditor->Hide();
		m_Framebuffer->Bind();
		Renderer2D::BeginScene();
		SceneManager::CurrentScene()->GetRegistry().view<LuaScriptComponent>().each([](auto entity, auto& luaScriptComp)
			{
				luaScriptComp.OnDebugRender();
			});
		Renderer2D::EndScene();
		m_Framebuffer->UnBind();
	}
	else if (sceneState == SceneState::Play)
	{
		m_TilemapEditor->Hide();
		m_Framebuffer->Bind();
		Matrix4x4 view;
		Matrix4x4 projection;
		if (Entity cameraEntity = SceneManager::CurrentScene()->GetPrimaryCameraEntity())
		{
			auto [cameraComp, transformComp] = cameraEntity.GetComponents<CameraComponent, TransformComponent>();
			view = Matrix4x4::Translate(transformComp.GetWorldPosition()) * Matrix4x4::Rotate(Quaternion(transformComp.rotation));
			projection = cameraComp.camera.GetProjectionMatrix();
		}

		Renderer2D::BeginScene();
		SceneManager::CurrentScene()->GetRegistry().view<LuaScriptComponent>().each([](auto entity, auto& luaScriptComp)
			{
				luaScriptComp.OnDebugRender();
			});
		Renderer2D::EndScene();
		m_Framebuffer->UnBind();
	}
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

	ImGuiIO& io = ImGui::GetIO();
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar;

	if (SceneManager::CurrentScene()->IsDirty() && SceneManager::GetSceneState() == SceneState::Edit)
		flags |= ImGuiWindowFlags_UnsavedDocument;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	bool shown;

	if (m_Fullscreen) {
		bool use_work_area = true;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(use_work_area ? viewport->WorkPos : viewport->Pos);
		ImGui::SetNextWindowSize(use_work_area ? viewport->WorkSize : viewport->Size);
		flags |= ImGuiWindowFlags_NoDecoration;
		shown = ImGui::Begin("Viewport fullscreen", m_Show, flags);
	}
	else {
		ImGui::SetNextWindowSize(ImVec2(800, 800), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(30, 30), ImGuiCond_FirstUseEver);
		shown = ImGui::Begin(ICON_FA_BORDER_ALL" Viewport", m_Show, flags);
	}

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

			if (ImGui::Button(ICON_FA_ARROW_POINTER))
				m_Operation = OperationMode::Select;

			if (!selected)
				ImGui::PopStyleColor();
			ImGui::Tooltip("Select (Q)");

			// Move --------------------------------------------------------------------------------
			selected = m_Operation == OperationMode::Move;
			if (!selected)
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

			if (ImGui::Button(ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT))
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
				if (ImGui::Button(ICON_FA_EARTH_EUROPE))
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
				if (ImGui::MenuItem("Collision", "", &m_ShowCollision))
					SceneManager::CurrentScene()->SetShowDebug(m_ShowCollision);
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

		ImVec2 panelSize = ImGui::GetContentRegionAvail();
		if (m_ViewportSize.x != panelSize.x || m_ViewportSize.y != panelSize.y)
		{
			m_ViewportSize = panelSize;
			m_CameraController.SetViewPortSize(Vector2f(m_ViewportSize.x, m_ViewportSize.y));
			m_CameraController.SetAspectRatio(m_ViewportSize.x / m_ViewportSize.y);
			SceneManager::CurrentScene()->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		ImVec2 window_pos = ImGui::GetCursorScreenPos();

		uintptr_t tex = (uintptr_t)m_Framebuffer->GetColourAttachment();

		ImGui::Image((void*)tex, m_ViewportSize, ImVec2(0, 1), ImVec2(1, 0));
		ImGui::PopStyleVar(2);
		m_RightClickMenuOpen = false;
		if (!ImGui::IsMouseDragging(ImGuiMouseButton_Right))
		{
			if (ImGui::BeginPopupContextItem("Viewport Right Click"))
			{
				m_RightClickMenuOpen = true;
				if (ImGui::MenuItem(ICON_FA_SCISSORS" Cut", "Ctrl + X", nullptr, HasSelection()))
					Cut();
				if (ImGui::MenuItem(ICON_FA_COPY" Copy", "Ctrl + C", nullptr, HasSelection()))
					Copy();
				if (ImGui::MenuItem(ICON_FA_PASTE" Paste", "Ctrl + V", nullptr, ImGui::GetClipboardText() != nullptr))
					Paste();
				if (ImGui::MenuItem(ICON_FA_CLONE" Duplicate", "Ctrl + D", nullptr, HasSelection()))
					Duplicate();
				if (ImGui::MenuItem(ICON_FA_TRASH_CAN" Delete", "Del", nullptr, HasSelection()))
					Delete();
				ImGui::EndPopup();
			}
		}

		ImVec2 mouse_pos = ImGui::GetMousePos();

		m_RelativeMousePosition = { mouse_pos.x - window_pos.x, mouse_pos.y - window_pos.y };
		if ((ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Right)))
		{
			m_MousePositionBeginClick = m_RelativeMousePosition;
		}

		m_ViewportHovered = (m_RelativeMousePosition.x < m_ViewportSize.x && m_RelativeMousePosition.y < m_ViewportSize.y
			&& m_RelativeMousePosition.x > 0.0f && m_RelativeMousePosition.y > 0.0f)
			|| (Input::IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)
				&& m_MousePositionBeginClick.x < m_ViewportSize.x && m_MousePositionBeginClick.y < m_ViewportSize.y
				&& m_MousePositionBeginClick.x > 0.0f && m_MousePositionBeginClick.y > 0.0f);

		if (SceneManager::GetSceneState() == SceneState::Edit)
		{
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Asset", ImGuiDragDropFlags_AcceptPeekOnly))
				{
					std::filesystem::path* file = (std::filesystem::path*)payload->Data;

					if (ViewerManager::GetFileType(*file) == FileType::MESH)
					{
						if (ImGui::AcceptDragDropPayload("Asset", ImGuiDragDropFlags_None))
						{
							std::string entityName = file->filename().string();
							entityName = entityName.substr(0, entityName.find_last_of('.'));
							Entity staticMeshEntity = SceneManager::CurrentScene()->CreateEntity(entityName);
							staticMeshEntity.AddComponent<TransformComponent>();

							StaticMeshComponent& staticMeshComp = staticMeshEntity.AddComponent<StaticMeshComponent>();

							staticMeshComp.mesh = AssetManager::GetAsset<StaticMesh>(*file);
							staticMeshComp.materialOverrides.resize(staticMeshComp.mesh->GetMesh()->GetSubmeshes().size());
						}
					}
					else if (file->extension() == ".tmx")
					{
						if (ImGui::AcceptDragDropPayload("Asset", ImGuiDragDropFlags_None))
						{
							ImportManager::ImportAsset(*file, file->stem());
						}
					}
					else if (file->extension() == ".scene")
					{
						if (ImGui::AcceptDragDropPayload("Asset", ImGuiDragDropFlags_None))
						{
							Ref<Scene> scene = CreateRef<Scene>(*file);
							scene->Load();
							SceneManager::CurrentScene()->InstantiateScene(scene, Vector3f());
						}
					}

					CLIENT_DEBUG("Dragging over viewport: {0}", file->string());
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

			if (m_HierarchyPanel->GetSelectedEntity().IsSceneValid())
			{
				Entity selectedEntity = m_HierarchyPanel->GetSelectedEntity();

				TransformComponent* transformComp = selectedEntity.TryGetComponent<TransformComponent>();
				if (transformComp)
				{
					if (m_HierarchyPanel->IsFocused())
					{
						m_CameraController.LookAt(transformComp->GetWorldPosition(), transformComp->scale.x); // TODO: get the bounds of the object and offset the camera by that amount
						m_HierarchyPanel->HasFocused();
					}


					// Draw a camera preview if the selected entity has a camera
					if (selectedEntity.HasComponent<CameraComponent>())
					{
						ImVec2 cameraPreviewPosition = ImVec2(window_pos.x - ImGui::GetStyle().ItemSpacing.x - 1 + m_ViewportSize.x - m_CameraPreview->GetSpecification().width,
							window_pos.y - ImGui::GetStyle().ItemSpacing.y + m_ViewportSize.y - m_CameraPreview->GetSpecification().height - 24.0f);

						ImU32 color = ((ImU32)(ImGui::GetStyle().Colors[ImGuiCol_TitleBg].x * 255.0f)) |
							((ImU32)(ImGui::GetStyle().Colors[ImGuiCol_TitleBg].y * 255.0f) << 8) |
							((ImU32)(ImGui::GetStyle().Colors[ImGuiCol_TitleBg].z * 255.0f) << 16) |
							255 << 24;

						ImGui::GetWindowDrawList()->AddRectFilled(cameraPreviewPosition,
							ImVec2(cameraPreviewPosition.x + m_CameraPreview->GetSpecification().width + 2,
								cameraPreviewPosition.y + m_CameraPreview->GetSpecification().height + 24.0f),
							color, ImGui::GetStyle().WindowRounding);

						uintptr_t cameraTex = (uintptr_t)m_CameraPreview->GetColourAttachment();
						float cameraCursorPosition = topLeft.x - ImGui::GetStyle().ItemSpacing.x + m_ViewportSize.x - m_CameraPreview->GetSpecification().width;
						ImGui::SetCursorPos(ImVec2(cameraCursorPosition, topLeft.y - ImGui::GetStyle().ItemSpacing.y + m_ViewportSize.y - m_CameraPreview->GetSpecification().height - 21.0f));
						ImGui::Text(" %s", selectedEntity.GetName().c_str());
						ImGui::SetCursorPos(ImVec2(cameraCursorPosition, ImGui::GetCursorPosY()));
						ImGui::Image((void*)cameraTex, ImVec2((float)m_CameraPreview->GetSpecification().width, (float)m_CameraPreview->GetSpecification().height), ImVec2(0, 1), ImVec2(1, 0));
					}

					// Gizmos

					Matrix4x4 transformMat = transformComp->GetWorldMatrix();

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
					else if (selectedEntity.HasComponent<StaticMeshComponent, PrimitiveComponent, TilemapComponent>())
					{
						const BoundingBox* box = nullptr;
						if (auto comp = selectedEntity.TryGetComponent<StaticMeshComponent>())
						{
							if (comp->mesh)
								box = &comp->mesh->GetBounds();
						}
						else if (auto comp = selectedEntity.TryGetComponent<PrimitiveComponent>())
						{
							if (comp->mesh)
								box = &comp->mesh->GetBounds();
						}
						else if (auto comp = selectedEntity.TryGetComponent<TilemapComponent>())
						{
							if (comp->mesh)
								box = &comp->mesh->GetBounds();
						}

						if (box)
						{
							bounds[0] = box->Min().x;
							bounds[1] = box->Min().y;
							bounds[2] = box->Min().z;
							bounds[3] = box->Max().x;
							bounds[4] = box->Max().y;
							bounds[5] = box->Max().z;
						}
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
						if (!m_EditTransformCommand)
							m_EditTransformCommand = CreateRef<EditComponentCommand<TransformComponent>>(selectedEntity);
						transformMat.Transpose();
						Matrix4x4 parentMatrix = Matrix4x4::Inverse(transformComp->GetParentMatrix());
						transformMat = parentMatrix * transformMat;
						transformMat.Transpose();

						ImGuizmo::DecomposeMatrixToComponents(transformMat.m16, translation, rotation, scale);

						Vector3f rotationRad((float)DegToRad(rotation[0]), (float)DegToRad(rotation[1]), (float)DegToRad(rotation[2]));

						CORE_ASSERT(!std::isnan(translation[0]), "Translation is not a number!");

						RigidBody2DComponent* rigidBody2DComp = selectedEntity.TryGetComponent<RigidBody2DComponent>();

						if (SceneManager::GetSceneState() != SceneState::Edit && rigidBody2DComp)
						{
							rigidBody2DComp->SetTransform(Vector2f(translation[0], translation[1]), rotationRad.z);
							transformComp->position.z = translation[2];
							transformComp->rotation.x = rotationRad.x;
							transformComp->rotation.y = rotationRad.y;

							if (SceneManager::GetSceneState() == SceneState::SimulatePause)
							{
								Vector2f position;
								float angle;

								rigidBody2DComp->GetTransform(position, angle);
								transformComp->position.x = position.x;
								transformComp->position.y = position.y;
								transformComp->rotation.z = angle;
							}
						}
						else
						{
							Vector3f deltaRotation = rotationRad - transformComp->rotation;
							if (gizmoOperation == ImGuizmo::OPERATION::TRANSLATE || gizmoOperation == ImGuizmo::OPERATION::UNIVERSAL || m_Operation == OperationMode::Select)
							{
								Vector3f translationVec = Vector3f(translation[0], translation[1], translation[2]);
								if ((translationVec - transformComp->position).SqrMagnitude() >= 0.000001f)
									SceneManager::CurrentScene()->MakeDirty();
								transformComp->position = translationVec;
							}
							if (gizmoOperation == ImGuizmo::OPERATION::ROTATE || gizmoOperation == ImGuizmo::OPERATION::UNIVERSAL)
							{
								if (deltaRotation.SqrMagnitude() >= 0.000001f)
									SceneManager::CurrentScene()->MakeDirty();
								transformComp->rotation += deltaRotation;
							}
							if (gizmoOperation == ImGuizmo::OPERATION::SCALE || gizmoOperation == ImGuizmo::OPERATION::UNIVERSAL || m_Operation == OperationMode::Select)
							{
								Vector3f scaleVec = Vector3f(scale[0], scale[1], scale[2]);
								if ((scaleVec - transformComp->scale).SqrMagnitude() >= 0.000001f)
									SceneManager::CurrentScene()->MakeDirty();
								transformComp->scale = scaleVec;
							}
						}
					}
					else if (m_EditTransformCommand)
					{
						HistoryManager::AddHistoryRecord(m_EditTransformCommand);
						m_EditTransformCommand = nullptr;
					}
				}

				if (WidgetComponent* widgetComp = selectedEntity.TryGetComponent<WidgetComponent>())
				{

					ImVec2 topLeft(m_ViewportSize.x * widgetComp->anchorLeft + window_pos.x, m_ViewportSize.y * widgetComp->anchorTop + window_pos.y);
					ImVec2 bottomLeft(m_ViewportSize.x * widgetComp->anchorLeft + window_pos.x, m_ViewportSize.y * widgetComp->anchorBottom + window_pos.y);
					ImVec2 bottomRight(m_ViewportSize.x * widgetComp->anchorRight + window_pos.x, m_ViewportSize.y * widgetComp->anchorBottom + window_pos.y);
					ImVec2 topRight(m_ViewportSize.x * widgetComp->anchorRight + window_pos.x, m_ViewportSize.y * widgetComp->anchorTop + window_pos.y);


					ImDrawList* drawList = ImGui::GetWindowDrawList();

					drawList->AddCircleFilled(topLeft, 4, IM_COL32(44, 44, 44, 255));
					drawList->AddCircleFilled(topLeft, 3, IM_COL32(255, 255, 255, 255));

					drawList->AddCircleFilled(bottomLeft, 4, IM_COL32(44, 44, 44, 255));
					drawList->AddCircleFilled(bottomLeft, 3, IM_COL32(255, 255, 255, 255));

					drawList->AddCircleFilled(bottomRight, 4, IM_COL32(44, 44, 44, 255));
					drawList->AddCircleFilled(bottomRight, 3, IM_COL32(255, 255, 255, 255));

					drawList->AddCircleFilled(topRight, 4, IM_COL32(44, 44, 44, 255));
					drawList->AddCircleFilled(topRight, 3, IM_COL32(255, 255, 255, 255));
				}
			}
		}

		ImVec2 statsBoxPosition = ImVec2(topLeft.x + ImGui::GetStyle().ItemSpacing.x, topLeft.y + ImGui::GetStyle().ItemSpacing.y);
		ImGui::SetCursorPos(statsBoxPosition);
		if (m_ShowFrameRate)
		{
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(window_pos.x + ImGui::GetStyle().ItemSpacing.x, window_pos.y + ImGui::GetStyle().ItemSpacing.y), ImVec2(window_pos.x + 50, window_pos.y + 24), IM_COL32(0, 0, 0, 30), 3.0f);
			ImGui::Text("%.1f", io.Framerate);
		}

		if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && !ImGuizmo::IsUsing() && (m_MousePositionBeginClick - m_RelativeMousePosition).SqrMagnitude() > 0.01f
			&& m_MousePositionBeginClick.x < m_ViewportSize.x && m_MousePositionBeginClick.y < m_ViewportSize.y
			&& m_MousePositionBeginClick.x > 0.0f && m_MousePositionBeginClick.y > 0.0f
			&& !m_TilemapEditor->IsHovered())
		{
			ImU32 color = ((ImU32)(ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered].x * 255.0f)) |
				((ImU32)(ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered].y * 255.0f) << 8) |
				((ImU32)(ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered].z * 255.0f) << 16) |
				100 << 24;
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(window_pos.x + m_MousePositionBeginClick.x, window_pos.y + m_MousePositionBeginClick.y),
				ImVec2(window_pos.x + m_RelativeMousePosition.x, window_pos.y + m_RelativeMousePosition.y), color);
		}

		if (m_ShowStats)
		{
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(window_pos.x, window_pos.y + ImGui::GetStyle().ItemSpacing.y), ImVec2(window_pos.x + 250, window_pos.y + (24 * 4)), IM_COL32(0, 0, 0, 30), 3.0f);
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
}

void ViewportPanel::OnEvent(Event& event)
{
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<SceneChangedEvent>([&](SceneChangedEvent& event) {
		SceneManager::CurrentScene()->SetShowDebug(m_ShowCollision);

		m_CameraController.SetPosition(Vector3f());
		return false;
		});
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
	return HistoryManager::CanUndo();
}

bool ViewportPanel::CanRedo() const
{
	return HistoryManager::CanRedo();
}

void ViewportPanel::Undo(int astep)
{
	CLIENT_DEBUG("Undid");

	HistoryManager::Undo(astep);

	SceneManager::CurrentScene()->MakeDirty();
}

void ViewportPanel::Redo(int astep)
{
	CLIENT_DEBUG("Redid");

	HistoryManager::Redo(astep);

	SceneManager::CurrentScene()->MakeDirty();
}

void ViewportPanel::HandleKeyboardInputs()
{
	ImGuiIO& io = ImGui::GetIO();
	auto shift = io.KeyShift;
	auto ctrl = io.ConfigMacOSXBehaviors ? io.KeySuper : io.KeyCtrl;
	auto alt = io.ConfigMacOSXBehaviors ? io.KeyCtrl : io.KeyAlt;

	if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape)))
	{
		SceneManager::ChangeSceneState(SceneState::Edit);
	}

	if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F11)))
	{
		m_Fullscreen = !m_Fullscreen;
	}

	if (SceneManager::GetSceneState() == SceneState::Play) {
		if (shift && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Tab)))
		{
			Application::GetWindow()->EnableCursor();
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
			io.ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		}
	}
	else if (alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_P)))
	{
		SceneManager::ChangeSceneState(SceneState::Play);
	}

	if (m_WindowHovered && !ImGui::IsAnyMouseDown())
	{
		if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Q)))
			m_Operation = OperationMode::Select;
		else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_W)))
			m_Operation = OperationMode::Move;
		else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_E)))
			m_Operation = OperationMode::Rotate;
		else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_R)))
			m_Operation = OperationMode::Scale;
		else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_T)))
			m_Operation = OperationMode::Universal;
	}
}