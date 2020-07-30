#include "EditorApp.h"

#include "ImGui/ImGuiDockSpace.h"
#include "ImGui/ImGuiProjectsStartScreen.h"

Editor::Editor(const WindowProps& props)
	:Application(props)
{
	RenderCommand::SetClearColour(Colours::GREY);

	if (!Application::GetOpenDocument().empty())
	{
		CLIENT_INFO("Opening project: {0}", Application::GetOpenDocument().string());
	}
	else
	{
		PushOverlay(new ImGuiProjectsStartScreen());
		CLIENT_INFO("No project has been opened");
	}

	PushOverlay(new ImGuiDockSpace());

	Application::GetWindow().SetIcon(GetWorkingDirectory().string() + "\\resources\\Icons\\Logo.png");
}

void Editor::OnUpdate()
{
}