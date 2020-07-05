#include "EditorApp.h"

#include "ImGui/ImGuiDockSpace.h"
#include "ImGui/ImGuiConsole.h"
#include "ImGui/ImGuiJoystickInfo.h"
#include "ImGui/ImGuiContentExplorer.h"
#include "ImGui/ImGuiPreferences.h"
#include "ImGui/ImGuiViewport.h"

Editor::Editor(const WindowProps& props)
	:Application(props)
{
	RenderCommand::SetClearColour(Colours::GREY);

	ImGuiDockSpace* dockSpace = new ImGuiDockSpace();

	PushOverlay(dockSpace);
	PushOverlay(new ImGuiEditorPreferences(dockSpace->GetShowEditorPreferences()));
	PushOverlay(new ImGuiContentExplorer(dockSpace->GetShowContentExplorer()));
	PushOverlay(new ImGuiConsole(dockSpace->GetShowConsole()));
	PushOverlay(new ImGuiJoystickInfo(dockSpace->GetShowJoystickInfo()));
	PushOverlay(new ImGuiViewportPanel(dockSpace->GetShowViewport()));

	Application::GetWindow().SetIcon("resources/Icons/Logo.png");
}

void Editor::OnUpdate()
{
	RenderCommand::Clear();
}