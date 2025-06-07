#include "MaterialView.h"

#include "IconsFontAwesome6.h"
#include "FileSystem/FileDialog.h"
#include "ImGui/ImGuiTextureEdit.h"

#include "MainDockSpace.h"

#include "Engine.h"

MaterialView::MaterialView(bool* show, std::filesystem::path filepath)
	:View("MaterialView"), m_Show(show), m_FilePath(filepath), m_Camera(-1.0f, 1.0f, -1.0f, 1.0f)
{
	m_Mesh = GeometryGenerator::CreateSphere(1.0f, 50, 50);

	FrameBufferSpecification frameBufferSpecification = { 640, 480 };
	frameBufferSpecification.attachments = { FrameBufferTextureFormat::RGBA8 , FrameBufferTextureFormat::Depth };
	m_Framebuffer = FrameBuffer::Create(frameBufferSpecification);
}

void MaterialView::OnAttach()
{
	m_Material = AssetManager::GetAsset<Material>(m_FilePath);

	m_LocalMaterial = CreateRef<Material>(*m_Material);

	m_WindowName = ICON_FA_BOWLING_BALL + std::string(" " + m_FilePath.filename().string());
}

void MaterialView::OnImGuiRender()
{
	if (!*m_Show)
	{
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

	ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar;

	if (m_Dirty)
		flags |= ImGuiWindowFlags_UnsavedDocument;

	ImGui::SetNextWindowSize(ImVec2(1000, 640), ImGuiCond_FirstUseEver);

	if (ImGui::Begin(m_WindowName.c_str(), m_Show, flags))
	{
		if (ImGui::IsWindowFocused())
		{
			MainDockSpace::SetFocussedWindow(this);
		}
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem(ICON_FA_FLOPPY_DISK" Save"))
					Save();
				if (ImGui::MenuItem(ICON_FA_FILE_SIGNATURE" Save As"))
					SaveAs();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
	}

	if (ImGui::BeginTable("##TopeLevel", 2, ImGuiTableFlags_Resizable))
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		if (ImGui::BeginCombo("Shader", m_LocalMaterial->GetShader().c_str()))
		{
			//TODO: have a list of shaders available
			if (ImGui::Selectable("Standard"))
			{
				m_LocalMaterial->SetShader("Standard");
				m_Dirty = true;
			}
			if (ImGui::Selectable("Grid"))
			{
				m_LocalMaterial->SetShader("Grid");
				m_Dirty = true;
			}
			ImGui::EndCombo();
		}

		Colour tintColour = m_LocalMaterial->GetTint();
		if (ImGui::ColorEdit4("Tint", &tintColour[0]))
		{
			m_LocalMaterial->SetTint(tintColour);
			m_Dirty = true;
		}

		bool twoSided = m_LocalMaterial->IsTwoSided();
		if (ImGui::Checkbox("Two Sided", &twoSided))
		{
			m_LocalMaterial->SetTwoSided(twoSided);
			m_Dirty = true;
		}

		bool transparent = m_LocalMaterial->IsTransparent();
		if (ImGui::Checkbox("Transparent", &transparent))
		{
			m_LocalMaterial->SetTransparency(transparent);
			m_Dirty = true;
		}

		bool castShadows = m_LocalMaterial->CastsShadows();
		if (ImGui::Checkbox("Casts Shadows", &castShadows))
		{
			m_LocalMaterial->SetCastShadows(castShadows);
			m_Dirty = true;
		}

		Ref<Texture2D> albedo = m_LocalMaterial->GetTexture(0);
		if (ImGui::Texture2DEdit("Albedo", albedo, ImVec2(128, 128)))
		{
			m_LocalMaterial->AddTexture(albedo, 0);
			m_Dirty = true;
		}

		Ref<Texture2D> normalMap = m_LocalMaterial->GetTexture(1);
		if (ImGui::Texture2DEdit("Normal Map", normalMap, ImVec2(128, 128)))
		{
			m_LocalMaterial->AddTexture(normalMap, 1);
			m_Dirty = true;
		}

		Ref<Texture2D> mixMap = m_LocalMaterial->GetTexture(2);
		if (ImGui::Texture2DEdit("Mix Map", mixMap, ImVec2(128, 128)))
		{
			m_LocalMaterial->AddTexture(mixMap, 2);
			m_Dirty = true;
		}

		ImGui::TableSetColumnIndex(1);

		m_ViewportSize = ImGui::GetContentRegionAvail();

		ImGui::Image(m_Framebuffer->GetColourAttachment(), m_ViewportSize);

		ImGui::EndTable();
	}
	ImGui::End();
}

void MaterialView::OnUpdate(float deltaTime)
{
	PROFILE_FUNCTION();

	FrameBufferSpecification spec = m_Framebuffer->GetSpecification();
	if (((uint32_t)m_ViewportSize.x != spec.width || (uint32_t)m_ViewportSize.y != spec.height) && (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f))
	{
		m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		float aspectRatio = m_ViewportSize.x / m_ViewportSize.y;
		if (aspectRatio > 1.0f)
			m_Camera.SetProjection(-aspectRatio, aspectRatio, -1.0f, 1.0f);
		else
			m_Camera.SetProjection(-1.0f, 1.0f, -1.0f / aspectRatio, 1.0f / aspectRatio);
	}

	m_Framebuffer->Bind();
	RenderCommand::Clear();

	Renderer::BeginScene(Matrix4x4::Translate(Vector3f(0.0f, 0.0f, 1.5f)), m_Camera.GetProjectionMatrix());

	Renderer::Submit(m_Mesh, m_LocalMaterial);

	Renderer::EndScene();
	m_Framebuffer->UnBind();
}

void MaterialView::Save()
{
	m_LocalMaterial->SaveMaterial();
	m_Material->Load(m_FilePath);
	m_LocalMaterial = CreateRef<Material>(*m_Material);
	m_Dirty = false;
}

void MaterialView::SaveAs()
{
	auto ext = m_FilePath.extension();
	std::optional<std::wstring> dialogPath = FileDialog::SaveAs(L"Save As...", ConvertToWideChar(m_FilePath.extension().string()));
	if (dialogPath)
	{
		m_FilePath = dialogPath.value();
		if (!m_FilePath.has_extension())
			m_FilePath.replace_extension(ext);
		Save();
	}
}