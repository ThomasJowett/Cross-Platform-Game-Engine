#include "StaticMeshView.h"

#include "IconsFontAwesome5.h"
#include "MainDockSpace.h"

StaticMeshView::StaticMeshView(bool* show, std::filesystem::path filepath)
	:Layer("StaticMeshView"), m_Show(show), m_FilePath(filepath)
{
	m_Framebuffer = FrameBuffer::Create({ 640, 480 });
}

void StaticMeshView::OnAttach()
{
	m_WindowName = ICON_FA_SHAPES + std::string(" " + m_FilePath.filename().string());

	m_Mesh = CreateRef<Mesh>(m_FilePath);

	m_ShaderLibrary.Load("NormalMap");
	m_Texture = Texture2D::Create(Application::GetWorkingDirectory().string() + "\\resources\\UVChecker.png");

	m_CameraController.SetPosition({ 0.0, 0.0, 2.0 });
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

		ImGui::Columns(2);

		ImVec2 panelSize = ImGui::GetContentRegionAvail();
		if (m_ViewportSize.x != panelSize.x || m_ViewportSize.y != panelSize.y)
		{
			m_ViewportSize = panelSize;
		}

		pos = ImGui::GetCursorScreenPos();
		ImVec2 mouse_pos = ImGui::GetMousePos();

		m_RelativeMousePosition = { mouse_pos.x - ImGui::GetWindowPos().x - 1.0f, mouse_pos.y - ImGui::GetWindowPos().y - 8.0f - ImGui::GetFontSize() };

		m_CameraController.OnMouseMotion(m_RelativeMousePosition);

		//if (m_CursorDisabled)
		//	ImGui:

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				ImGui::EndMenu();
			}
		}
		ImGui::EndMenuBar();

		uint64_t tex = (uint64_t)m_Framebuffer->GetColourAttachment();

		ImGui::Image((void*)tex, m_ViewportSize, ImVec2(0, 1), ImVec2(1, 0));

		//ImGui::NextColumn();
		//
		//ImGui::Image((void*)tex, m_ViewportSize, ImVec2(0, 1), ImVec2(1, 0));
	}
	ImGui::End();

	ImGui::PopStyleVar();
}

void StaticMeshView::OnUpdate(float deltaTime)
{
	PROFILE_FUNCTION();

	m_Framebuffer->Bind();
	RenderCommand::Clear();

	Ref<Shader> shader = m_ShaderLibrary.Get("NormalMap");
	shader->Bind();

	shader->SetInt("u_texture", 0);
	shader->SetFloat4("u_colour", Colours::WHITE);
	shader->SetFloat("u_tilingFactor", 1.0f);

	//m_CursorDisabled = false;
	//TODO: fix the cursor position stuff
	bool rightMouseDown = Input::IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);

	if (m_CursorDisabled)
		m_CameraController.OnUpdate(deltaTime);

	if (m_WindowHovered && rightMouseDown && !m_CursorDisabled)
	{
		Application::GetWindow().DisableCursor(); //TODO: Fix the issue that disabling the cursor jumps the mouse position
		m_CursorDisabled = true;
	}

	if (m_WindowHovered && !rightMouseDown)
	{
		m_CursorDisabled = false;
		Application::GetWindow().EnableCursor();
	}

	Renderer::BeginScene(m_CameraController.GetTransformMatrix(), m_CameraController.GetCamera()->GetProjectionMatrix());
	m_Texture->Bind();

	Renderer::Submit(shader, m_Mesh->GetVertexArray(), Matrix4x4());

	Renderer::EndScene();
	m_Framebuffer->UnBind();
}

void StaticMeshView::OnFixedUpdate()
{
	FrameBufferSpecification spec = m_Framebuffer->GetSpecification();
	if (((uint32_t)m_ViewportSize.x != spec.Width || (uint32_t)m_ViewportSize.y != spec.Height) && (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f))
	{
		m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_CameraController.SetAspectRatio(m_ViewportSize.x / m_ViewportSize.y);
	}
}