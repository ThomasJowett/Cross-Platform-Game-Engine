#include "ViewportPanel.h"

#include "MainDockSpace.h"

#include "IconsFontAwesome5.h"

#include "Scene/SceneManager.h"

ViewportPanel::ViewportPanel(bool* show)
	:m_Show(show), Layer("Viewport")
{
	m_Framebuffer = FrameBuffer::Create({ 1920, 1080 });

	m_Mode = Mode::Select;
}

void ViewportPanel::OnAttach()
{
	//TEMP CODE


	m_CameraController.SetPosition({ 0.0, 0.0, 2.0 });

	m_Scene = CreateRef<Scene>("Test Scene");

	SceneManager::SetScene(m_Scene);

	//TODO: load the default scene and deserialize it
	//for now it just loads a mesh of a bucket

	Entity square = m_Scene->CreateEntity("Test Square");
	square.AddComponent<SpriteComponent>(Colour(0.0f,1.0f, 0.0f, 0.5f));
	
	SceneCamera orthoCamera;
	orthoCamera.SetOrthoGraphic(1, 1.0f, -1.0f);
	square = m_Scene->CreateEntity("2D Camera");
	square.AddComponent<CameraComponent>(orthoCamera, true);


	Mesh mesh(Application::GetWorkingDirectory().string() + "\\resources\\Bucket.staticMesh");

	m_ShaderLibrary.Load("NormalMap");
	Material material(m_ShaderLibrary.Get("NormalMap"));

	material.AddTexture(Texture2D::Create(Application::GetWorkingDirectory().string() + "\\resources\\Bucket_Texture.png"), 0);

	Entity entity = m_Scene->CreateEntity("Bucket");
	entity.AddComponent<StaticMeshComponent>(mesh, material);

	entity.GetComponent<TransformComponent>() = Matrix4x4::Translate({ -1.0f, -1.5f,0.0f })
		* Matrix4x4::Rotate(Vector3f(-PI / 2.0f, 0.0f, 0.0f));

	//Camera camera = Camera(Matrix4x4::PerspectiveRH(PI * 0.5, 16.0 / 9.0, 1.0, -1.0), Matrix4x4());
	SceneCamera camera;

	camera.SetPosition({ 0.0, 0.0, 20.0 });

	entity = m_Scene->CreateEntity("MainCamera");
	entity.AddComponent<CameraComponent>(camera, true);

	class CameraController :public ScriptableEntity
	{

	public:
		void OnUpdate(float deltaTime) override
		{
			//CLIENT_DEBUG(deltaTime);
		}
	};

	entity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
}

void ViewportPanel::OnUpdate(float deltaTime)
{
	PROFILE_FUNCTION();

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
	//Renderer::BeginScene(*m_CameraController.GetCamera());
	//Renderer2D::BeginScene(m_OrthoCamera.GetCamera());

	m_Scene->OnUpdate(deltaTime);

	//Renderer2D::EndScene();
	//Renderer::EndScene();

	m_Framebuffer->UnBind();
}

void ViewportPanel::OnFixedUpdate()
{
	FrameBufferSpecification spec = m_Framebuffer->GetSpecification();
	if (((uint32_t)m_ViewportSize.x != spec.Width || (uint32_t)m_ViewportSize.y != spec.Height) && (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f))
	{
		m_Framebuffer->Resize(m_ViewportSize.x, m_ViewportSize.y);
		m_CameraController.SetAspectRatio(m_ViewportSize.x / m_ViewportSize.y);

		m_Scene->OnViewportResize(m_ViewportSize.x, m_ViewportSize.y);
	}
}

void ViewportPanel::OnImGuiRender()
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

void ViewportPanel::Copy()
{
	// TODO: viewport copy
	CLIENT_DEBUG("Copied");
}

void ViewportPanel::Cut()
{
	// TODO: viewport cut
	CLIENT_DEBUG("Cutted");
}

void ViewportPanel::Paste()
{
	//TODO: viewport paste
	CLIENT_DEBUG("Pasted {0}", std::string(ImGui::GetClipboardText()));
}

void ViewportPanel::Duplicate()
{
	//TODO: viewport duplicate
	CLIENT_DEBUG("Duplicated");
}

void ViewportPanel::Delete()
{
	//TODO: viewport delete
	CLIENT_DEBUG("Deleted");
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
	//TODO: viewport undolist
	return false;
}

bool ViewportPanel::CanRedo() const
{
	return false;
}

void ViewportPanel::Undo(int astep)
{
	CLIENT_DEBUG("Undid");
}

void ViewportPanel::Redo(int astep)
{
	CLIENT_DEBUG("Redid");
}

void ViewportPanel::Save()
{
	CLIENT_DEBUG("Saving...");
}

void ViewportPanel::SaveAs()
{
	CLIENT_DEBUG("Saving As...");
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
