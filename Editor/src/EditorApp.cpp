#include "EditorApp.h"

#include "MainDockSpace.h"
#include "ProjectsStartScreen.h"
#include "Renderer/RenderCommand.h"

Editor::Editor(const WindowProps& props)
	:Application(props)
{
	RenderCommand::SetClearColour(Colours::GREY);

	if (Application::GetOpenDocument().empty())
	{
		m_LayerStack.PushOverlay(CreateRef<ProjectsStartScreen>());
		CLIENT_INFO("No project has been opened");
	}

	m_LayerStack.PushOverlay(CreateRef<MainDockSpace>());

	Application::GetWindow().SetIcon(GetWorkingDirectory() / "resources" / "Icons" / "Logo.png");
}
