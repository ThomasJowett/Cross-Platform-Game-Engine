#include "EditorApp.h"

#include "ImGui/ImGuiDockSpace.h"
#include "ImGui/ImGuiConsole.h"
#include "ImGui/ImGuiJoystickInfo.h"
#include "ImGui/ImGuiContentExplorer.h"

Editor::Editor(const WindowProps& props)
	:Application(props)
{
	RenderCommand::SetClearColour(Colours::GREY);

	ImGuiDockSpace* dockSpace = new ImGuiDockSpace();

	PushOverlay(dockSpace);
	PushOverlay(new ImGuiContentExplorer(dockSpace->GetShowContentExplorer()));
	PushOverlay(new ImGuiConsole(dockSpace->GetShowConsole()));
	PushOverlay(new ImGuiJoystickInfo(dockSpace->GetShowJoystickInfo()));

}

void Editor::OnUpdate()
{
	RenderCommand::Clear();
}