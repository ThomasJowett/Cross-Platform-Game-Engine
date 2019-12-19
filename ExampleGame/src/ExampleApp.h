#pragma once
#include <include.h>
#include "Core/EntryPoint.h"

class ExampleGame : public Application
{
public:
	ExampleGame();


	~ExampleGame() = default;
};

/* Entry Point*/
Application* CreateApplication()
{
	return new ExampleGame();
}