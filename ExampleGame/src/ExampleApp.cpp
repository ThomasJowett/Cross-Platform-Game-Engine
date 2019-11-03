#include <include.h>

class ExampleGame : public Application
{
public:
	ExampleGame()
	{

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