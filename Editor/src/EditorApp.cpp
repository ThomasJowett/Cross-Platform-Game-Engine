#include "EditorApp.h"

#include "ImGui/ImGuiDockSpace.h"
#include "ImGui/ImGuiConsole.h"
#include "ImGui/ImGuiJoystickInfo.h"
#include "ImGui/ImGuiContentExplorer.h"
#include "ImGui/ImGuiPreferences.h"
#include "ImGui/ImGuiViewport.h"

#include "TestLayer.h"

Editor::Editor(const WindowProps& props)
	:Application(props)
{
	RenderCommand::SetClearColour(Colours::GREY);

	ImGuiDockSpace* dockSpace = new ImGuiDockSpace();

	Ref<FrameBuffer> framebuffer = FrameBuffer::Create({ 1920, 1080 });

	PushLayer(new TestLayer(framebuffer));

	PushOverlay(dockSpace);
	PushOverlay(new ImGuiEditorPreferences(dockSpace->GetShowEditorPreferences()));
	PushOverlay(new ImGuiContentExplorer(dockSpace->GetShowContentExplorer()));
	PushOverlay(new ImGuiConsole(dockSpace->GetShowConsole()));
	PushOverlay(new ImGuiJoystickInfo(dockSpace->GetShowJoystickInfo()));
	PushOverlay(new ImGuiViewportPanel(dockSpace->GetShowViewport(), framebuffer));

	Application::GetWindow().SetIcon("resources/Icons/Logo.png");
}

void Editor::OnUpdate()
{
	RenderCommand::Clear();
}