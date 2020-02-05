#include "ExampleLayer2D.h"
#include "ExampleLayer3D.h"
#include "GridLayer.h"
#include "ExampleApp.h"

ExampleGame::ExampleGame()
{
	RenderCommand::SetClearColour(Colour::GREY);

	//PushLayer(new GridLayer());
	PushLayer(new ExampleLayer3D());
	PushLayer(new ExampleLayer2D());
	PushOverlay(new ImGuiLayer());

	Application::Get().GetWindow().SetIcon("resources/Circle.png");

	Application::Get().GetWindow().SetWindowMode(WindowMode::WINDOWED, 1920, 1080);
}

void ExampleGame::OnUpdate()
{
	RenderCommand::Clear();
}
