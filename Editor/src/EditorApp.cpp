#include "EditorApp.h"

#include "ImGui/ImGuiDockSpace.h"
#include "ImGui/ImGuiConsole.h"

Editor::Editor(const WindowProps& props)
	:Application(props)
{
	RenderCommand::SetClearColour(Colours::GREY);

	ImGuiDockSpace* dockSpace = new ImGuiDockSpace();

	PushOverlay(dockSpace);
	PushOverlay(new ImGuiConsole(dockSpace->GetShowConsole()));

}

void Editor::OnUpdate()
{
	RenderCommand::Clear();
}