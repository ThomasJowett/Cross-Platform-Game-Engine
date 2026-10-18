#include "StaticMeshView.h"

#include "IconsFontAwesome6.h"
#include "MainDockSpace.h"
#include "Utilities/GeometryGenerator.h"
#include "Utilities/MeshSerializer.h"
#include "Renderer/RenderCommand.h"
#include "Core/Input.h"
#include "ImGui/ImGuiUtilities.h"
#include "ImGui/ImGuiAssetEdit.h"
#include "Renderer/Renderer.h"
#include "FileSystem/FileDialog.h"
#include "FileSystem/WebProject.h"
#include "ViewportCameraController.h"

StaticMeshView::StaticMeshView(bool* show, std::filesystem::path filepath)
	:View("StaticMeshView"), m_Show(show), m_FilePath(filepath)
{
	FrameBufferSpecification frameBufferSpecification = { 640, 480 };
	frameBufferSpecification.attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER, FrameBufferTextureFormat::Depth };
	m_Framebuffer = FrameBuffer::Create(frameBufferSpecification);
}

void StaticMeshView::OnAttach()
{
	m_WindowName = ICON_FA_SHAPES + std::string(" " + m_FilePath.filename().string());

	m_Mesh = AssetManager::GetAsset<StaticMesh>(m_FilePath);

	if (m_Mesh && m_Mesh->GetMesh())
	{
		m_Materials = m_Mesh->GetMesh()->GetMaterials();
		m_Bounds = m_Mesh->GetBounds();
	}
	m_NeedsFraming = true;

	m_GridMaterial = CreateRef<Material>("Grid", Colour(0.2f, 0.2f, 0.2f, 1.0f));
	m_GridMaterial->SetTwoSided(true);
	m_GridMaterial->SetTransparency(true);
	m_GridMesh = GeometryGenerator::CreateGrid(1000.0f, 1000.0f, 2, 2, 1.0f, 1.0f);
	m_GridMesh->SetMaterials({ m_GridMaterial });
}

void StaticMeshView::OnImGuiRender()
{
	if (!*m_Show)
	{
		if (m_MouseLooking)
		{
			m_MouseLooking = false;
			ViewportCameraController::EndMouseLook();
		}

		if (m_Dirty)
		{
			ImGui::OpenPopup("Save Prompt");
		}

		if (ImGui::BeginPopupModal("Save Prompt"))
		{
			ImGui::TextUnformatted("Save unsaved changes?");
			if (ImGui::Button("Save"))
			{
				Save();
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Don't Save"))
			{
				m_Dirty = false;
				if (m_Mesh && m_Mesh->GetMesh())
					m_Materials = m_Mesh->GetMesh()->GetMaterials();
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				*m_Show = true;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
		return;
	}

	ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

	if (m_Dirty)
		flags |= ImGuiWindowFlags_UnsavedDocument;

	ImGui::SetNextWindowSize(ImVec2(960, 540), ImGuiCond_FirstUseEver);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	bool open = ImGui::Begin(m_WindowName.c_str(), m_Show, flags);
	ImGui::PopStyleVar();

	if (open)
	{
		if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
		{
			MainDockSpace::SetFocussedWindow(this);

			if (!ImGui::GetIO().WantTextInput && !ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_F))
				m_CameraController.Frame(m_Bounds);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem(ICON_FA_FLOPPY_DISK" Save", "Ctrl + S", nullptr, m_Dirty))
					Save();
				if (ImGui::MenuItem(ICON_FA_FILE_SIGNATURE" Save As", "Ctrl + Shift + S"))
					SaveAs();
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::MenuItem("Frame mesh", "F"))
					m_CameraController.Frame(m_Bounds);
				ImGui::Separator();
				for (const AxisView& view : s_AxisViews)
				{
					if (ImGui::MenuItem(view.name))
						m_CameraController.SetView(view.pitch, view.yaw);
				}
				ImGui::Separator();
				ImGui::MenuItem("Show grid", nullptr, &m_ShowGrid);
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));
		if (ImGui::BeginTable("##StaticMeshLayout", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV))
		{
			ImGui::TableSetupColumn("Details", ImGuiTableColumnFlags_WidthFixed, 280.0f);
			ImGui::TableSetupColumn("Viewport", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			if (ImGui::BeginChild("##Details", ImVec2(0, 0), ImGuiChildFlags_AlwaysUseWindowPadding))
				DrawDetailsPanel();
			ImGui::EndChild();

			ImGui::TableSetColumnIndex(1);
			DrawViewport();

			ImGui::EndTable();
		}
		ImGui::PopStyleVar();
	}
	ImGui::End();
}

