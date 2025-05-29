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

	ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiCond_FirstUseEver);

	if (ImGui::BeginPopupModal("Pack Assets", &m_Show, ImGuiWindowFlags_NoResize))
	{
		if (m_CurrentStage == Stage::DiscoveringAssets) {
			ImGui::Text("Select assets to pack into the bundle:");
			ImGui::Separator();

			float footerHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
			ImVec2 contentRegion = ImGui::GetContentRegionAvail();

			if (ImGui::BeginChild("AssetTree", ImVec2(contentRegion.x, contentRegion.y - footerHeight), true))
			{
				for (auto asset : m_AssetTree->children)
					DrawAssetTree(asset);
			}
			ImGui::EndChild();

			if (ImGui::Button("Pack Selected Assets"))
			{
				m_PackThread = std::thread([this]() {
					m_CurrentStage = Stage::PackingAssets;
					m_Progress.store(0.0f);
					PackAssets();
					});
			}

			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				m_Show = false;
				ImGui::CloseCurrentPopup();
				Application::GetLayerStack().RemoveLayer(shared_from_this());
			}
		}
		else if (m_CurrentStage == Stage::PackingAssets) {
			ImGui::Text("Packing assets...");
			ImGui::ProgressBar(m_Progress.load(), ImVec2(0.0f, 0.0f));
		}
		else if (m_CurrentStage == Stage::ExportingGame) {
			ImGui::Text("Exporting game...");
			ImGui::ProgressBar(m_Progress.load(), ImVec2(0.0f, 0.0f));
		}
		else if (m_CurrentStage == Stage::Done) {
			ImGui::Text("Assets packed successfully!");
			if (ImGui::Button("Close"))
			{
				m_Show = false;
				ImGui::CloseCurrentPopup();
			}

			if(m_PackThread.joinable())
			{
				m_PackThread.join();
			}
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

	m_AssetTree = CreateRef<AssetNode>();

	for (const auto& asset : m_DiscoveredAssets) {
		AddAssetToTree(asset, m_AssetTree);
	}
}

void AssetPacker::AddAssetToTree(const std::filesystem::path& assetPath, Ref<AssetNode> root)
{
	PROFILE_FUNCTION();
	auto current = root;
	std::filesystem::path relativePath = std::filesystem::relative(assetPath, m_ProjectDirectory);
	for (const auto& part : relativePath.parent_path()) {
		auto it = std::find_if(current->children.begin(), current->children.end(),
			[&](const auto& node) { return node->name == part.string() && node->isDirectory; });

		if (it == current->children.end()) {
			auto folder = std::make_shared<AssetNode>();
			folder->name = part.string();
			folder->isDirectory = true;
			current->children.push_back(folder);
			current = folder;
		}
		else {
			current = *it;
		}
	}

	// Add actual file
	auto file = CreateRef<AssetNode>();
	file->name = assetPath.filename().string();
	file->fullPath = assetPath;
	file->isDirectory = false;
	current->children.push_back(file);
}

void AssetPacker::PackAssets()
{
	PROFILE_FUNCTION();

	m_CurrentStage = Stage::PackingAssets;
	
	mz_zip_archive zip = {};
	memset(&zip, 0, sizeof(zip));

	std::filesystem::path outputZipPath = m_ExportDirectory / "packed_assets.zip";

	if (!mz_zip_writer_init_file(&zip, outputZipPath.string().c_str(), 0)) {
		ENGINE_ERROR("Failed to initialize zip writer: {0}", mz_zip_get_error_string(mz_zip_get_last_error(&zip)));
		return;
	}

	std::vector<std::filesystem::path> filesToPack;
	filesToPack.insert(filesToPack.end(), m_SelectedAssets.begin(), m_SelectedAssets.end());

	auto collectFilesFromDir = [&](const std::filesystem::path& dir) {
		if (std::filesystem::exists(dir) && std::filesystem::is_directory(dir))
		{
			for (const auto& entry : std::filesystem::directory_iterator(dir))
			{
				filesToPack.push_back(entry.path());
			}
		}
	};

	std::filesystem::path dataPath = Application::GetWorkingDirectory();
	collectFilesFromDir(dataPath / "data" / "Shaders");
	collectFilesFromDir(dataPath / "data" / "Fonts");

	const size_t totalFiles = filesToPack.size();
	size_t filesPacked = 0;

	for (const auto& asset : filesToPack)
	{
		std::string relativePath;
		if(asset.string().compare(0, m_ProjectDirectory.string().length(), m_ProjectDirectory.string()) == 0)
			relativePath = std::filesystem::relative(asset, m_ProjectDirectory).string();
		else
			relativePath = std::filesystem::relative(asset, dataPath).string();
		std::replace(relativePath.begin(), relativePath.end(), '\\', '/'); // Ensure forward slashes for zip
		if (!mz_zip_writer_add_file(&zip, relativePath.c_str(), asset.string().c_str(), nullptr, 0, MZ_BEST_SPEED))
		{
			ENGINE_ERROR("Failed to add file to zip: {0}", mz_zip_get_error_string(mz_zip_get_last_error(&zip)));
			mz_zip_writer_end(&zip);
			return;
		}

		++filesPacked;
		m_Progress.store(static_cast<float>(filesPacked) / totalFiles);
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

	m_CurrentStage = Stage::ExportingGame;

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
	m_CurrentStage = Stage::Done;

	ENGINE_INFO("Exported game to {0}", executableName);
	Application::GetLayerStack().RemoveLayer(shared_from_this());
}

void AssetPacker::DrawAssetTree(Ref<AssetNode> node)
{
	PROFILE_FUNCTION();
	if (node->isDirectory)
	{
		bool allSelected = true;
		bool anySelected = false;
		for (const auto& child : node->children) {
			if (!child->isDirectory) {
				bool selected = m_SelectedAssets.find(child->fullPath) != m_SelectedAssets.end();
				anySelected |= selected;
				allSelected &= selected;
			}
		}
		bool folderSelected = allSelected;
		ImGui::PushID(node->fullPath.string().c_str());
		if (ImGui::Checkbox(("##checkbox_" + node->name).c_str(), &folderSelected))
		{
			std::function<void(const Ref<AssetNode>&)> toggleSelection = [&](const Ref<AssetNode>& childNode) {
				if (childNode->isDirectory) {
					for (const auto& child : childNode->children) {
						toggleSelection(child);
					}
				}
				else {
					if (folderSelected)
						m_SelectedAssets.insert(childNode->fullPath);
					else
						m_SelectedAssets.erase(childNode->fullPath);
				}
				};
			toggleSelection(node);
		}
		ImGui::SameLine();
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
		bool opened = ImGui::TreeNodeEx(node->name.c_str(), flags);
		
		if (opened)
		{
			for (const auto& child : node->children)
			{
				DrawAssetTree(child);
			}
			ImGui::TreePop();
		}
		ImGui::PopID();
	}
	else
	{
		bool selected = m_SelectedAssets.find(node->fullPath) != m_SelectedAssets.end();
		if (ImGui::Checkbox(node->name.c_str(), &selected))
		{
			if (selected)
				m_SelectedAssets.insert(node->fullPath);
			else
				m_SelectedAssets.erase(node->fullPath);
		}
	}
}
