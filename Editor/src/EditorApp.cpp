#include "EditorApp.h"

#include "MainDockSpace.h"
#include "ProjectsStartScreen.h"
#include "Renderer/RenderCommand.h"

Editor::Editor()
{
	Window* window = Application::CreateDesktopWindow(WindowProps("Editor", 1920, 1080, 100, 100));

	if (!window)
		return;
	RenderCommand::SetClearColour(Colours::GREY);

	if (Application::GetOpenDocument().empty())
	{
		m_LayerStack.PushOverlay(CreateRef<ProjectsStartScreen>());
		CLIENT_INFO("No project has been opened");
	}

	m_LayerStack.PushOverlay(CreateRef<MainDockSpace>());

	window->SetIcon(GetWorkingDirectory() / "data" / "Icons" / "Logo.png");
}
