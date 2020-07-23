#include "EditorApp.h"

#include "ImGui/ImGuiDockSpace.h"

Editor::Editor(const WindowProps& props)
	:Application(props)
{
	RenderCommand::SetClearColour(Colours::GREY);
	PushOverlay(new ImGuiDockSpace());

	Application::GetWindow().SetIcon(GetWorkingDirectory().string() + "\\resources\\Icons\\Logo.png");

	CLIENT_INFO("Opening project: {0}", Application::GetOpenDocument().string());
}

void Editor::OnUpdate()
{
}