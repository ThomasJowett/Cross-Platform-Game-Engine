#include "ExampleLayer2D.h"
#include "ExampleLayer3D.h"
#include "ExampleApp.h"

ExampleGame::ExampleGame()
{
	PushLayer(new ExampleLayer3D());
	PushOverlay(new ImGuiLayer());
}
