#include <Jupiter.h>

class ExampleGame : public Jupiter::Application
{
public:
	ExampleGame()
	{

	}

	~ExampleGame()
	{

	}
};

Jupiter::Application* Jupiter::CreateApplication()
{
	return new ExampleGame();
}