#pragma once
#include <include.h>
#include "Core/EntryPoint.h"

class ExampleGame : public Application
{
public:
	ExampleGame();
	~ExampleGame() = default;

	virtual void OnUpdate() override;
};

/* Entry Point*/
Application* CreateApplication()
{
	return new ExampleGame();
}