#include "AssetPacker.h"

#include "Directory.h"
#include "Viewers/ViewerManager.h"
#include <Logging/Instrumentor.h>
#include <imgui.h>

#include "cereal/archives/json.hpp"
#include "cereal/types/string.hpp"

#include <miniz.h>

struct AssetBundleFooter {
	uint64_t zipSize;
	uint64_t gameTitleSize;
	uint64_t defaultSceneSize;
	char magic[8];
};

constexpr const char* BUNDLE_MAGIC = "GMBUNDLE";

AssetPacker::AssetPacker(bool* show, const std::filesystem::path& projectDirectory, const std::filesystem::path& exportDirectory)
	: Layer("Asset Packer"), m_Show(show), m_ProjectDirectory(projectDirectory), m_ExportDirectory(exportDirectory)
{
	DiscoverAssets();

	for (const auto& asset : m_DiscoveredAssets)
	{
		m_SelectedAssets.insert(asset);
	}

	m_ExportDirectory.remove_filename();
	m_GameName = exportDirectory.filename();

#ifdef _WINDOWS
	m_GameName.replace_extension(".exe");
#endif // _WINDOWS

	std::ifstream file(Application::GetOpenDocument());

	if (!file.is_open()) return;

	cereal::JSONInputArchive input(file);
	input(m_Data);

	file.close();
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
		if (!mz_zip_writer_add_file(&zip, relativePath.c_str(), asset.string().c_str(), nullptr, 0, MZ_BEST_SPEED))
		{
			ENGINE_ERROR("Failed to add file to zip: {0}", mz_zip_get_error_string(mz_zip_get_last_error(&zip)));
			mz_zip_writer_end(&zip);
			return;
		}
	}

	std::filesystem::path dataPath = Application::GetWorkingDirectory();

	std::filesystem::path shadersPath = dataPath / "data" / "Shaders";

	if (std::filesystem::exists(shadersPath) && std::filesystem::is_directory(shadersPath))
	{
		for (const auto& shader : std::filesystem::directory_iterator(shadersPath))
		{
			std::string relativePath = std::filesystem::relative(shader, dataPath).string();
			std::replace(relativePath.begin(), relativePath.end(), '\\', '/'); // Ensure forward slashes for zip
			if (!mz_zip_writer_add_file(&zip, relativePath.c_str(), shader.path().string().c_str(), nullptr, 0, MZ_BEST_SPEED))
			{
				ENGINE_ERROR("Failed to add file to zip: {0}", mz_zip_get_error_string(mz_zip_get_last_error(&zip)));
				mz_zip_writer_end(&zip);
				return;
			}
		}
	}

	std::filesystem::path fontsPath = dataPath / "data" / "Fonts";

	if (std::filesystem::exists(fontsPath) && std::filesystem::is_directory(fontsPath))
	{
		for (const auto& font : std::filesystem::directory_iterator(fontsPath))
		{
			std::string relativePath = std::filesystem::relative(font, dataPath).string();
			std::replace(relativePath.begin(), relativePath.end(), '\\', '/'); // Ensure forward slashes for zip
			if (!mz_zip_writer_add_file(&zip, relativePath.c_str(), font.path().string().c_str(), nullptr, 0, MZ_BEST_SPEED))
			{
				ENGINE_ERROR("Failed to add file to zip: {0}", mz_zip_get_error_string(mz_zip_get_last_error(&zip)));
				mz_zip_writer_end(&zip);
				return;
			}
		}
	}

	if (!mz_zip_writer_finalize_archive(&zip))
	{
		ENGINE_ERROR("Failed to finalize zip archive: {0}", mz_zip_get_error_string(mz_zip_get_last_error(&zip)));
	}

	mz_zip_writer_end(&zip);

	ExportGame();
}

void AssetPacker::ExportGame()
{
	PROFILE_FUNCTION();

	std::filesystem::path exePath = Application::GetWorkingDirectory() / "runtime" / "Runtime.exe";
	if (!std::filesystem::exists(exePath))
	{
		exePath = Application::GetWorkingDirectory() / "runtime" / "Runtime";
	}
	std::filesystem::path zipPath = m_ExportDirectory / "packed_assets.zip";

	std::filesystem::path executableName = m_ExportDirectory / m_GameName;

	std::ifstream exeFile(exePath, std::ios::binary);
	std::ifstream zipFile(zipPath, std::ios::binary);
	std::ofstream outFile(executableName, std::ios::binary);

	std::vector<char> exeData((std::istreambuf_iterator<char>(exeFile)), {});
	std::vector<char> zipData((std::istreambuf_iterator<char>(zipFile)), {});

	outFile.write(exeData.data(), exeData.size());
	outFile.write(zipData.data(), zipData.size());
	outFile.write(m_GameName.string().c_str(), m_GameName.string().size());
	outFile.write(m_Data.defaultScene.data(), m_Data.defaultScene.size());

	// Write the footer
	AssetBundleFooter footer;
	footer.zipSize = zipData.size();
	footer.gameTitleSize = m_GameName.string().size();
	footer.defaultSceneSize = m_Data.defaultScene.size();
	std::memcpy(footer.magic, BUNDLE_MAGIC, sizeof(footer.magic));

	outFile.write(reinterpret_cast<const char*>(&footer), sizeof(footer));

	exeFile.close();
	zipFile.close();

	std::filesystem::remove(zipPath);

	ENGINE_INFO("Exported game to {0}", executableName);
}
