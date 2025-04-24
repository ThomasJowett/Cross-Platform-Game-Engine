#include "Core/Application.h"
#include "Renderer/RenderCommand.h"
#include "Scene/SceneManager.h"

#include "RuntimeLayer.h"

#include <miniz.h>

struct AssetBundleFooter {
	uint64_t zipSize;
	uint64_t gameTitleSize;
	uint64_t defaultSceneSize;
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

	std::ifstream exe(argv[0], std::ios::binary | std::ios::ate);
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

	std::streamoff zipOffset = exeSize - sizeof(AssetBundleFooter) - footer.defaultSceneSize - footer.gameTitleSize - footer.zipSize;

	exe.seekg(zipOffset);

	std::vector<uint8_t> zipData(footer.zipSize);
	exe.read(reinterpret_cast<char*>(zipData.data()), footer.zipSize);

	mz_zip_archive zip = {};
	if (!mz_zip_reader_init_mem(&zip, zipData.data(), zipData.size(), 0))
	{
		ENGINE_ERROR("Failed to initialize zip reader: {0}", mz_zip_get_error_string(mz_zip_get_last_error(&zip)));
		return EXIT_FAILURE;
	}

	int numFiles = (int)mz_zip_reader_get_num_files(&zip);
	ENGINE_DEBUG("Zip contains {0} files", numFiles);

	for (int i = 0; i < numFiles; i++)
	{
		mz_zip_archive_file_stat file_stat;
		if (mz_zip_reader_file_stat(&zip, i, &file_stat))
		{
			ENGINE_DEBUG("Extracting: {0}", file_stat.m_filename);

			// TODO: load this into the asset library
		}
	}

	mz_zip_reader_end(&zip);

	//Window* window = app->CreateDesktopWindow(WindowProps(gameTitle, 1920, 1080, 100, 100));

	//if (!window)
	//	return EXIT_FAILURE;

	//RenderCommand::SetClearColour(Colours::GREY);

	//SceneManager::ChangeScene(std::filesystem::path(defaultScene));

	//app->GetLayerStack().PushLayer(CreateRef<RuntimeLayer>());

	//app->Run();

	return EXIT_SUCCESS;
}
