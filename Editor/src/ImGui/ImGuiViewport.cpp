#include "ImGuiViewport.h"

#include "imgui/imgui_internal.h"
#include "ImGui/ImGuiDockSpace.h"

#include "IconsFontAwesome5.h"

ImGuiViewportPanel::ImGuiViewportPanel(bool* show)
	:m_Show(show), Layer("Viewport")
{
	m_Framebuffer = FrameBuffer::Create({ 1920, 1080 });

	m_Mode = Mode::Select;
}

void ImGuiViewportPanel::OnAttach()
{
	//TEMP CODE
	m_TorusVertexArray = GeometryGenerator::CreateTorus(0.7f, 0.25f, 30);

	m_ShaderLibrary.Load("NormalMap");
	m_Texture = Texture2D::Create(Application::GetWorkingDirectory().string() + "\\resources\\UVChecker.png");

	m_CameraController.SetPosition({ 0.0, 0.0, 2.0 });

	m_Scene = CreateRef<Scene>();

	Entity square = m_Scene->CreateEntity("Test Square");
	square.AddComponent<SpriteComponent>(Colour(Colours::GREEN));
}

void ImGuiViewportPanel::OnUpdate(float deltaTime)
{
	PROFILE_FUNCTION();

	m_Framebuffer->Bind();
	RenderCommand::Clear();

	Ref<Shader> shader = m_ShaderLibrary.Get("NormalMap");
	shader->Bind();

	shader->SetInt("u_texture", 0);
	shader->SetFloat4("u_colour", 1.0f, 1.0f, 1.0f, 1.0f);
	shader->SetFloat("u_tilingFactor", 1.0f);


	static bool cursorDisabled = false;
	bool rightMouseDown = Input::IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);

	if (cursorDisabled)
		m_CameraController.OnUpdate(deltaTime);

	if (m_WindowHovered && rightMouseDown && !cursorDisabled)
	{
		Application::GetWindow().DisableCursor(); //TODO: Fix the issue that disabling the cursor jumps the mouse position
		cursorDisabled = true;
	}

	else if (cursorDisabled && !rightMouseDown)
	{
		cursorDisabled = false;
		Application::GetWindow().EnableCursor();
	}

	Renderer2D::BeginScene(m_OrthoCamera.GetCamera());
	m_Scene->OnUpdate(deltaTime);
	Renderer2D::EndScene();

	//TODO: have a scene object that will be traversed here instead of creating a scene manually
	Renderer::BeginScene(*m_CameraController.GetCamera());
	m_Texture->Bind();

	//Torus
	Renderer::Submit(shader, m_TorusVertexArray, Matrix4x4::Translate({ 0.0f, 0.0f,0.0f })
		* Matrix4x4::Rotate(Vector3f(PI / 2.0f, 0.0f, 0.0f)));

	Renderer::EndScene();
	m_Framebuffer->UnBind();
}

void ImGuiViewportPanel::OnFixedUpdate()
{
	FrameBufferSpecification spec = m_Framebuffer->GetSpecification();
	if (((uint32_t)m_ViewportSize.x != spec.Width || (uint32_t)m_ViewportSize.y != spec.Height) && (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f))
	{
		m_Framebuffer->Resize(m_ViewportSize.x, m_ViewportSize.y);
		m_CameraController.SetAspectRatio(m_ViewportSize.x / m_ViewportSize.y);
	}
}

void ImGuiViewportPanel::OnImGuiRender()
{
	PROFILE_FUNCTION();

	if (!*m_Show)
	{
		return;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	ImGui::SetNextWindowSize(ImVec2(800, 800), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(30, 30), ImGuiCond_FirstUseEver);

	ImVec2 pos;
	ImGuiIO& io = ImGui::GetIO();
	bool viewShown = ImGui::Begin(ICON_FA_BORDER_ALL" Viewport", m_Show, ImGuiWindowFlags_NoScrollbar);
	if (viewShown)
	{
		HandleKeyboardInputs();

		m_WindowHovered = ImGui::IsWindowHovered();

		if (m_WindowHovered)
		{
			if (!Input::IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_COUNT); //HACK: this is to stop imgui from changing the cursor back to something every frame
				Application::GetWindow().SetCursor(Cursors::CrossHair);
			}
		}

		if (ImGui::IsWindowFocused())
		{
			ImGuiDockSpace::SetFocussedWindow(this);
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

		auto tex = m_Framebuffer->GetColourAttachment();

		ImGui::Image((void*)tex, m_ViewportSize, ImVec2(0, 1), ImVec2(1, 0));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Asset", ImGuiDragDropFlags_None))
			{
				std::filesystem::path* file = (std::filesystem::path*)payload->Data;
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

void ImGuiViewportPanel::Copy()
{
	// TODO: viewport copy
	CLIENT_DEBUG("Copied");
}

void ImGuiViewportPanel::Cut()
{
	// TODO: viewport cut
	CLIENT_DEBUG("Cutted");
}

void ImGuiViewportPanel::Paste()
{
	//TODO: viewport paste
	CLIENT_DEBUG("Pasted {0}", std::string(ImGui::GetClipboardText()));
}

void ImGuiViewportPanel::Duplicate()
{
	//TODO: viewport duplicate
	CLIENT_DEBUG("Duplicated");
}

void ImGuiViewportPanel::Delete()
{
	//TODO: viewport delete
	CLIENT_DEBUG("Deleted");
}

void ImGuiViewportPanel::SelectAll()
{
}

bool ImGuiViewportPanel::HasSelection() const
{
	return false;
}

bool ImGuiViewportPanel::CanUndo() const
{
	//TODO: viewport undolist
	return false;
}

bool ImGuiViewportPanel::CanRedo() const
{
	return false;
}

void ImGuiViewportPanel::Undo(int astep)
{
	CLIENT_DEBUG("Undid");
}

void ImGuiViewportPanel::Redo(int astep)
{
	CLIENT_DEBUG("Redid");
}

void ImGuiViewportPanel::Save()
{
	CLIENT_DEBUG("Saving...");
}

void ImGuiViewportPanel::SaveAs()
{
	CLIENT_DEBUG("Saving As...");
}

void ImGuiViewportPanel::HandleKeyboardInputs()
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
