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
	
	mz_zip_archive zip = {};
	memset(&zip, 0, sizeof(zip));

	std::filesystem::path outputZipPath = m_ExportDirectory / "packed_assets.zip";

	if (!mz_zip_writer_init_file(&zip, outputZipPath.string().c_str(), 0)) {
		ENGINE_ERROR("Failed to initialize zip writer: {0}", mz_zip_get_error_string(mz_zip_get_last_error(&zip)));
		return;
	}

	for (const auto& asset : m_SelectedAssets)
	{
		std::string relativePath = std::filesystem::relative(asset, m_ProjectDirectory).string();
		std::replace(relativePath.begin(), relativePath.end(), '\\', '/'); // Ensure forward slashes for zip
		if (!mz_zip_writer_add_file(&zip, relativePath.c_str(), asset.string().c_str(), nullptr, 0, 0))
		{
			ENGINE_ERROR("Failed to add file to zip: {0}", mz_zip_get_error_string(mz_zip_get_last_error(&zip)));
			mz_zip_writer_end(&zip);
			return;
		}
	}

	// TODO: Add shaders to the zip file

	if (!mz_zip_writer_finalize_archive(&zip))
	{
		ENGINE_ERROR("Failed to finalize zip archive: {0}", mz_zip_get_error_string(mz_zip_get_last_error(&zip)));
	}

	mz_zip_writer_end(&zip);

	ExportGame();
}
