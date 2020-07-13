#include "EditorApp.h"

#include "ImGui/ImGuiDockSpace.h"

Editor::Editor(const WindowProps& props)
	:Application(props)
{
	RenderCommand::SetClearColour(Colours::GREY);
	PushOverlay(new ImGuiDockSpace());

	Application::GetWindow().SetIcon("resources/Icons/Logo.png");
}

void Editor::OnUpdate()
{
}