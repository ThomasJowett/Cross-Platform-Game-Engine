#include "StaticMeshView.h"

#include "IconsFontAwesome6.h"
#include "MainDockSpace.h"
#include "Engine.h"

StaticMeshView::StaticMeshView(bool* show, std::filesystem::path filepath)
	:View("StaticMeshView"), m_Show(show), m_FilePath(filepath)
{
	FrameBufferSpecification frameBufferSpecification = { 640, 480 };
	frameBufferSpecification.attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::Depth };
	m_Framebuffer = FrameBuffer::Create(frameBufferSpecification);
}

void StaticMeshView::OnAttach()
{
	m_WindowName = ICON_FA_SHAPES + std::string(" " + m_FilePath.filename().string());

	m_Mesh = AssetManager::GetStaticMesh(m_FilePath);

	//m_StandardMaterial = CreateRef<Material>("Standard", Colours::WHITE);
	//m_StandardMaterial->AddTexture(Texture2D::Create(std::filesystem::path(Application::GetWorkingDirectory() / "resources" / "UVChecker.png").string()), 0);

	Ref<Material> gridMaterial = CreateRef<Material>("Grid", Colours::GREY);
	gridMaterial->SetTwoSided(true);
	gridMaterial->SetTilingFactor(100.0f);
	m_GridMesh = CreateRef<Mesh>(GeometryGenerator::CreateGrid(1000.0f, 1000.0f, 1, 1, 1.0f, 1.0f), gridMaterial);

	m_CameraController.SetPosition({ 0.0, 0.0, 0.0 });
	m_CameraController.SwitchCamera(true);
	//future code
	// m_CameraController.SetPosition({0.0,0.0, m_Mesh->GetBounds().radius})
}

void StaticMeshView::OnImGuiRender()
{
	if (!*m_Show)
	{
		return;
	}

	ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
	ImVec2 pos;
	ImGuiIO& io = ImGui::GetIO();
	if (ImGui::Begin(m_WindowName.c_str(), m_Show, flags))
	{
		m_WindowHovered = ImGui::IsWindowHovered();
		m_WindowFocussed = ImGui::IsWindowFocused();

		if (m_WindowHovered)
		{
			if (!Input::IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
			{
				//ImGui::SetMouseCursor(ImGuiMouseCursor_COUNT); //HACK: this is to stop imgui from changing the cursor back to something every frame
				//Application::GetWindow().SetCursor(Cursors::CrossHair);
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
		}

		pos = ImGui::GetCursorScreenPos();
		ImVec2 mouse_pos = ImGui::GetMousePos();

		m_RelativeMousePosition = { mouse_pos.x - ImGui::GetWindowPos().x - 1.0f, mouse_pos.y - ImGui::GetWindowPos().y - 8.0f - ImGui::GetFontSize() };

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::MenuItem("Reset camera"))
				{
					//m_CameraController.SetPosition(Vector3f(0.0f, 0.0f, 0.0f));
					m_CameraController.LookAt(Vector3f(0.0f, 0.0f, 0.0f), 1.0f);
				}
				ImGui::EndMenu();
			}
		}
		ImGui::EndMenuBar();

		uint64_t tex = (uint64_t)m_Framebuffer->GetColourAttachment();

		ImGui::Image((void*)tex, m_ViewportSize, ImVec2(0, 1), ImVec2(1, 0));
	}
	ImGui::End();

	ImGui::PopStyleVar();
}

void StaticMeshView::OnUpdate(float deltaTime)
{
	PROFILE_FUNCTION();

	FrameBufferSpecification spec = m_Framebuffer->GetSpecification();
	if (((uint32_t)m_ViewportSize.x != spec.width || (uint32_t)m_ViewportSize.y != spec.height) && (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f))
	{
		m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_CameraController.SetAspectRatio(m_ViewportSize.x / m_ViewportSize.y);
	}

	m_Framebuffer->Bind();
	RenderCommand::Clear();

	bool rightMouseDown = Input::IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);

	if (m_CursorDisabled)
		m_CameraController.OnUpdate(deltaTime, m_WindowHovered);

	if (m_WindowHovered && rightMouseDown && !m_CursorDisabled)
	{
		Application::GetWindow().DisableCursor(); //TODO: Fix the issue that disabling the cursor jumps the mouse position
		m_CursorDisabled = true;
	}

	if (m_CursorDisabled && !rightMouseDown)
	{
		m_CursorDisabled = false;
		Application::GetWindow().EnableCursor();
	}

	Renderer::BeginScene(m_CameraController.GetTransformMatrix(), m_CameraController.GetCamera()->GetProjectionMatrix());

	for (auto mesh : m_Mesh->GetMeshes())
	{
		Renderer::Submit(mesh->GetMaterial(), mesh->GetVertexArray());
	}

	Renderer::Submit(m_GridMesh->GetMaterial(), m_GridMesh->GetVertexArray());

	Renderer::EndScene();
	m_Framebuffer->UnBind();
}

void StaticMeshView::OnFixedUpdate()
{
}