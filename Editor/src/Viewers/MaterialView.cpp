#include "MaterialView.h"

#include "IconsFontAwesome5.h"
#include "FileSystem/FileDialog.h"
#include "ImGui/ImGuiTextureEdit.h"

MaterialView::MaterialView(bool* show, std::filesystem::path filepath)
	:Layer("MaterialView"), m_Show(show), m_FilePath(filepath)
{

}

void MaterialView::OnAttach()
{
	m_Material = CreateRef<Material>(m_FilePath);

	m_WindowName = ICON_FA_BACON + std::string(" " + m_FilePath.filename().string());
}

void MaterialView::OnImGuiRender()
{
	if (!*m_Show)
		return;

	ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar;

	if (m_Dirty)
		flags |= ImGuiWindowFlags_UnsavedDocument;

	ImGui::SetNextWindowSize(ImVec2(640, 680), ImGuiCond_FirstUseEver);

	if (ImGui::Begin(m_WindowName.c_str(), m_Show, flags))
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem(ICON_FA_SAVE" Save"))
					Save();
				if (ImGui::MenuItem(ICON_FA_FILE_SIGNATURE" Save As"))
					SaveAs();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
	}

	if (ImGui::BeginCombo("Shader", m_Material->GetShader().c_str()))
	{
		//TODO: have a list of shaders available
		if (ImGui::Selectable("Standard"))
		{
			m_Material->SetShader("Standard");
			m_Dirty = true;
		}
		ImGui::EndCombo();
	}

	//TODO: get the number of available texture slots through shader reflection

	Ref<Texture2D> albedo = m_Material->GetTexture(0);
	if (ImGui::Texture2DEdit("Albedo", albedo, ImVec2(128, 128)))
	{
		m_Material->AddTexture(albedo, 0);
		m_Dirty = true;
	}

	Ref<Texture2D> normalMap = m_Material->GetTexture(1);
	if (ImGui::Texture2DEdit("Normal Map", normalMap, ImVec2(128, 128)))
	{
		m_Material->AddTexture(normalMap, 0);
		m_Dirty = true;
	}

	Ref<Texture2D> mixMap = m_Material->GetTexture(2);
	if (ImGui::Texture2DEdit("Mix Map", mixMap, ImVec2(128, 128)))
	{
		m_Material->AddTexture(mixMap, 0);
		m_Dirty = true;
	}
	ImGui::End();
}

void MaterialView::OnUpdate(float deltaTime)
{
}

void MaterialView::OnFixedUpdate()
{
}

void MaterialView::Save()
{
	m_Material->SaveMaterial();
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