void StaticMeshView::DrawDetailsPanel()
{
	if (!m_Mesh || !m_Mesh->GetMesh())
	{
		ImGui::TextUnformatted("Could not load mesh");
		return;
	}

	Ref<Mesh> mesh = m_Mesh->GetMesh();

	if (ImGui::CollapsingHeader("Materials", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (size_t i = 0; i < m_Materials.size(); ++i)
		{
			Ref<Material> previous = m_Materials[i];
			std::string label = "Material " + std::to_string(i);
			ImGui::AssetEdit<Material>(label.c_str(), m_Materials[i], Material::GetDefaultMaterial(), FileType::MATERIAL);

			if (!m_Materials[i])
				m_Materials[i] = Material::GetDefaultMaterial();

			if (m_Materials[i] != previous)
				m_Dirty = true;
		}
	}

	if (ImGui::CollapsingHeader("Mesh Info", ImGuiTreeNodeFlags_DefaultOpen))
	{
		uint32_t floatsPerVertex = mesh->GetVertexLayout().GetStride() / sizeof(float);
		uint32_t vertexCount = floatsPerVertex ? (uint32_t)mesh->GetVertexList().size() / floatsPerVertex : 0;
		Vector3f size = m_Bounds.IsValid() ? m_Bounds.Max() - m_Bounds.Min() : Vector3f();

		ImGui::Text("Vertices: %u", vertexCount);
		ImGui::Text("Triangles: %u", mesh->GetIndexCount() / 3);
		ImGui::Text("Submeshes: %u", (uint32_t)mesh->GetSubmeshes().size());
		ImGui::Text("Size: %.3f x %.3f x %.3f", size.x, size.y, size.z);
	}
}

void StaticMeshView::DrawViewport()
{
	ImVec2 panelSize = ImGui::GetContentRegionAvail();
	if (m_ViewportSize.x != panelSize.x || m_ViewportSize.y != panelSize.y)
	{
		m_ViewportSize = panelSize;
		m_CameraController.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
	}

	if (m_ViewportSize.x <= 0.0f || m_ViewportSize.y <= 0.0f)
		return;

	// Framing needs the aspect ratio, which isn't known until the panel has a size
	if (m_NeedsFraming)
	{
		m_CameraController.Frame(m_Bounds);
		m_NeedsFraming = false;
	}

	ImVec2 viewportPos = ImGui::GetCursorScreenPos();
	ImGui::Image(m_Framebuffer->GetColourAttachment(), m_ViewportSize);

	// Holding an active item stops other windows, like the main viewport, reacting to this drag
	ImGui::SetCursorScreenPos(viewportPos);
	ImGui::InvisibleButton("##ViewportInput", m_ViewportSize,
		ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | ImGuiButtonFlags_MouseButtonMiddle);

	auto [mouseX, mouseY] = Input::GetMousePos();
	Vector2f mousePosition((float)mouseX, (float)mouseY);

	if (ImGui::IsItemActivated())
	{
		ViewportCameraController::BeginMouseLook();
		m_MouseLooking = true;
		// Disabling the cursor can jump its reported position on the next frame, so skip until that's passed
		m_IgnoreMouseFrames = 2;
	}

	if (m_MouseLooking)
	{
		if (ImGui::IsItemActive())
		{
			Vector2f mouseDelta = mousePosition - m_LastMousePosition;
			if (m_IgnoreMouseFrames > 0)
			{
				--m_IgnoreMouseFrames;
			}
			else if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
			{
				m_CameraController.Pan(mouseDelta);
			}
			else
			{
				m_CameraController.Orbit(mouseDelta);
			}
		}
		else
		{
			m_MouseLooking = false;
			ViewportCameraController::EndMouseLook();
		}
	}

	m_LastMousePosition = mousePosition;

	if (ImGui::IsItemHovered() && ImGui::GetIO().MouseWheel != 0.0f)
		m_CameraController.Zoom(ImGui::GetIO().MouseWheel);
}

void StaticMeshView::OnUpdate(float deltaTime)
{
	PROFILE_FUNCTION();

	if (!*m_Show)
		return;

	FrameBufferSpecification spec = m_Framebuffer->GetSpecification();
	if (((uint32_t)m_ViewportSize.x != spec.width || (uint32_t)m_ViewportSize.y != spec.height) && (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f))
	{
		m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
	}

	m_Framebuffer->Bind();
	RenderCommand::StartRenderPass();
	RenderCommand::Clear();

	Renderer::BeginScene(m_CameraController.GetTransformMatrix(), m_CameraController.GetProjectionMatrix());

	if (m_Mesh && m_Mesh->GetMesh())
		Renderer::Submit(m_Mesh->GetMesh(), m_Materials);

	if (m_ShowGrid)
	{
		// The grid shader reads its fade radius from the tiling factor, scale both with the zoom
		float fadeRadius = std::max(m_CameraController.GetDistance() * 4.0f, 10.0f);
		m_GridMaterial->SetTilingFactor(fadeRadius);
		float gridScale = std::max(1.0f, fadeRadius * 3.0f / 500.0f);

		Vector3f eyePosition = m_CameraController.GetPosition();
		Matrix4x4 gridTransform = Matrix4x4::Translate(Vector3f(eyePosition.x, 0.0f, eyePosition.z))
			* Matrix4x4::Scale(Vector3f(gridScale, 1.0f, gridScale));
		Renderer::Submit(m_GridMesh, gridTransform);
	}

	Renderer::EndScene();
	RenderCommand::EndRenderPass();
	m_Framebuffer->UnBind();
}

void StaticMeshView::OnFixedUpdate()
{
}

void StaticMeshView::Save()
{
	if (!m_Mesh || !m_Mesh->GetMesh())
		return;

	// Write a copy so the cached asset only changes once the file has been written
	Ref<Mesh> mesh = CreateRef<Mesh>(*m_Mesh->GetMesh());
	mesh->SetMaterials(m_Materials);

	std::filesystem::path absolutePath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / m_FilePath);
	if (!MeshSerializer::Serialize(absolutePath, mesh, MeshLayout::StaticMesh))
		return;

	if (Ref<StaticMesh> savedMesh = AssetManager::GetAsset<StaticMesh>(m_FilePath))
	{
		m_Mesh = savedMesh;
		if (m_Mesh->GetMesh())
			m_Mesh->GetMesh()->SetMaterials(m_Materials);
	}

	m_Dirty = false;

	WebProject::Sync();
}

void StaticMeshView::SaveAs()
{
	auto ext = m_FilePath.extension();
	std::optional<std::wstring> dialogPath = FileDialog::SaveAs(L"Save As...", { {L"Static Mesh File", std::wstring(L"*") + ConvertToWideChar(ext.string())} });

	if (dialogPath)
	{
		std::filesystem::path newPath = dialogPath.value();
		if (!newPath.has_extension())
			newPath.replace_extension(ext);

		m_FilePath = FileUtils::RelativePath(newPath, Application::GetOpenDocumentDirectory());
		Save();
	}
}
