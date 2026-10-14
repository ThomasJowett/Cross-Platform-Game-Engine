
#include "Core/Application.h"
#include "Renderer/RenderCommand.h"

#include "ProjectsStartScreen.h"
#include "MainDockSpace.h"
#include "FileSystem/AssetPacker.h"
#include "FileSystem/LuaDocGenerator.h"
#include "Core/InputParser.h"

int main(int argc, char* argv[])
{
	InputParser inputParser(argc, argv);

	bool hasExportGame = inputParser.CmdOptionExists("--export-game");
	bool hasGenerateDocs = inputParser.CmdOptionExists("--generate-docs");

	const std::string& exportGamePath = inputParser.GetCmdOption("--export-game");
	const std::string& generateDocsPath = inputParser.GetCmdOption("--generate-docs");

	Ref<Application> app = CreateRef<Application>();

	if (app == nullptr)
	{
		ENGINE_CRITICAL("Failed to create application\r\n");
		return EXIT_FAILURE;
	}

	int rCode = app->Init(inputParser);
	if (rCode != -1)
		return rCode;

	if (hasExportGame && !exportGamePath.empty())
	{
		bool show = true;
		AssetPacker packer(&show, Application::GetOpenDocumentDirectory(), std::filesystem::path(exportGamePath));
		packer.RunHeadlessExport();
		std::cout << "OK" << std::endl;
		return EXIT_SUCCESS;
	}

	if (!generateDocsPath.empty())
	{
		LuaDocGenerator::Generate(std::filesystem::path(generateDocsPath));
		std::cout << "OK" << std::endl;
		return EXIT_SUCCESS;
	}

	std::string windowTitle = std::string("Attercop Editor - ") + RendererAPI::GetAPIName();
	Window* window = app->CreateDesktopWindow(WindowProps(windowTitle, 1920, 1080, 100, 100));

	if (!window)
		return EXIT_FAILURE;

	RenderCommand::SetClearColour(Colours::GREY);

	if (Application::GetOpenDocument().empty())
	{
		app->GetLayerStack().PushOverlay(CreateRef<ProjectsStartScreen>());
		CLIENT_INFO("No project has been opened");
	}

	app->GetLayerStack().PushOverlay(CreateRef<MainDockSpace>());

	window->SetIcon(app->GetWorkingDirectory() / "data" / "Icons" / "Logo.png");

	app->Run();

	return EXIT_SUCCESS;
}
