#include "Core/Application.h"
#include "Renderer/RenderCommand.h"
#include "Scene/SceneManager.h"
#include "Scene/AssetManager.h"

#include "RuntimeLayer.h"

struct AssetBundleFooter {
	uint64_t zipSize;
	uint64_t gameTitleSize;
	uint64_t defaultSceneSize;
	uint64_t externalBundle; // 0 = zip is embedded before this footer, 1 = zip is a separate ".pak" file next to the executable
	char magic[8];
};

int main(int argc, char* argv[])
{
	Ref<Application> app = CreateRef<Application>();

	if (app == nullptr)
	{
		ENGINE_CRITICAL("Failed to create application\r\n");
		return EXIT_FAILURE;
	}

	int rCode = app->Init(argc, argv);
	if (rCode != -1)
		return rCode;

	// A signed macOS .app can't have data appended to its executable the way a flat Windows/
	// Linux exe can - AssetPacker::ExportGameToAppBundle() instead ships a "game.meta" (the same
	// footer/title/scene layout, just in its own file) and "packed_assets.pak" in Contents/
	// Resources next to the executable in Contents/MacOS.
	std::filesystem::path metadataSource = argv[0];
	bool isAppBundle = false;
	std::filesystem::path resourcesDir;
#ifdef __APPLE__
	std::filesystem::path exeDir = std::filesystem::weakly_canonical(std::filesystem::path(argv[0])).parent_path();
	isAppBundle = exeDir.filename() == "MacOS";
	if (isAppBundle)
	{
		resourcesDir = exeDir.parent_path() / "Resources";
		metadataSource = resourcesDir / "game.meta";
	}
#endif

	std::ifstream exe(metadataSource, std::ios::binary | std::ios::ate);
	std::streamoff exeSize = exe.tellg();
	exe.seekg(exeSize - sizeof(AssetBundleFooter));

	AssetBundleFooter footer;
	exe.read(reinterpret_cast<char*>(&footer), sizeof(footer));

	if (std::memcmp(footer.magic, "GMBUNDLE", sizeof(footer.magic)) != 0)
	{
		ENGINE_ERROR("Invalid asset bundle");
		return EXIT_FAILURE;
	}

	exe.seekg(exeSize - sizeof(AssetBundleFooter) - footer.defaultSceneSize);
	std::string defaultScene(footer.defaultSceneSize, '\0');
	exe.read(&defaultScene[0], footer.defaultSceneSize);

	ENGINE_DEBUG("Default scene: {0}", defaultScene);

	exe.seekg(exeSize - sizeof(AssetBundleFooter) - footer.defaultSceneSize - footer.gameTitleSize);
	std::string gameTitle(footer.gameTitleSize, '\0');
	exe.read(&gameTitle[0], footer.gameTitleSize);

	if (footer.externalBundle)
	{
		std::filesystem::path bundlePath;
		if (isAppBundle)
			bundlePath = resourcesDir / "packed_assets.pak";
		else
		{
			bundlePath = std::filesystem::path(argv[0]);
			bundlePath.replace_extension(".pak");
		}

		if (!AssetManager::LoadBundleFromFile(bundlePath))
		{
			ENGINE_ERROR("Failed to load external asset bundle: {0}", bundlePath.string());
			return EXIT_FAILURE;
		}
	}
	else
	{
		std::streamoff zipOffset = exeSize - sizeof(AssetBundleFooter) - footer.defaultSceneSize - footer.gameTitleSize - footer.zipSize;

		exe.seekg(zipOffset);

		std::vector<uint8_t> zipData(footer.zipSize);
		exe.read(reinterpret_cast<char*>(zipData.data()), footer.zipSize);

		AssetManager::LoadBundle(zipData.data(), zipData.size());
	}

	Window* window = app->CreateDesktopWindow(WindowProps(gameTitle, 1920, 1080, 100, 100));

	if (!window)
		return EXIT_FAILURE;

	// ImGuiManager::End() always issues its own render pass straight to the swapchain with
	// LoadOp::Clear, which would wipe whatever RenderPipeline::Render already composited there -
	// shipped games have no ImGui UI to draw, so skip that pass entirely.
	Application::ShowImGui(false);

	RenderCommand::SetClearColour(Colours::GREY);

	Application::SetOpenDocument(argv[0]);

	SceneManager::ChangeScene(std::filesystem::path(defaultScene));

	app->GetLayerStack().PushLayer(CreateRef<RuntimeLayer>());

	app->Run();

	return EXIT_SUCCESS;
}
