#include <Jupiter.h>

std::string test;

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