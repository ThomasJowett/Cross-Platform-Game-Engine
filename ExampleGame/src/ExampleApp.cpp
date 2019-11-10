#include <include.h>

class ExampleLayer :public Layer
{
public:
	ExampleLayer()
		:Layer("Example")
	{
	}

	void OnUpdate() override
	{
	}

	void OnEvent(Event& e) override
	{
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