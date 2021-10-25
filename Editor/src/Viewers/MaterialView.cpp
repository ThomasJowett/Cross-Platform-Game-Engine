#include "MaterialView.h"

MaterialView::MaterialView(bool* show, std::filesystem::path filepath)
	:Layer("MaterialView"), m_Show(show), m_FilePath(filepath)
{

}

void MaterialView::OnAttach()
{
	m_Material->LoadMaterial(m_FilePath);
}

void MaterialView::OnImGuiRender()
{
	if (!m_Show)
		return;

	if (ImGui::Begin(m_WindowName.c_str(), m_Show))
	{

	}
}

void MaterialView::OnUpdate(float deltaTime)
{
}

void MaterialView::OnFixedUpdate()
{
}
