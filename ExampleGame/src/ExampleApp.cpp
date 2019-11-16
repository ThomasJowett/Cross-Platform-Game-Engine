#include <include.h>

#include "imgui/imgui.h"

class ExampleLayer :public Layer
{
public:
	ExampleLayer()
		:Layer("Example")
	{
	}

	void OnUpdate() override
	{
		if (Input::IsKeyPressed(KEY_TAB))
		{
			std::cout << "Tab is pressed" << std::endl;
		}
	}

	void OnEvent(Event& e) override
	{
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Hello World");
		ImGui::End();
	}
};

class ExampleGame : public Application
{
public:
	ExampleGame()
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new ImGuiLayer());
	}

	~ExampleGame()
	{

	}
};

/* Entry Point*/
Application* CreateApplication()
{
	return new ExampleGame();
}