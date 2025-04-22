#include "AssetPacker.h"

#include "Directory.h"
#include "Viewers/ViewerManager.h"
#include <Logging/Instrumentor.h>
#include <imgui.h>



AssetPacker::AssetPacker(bool* show, const std::filesystem::path& projectDirectory)
	: Layer("Asset Packer"), m_Show(show), m_ProjectDirectory(projectDirectory)
{
	DiscoverAssets();
}

void AssetPacker::OnImGuiRender()
{
	PROFILE_FUNCTION();
	if (!m_Show)
	{
		return;
	}

	ImGui::OpenPopup("Pack Assets");

	if (ImGui::BeginPopupModal("Pack Assets", &m_Show, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Select assets to pack into the bundle:");
		ImGui::Separator();
		for (const auto& asset : m_DiscoveredAssets)
		{
			bool selected = m_SelectedAssets.find(asset) != m_SelectedAssets.end();
			if (ImGui::Checkbox(asset.filename().string().c_str(), &selected))
			{
				if (selected)
					m_SelectedAssets.insert(asset);
				else
					m_SelectedAssets.erase(asset);
			}
		}
		if (ImGui::Button("Pack Selected Assets"))
		{
			PackAssets();
			m_Show = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			m_Show = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void AssetPacker::DiscoverAssets()
{
	PROFILE_FUNCTION();

	// Get all the asset extensions
	std::vector<std::string> wantedExtensions = ViewerManager::GetAllExtensions();

	// Get all files in the project directory
	m_DiscoveredAssets = Directory::GetFilesRecursive(m_ProjectDirectory, wantedExtensions);

}

void AssetPacker::PackAssets()
{
	PROFILE_FUNCTION();
	// TODO: Implement the packing logic
}