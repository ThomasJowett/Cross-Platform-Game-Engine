#include "PhysicsMaterialView.h"
#include "IconsFontAwesome6.h"
#include "Scene/AssetManager.h"
#include "MainDockSpace.h"
#include "FileSystem/FileDialog.h"
#include "Utilities/StringUtils.h"

#include "imgui/imgui.h"
#include "ImGui/ImGuiUtilites.h"

PhysicsMaterialView::PhysicsMaterialView(bool* show, std::filesystem::path filepath)
	:View("PhysicsMaterialView"), m_Show(show), m_FilePath(filepath)
{
}

void PhysicsMaterialView::OnAttach()
{
	m_PhysicsMaterial = AssetManager::GetAsset<PhysicsMaterial>(m_FilePath);
	m_WindowName = ICON_FA_VOLLEYBALL + std::string(" " + m_FilePath.filename().string());
}

void PhysicsMaterialView::OnImGuiRender()
{
	if (!*m_Show)
		return;
	ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar;

	if (m_Dirty)
		flags |= ImGuiWindowFlags_UnsavedDocument;

	ImGui::SetNextWindowSize(ImVec2(300, 150), ImGuiCond_FirstUseEver);

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

		if (float density = m_PhysicsMaterial->GetDensity(); ImGui::DragFloat("Density", &density, 0.001f, 0.0f, 100.0f))
		{
			m_PhysicsMaterial->SetDensity(density);
			m_Dirty = true;
		}
		ImGui::Tooltip("The substance's mass per unit of volume");


		if (float friction = m_PhysicsMaterial->GetFriction(); ImGui::DragFloat("Friction", &friction, 0.001f, 0.0f, 1.0f))
		{
			m_PhysicsMaterial->SetFriction(friction);
			m_Dirty = true;
		}
		ImGui::Tooltip("The force between two surfaces that are sliding, or trying to slide, across each other");

		if (float restitution = m_PhysicsMaterial->GetRestitution(); ImGui::DragFloat("Restitution", &restitution, 0.001f, 0.0f, 1.0f))
		{
			m_PhysicsMaterial->SetRestitution(restitution);
			m_Dirty = true;
		}
		ImGui::Tooltip("The restoration of an objects speed after a collision");
	}
	ImGui::End();
}

void PhysicsMaterialView::Save()
{
	m_PhysicsMaterial->Save();
	m_Dirty = false;
}

void PhysicsMaterialView::SaveAs()
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

void PhysicsMaterialView::Undo(int asteps)
{
}

void PhysicsMaterialView::Redo(int asteps)
{
}

bool PhysicsMaterialView::CanUndo() const
{
	return false;
}

bool PhysicsMaterialView::CanRedo() const
{
	return false;
}
