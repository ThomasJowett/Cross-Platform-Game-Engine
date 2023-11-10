
#include "Core/Application.h"
#include "Renderer/RenderCommand.h"

#include "ProjectsStartScreen.h"
#include "MainDockSpace.h"

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

	DesktopWindow* window = app->CreateDesktopWindow(WindowProps("Editor", 1920, 1080, 100, 100));

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
