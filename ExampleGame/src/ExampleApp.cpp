#include "ExampleLayer2D.h"
#include "ExampleLayer3D.h"
#include "MonteCarloLayer.h"
#include "GridLayer.h"
#include "ExampleApp.h"

ExampleGame::ExampleGame(const WindowProps& props)
	:Application(props)
{
	// Sets the clear colour of the screen, by default is black
	RenderCommand::SetClearColour(Colours::GREY);

	//PushLayer(new GridLayer());
	PushLayer(new ExampleLayer3D());
	//PushLayer(new ExampleLayer2D());
	//PushLayer(new MonteCarloLayer());
	

	//Application::GetWindow().SetIcon("resources/Circle.png");

	//Application::GetWindow().SetWindowMode(WindowMode::WINDOWED, 1920, 1080);

	// Example usage of the logger
	//CLIENT_CRITICAL("[Example Critical] The game is about to crash!");
	//CLIENT_ERROR("[Example Error] Something went wrong!");
	//CLIENT_WARN("[Example Warning] Something may have gone wrong...");
	//CLIENT_INFO("[Example Information] Normal stuff happened");
	//CLIENT_DEBUG("[Example Debug] Diagnostic information");
	//CLIENT_TRACE("[Example Trace] Very fine detailed diagnostic information");
}

void ExampleGame::OnUpdate()
{
	RenderCommand::Clear();
}
