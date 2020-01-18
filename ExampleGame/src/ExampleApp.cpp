#include "ExampleLayer2D.h"
#include "ExampleLayer3D.h"
#include "GridLayer.h"
#include "ExampleApp.h"

ExampleGame::ExampleGame()
{
	RenderCommand::SetClearColour(0.4f, 0.4f, 0.4f, 1.0f);

	//PushLayer(new GridLayer());
	PushLayer(new ExampleLayer3D());
	PushLayer(new ExampleLayer2D());
	PushOverlay(new ImGuiLayer());
}

void ExampleGame::OnUpdate()
{
	RenderCommand::Clear();
}
