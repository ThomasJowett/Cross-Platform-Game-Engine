#include "Core/Application.h"
#include "Renderer/RenderCommand.h"
#include "Scene/SceneManager.h"

#include "RuntimeLayer.h"

int main(int argc, char* argv[])
{
	Ref<Application> app = CreateRef<Application>();

	if (app == nullptr)
	{
		ENGINE_CRITICAL("Failed to create application\r\n");
		return EXIT_FAILURE;
	}

	int rCode = app->ParseArgs(argc, argv);
	if (rCode != -1)
		return rCode;

	app->Init();

	Application::SetOpenDocument(Application::GetWorkingDirectory() / "Startup");

	std::ifstream file;
	file.open(Application::GetWorkingDirectory() / "Startup", std::ios::in | std::ios::binary);
	if (!file.good()) {
		ENGINE_ERROR("Could not open startup file");
		return EXIT_FAILURE;
	}
	size_t size;
	std::string gameName;
	std::string startupScene;
	file.read((char*)&size, sizeof(size));
	gameName.resize(size);
	file.read((char*)&gameName[0], size);
	file.read((char*)&size, sizeof(size));
	startupScene.resize(size);
	file.read((char*)&startupScene[0], size);
	file.close();

	Window* window = app->CreateDesktopWindow(WindowProps(gameName, 1920, 1080, 100, 100));

	if (!window)
		return EXIT_FAILURE;

	RenderCommand::SetClearColour(Colours::GREY);

	SceneManager::ChangeScene(std::filesystem::path(startupScene));

	app->GetLayerStack().PushLayer(CreateRef<RuntimeLayer>());

	app->Run();

	return EXIT_SUCCESS;
}
