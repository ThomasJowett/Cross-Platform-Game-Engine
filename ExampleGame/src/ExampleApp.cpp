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
		std::cout << "ExampleLayer::Upsdate\n";
	}

	void OnEvent(Event& e) override
	{
		//std::cout << e <<std::endl;
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