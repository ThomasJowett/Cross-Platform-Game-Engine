#pragma once
#include <include.h>
#include "Core/EntryPoint.h"

class ExampleGame : public Application
{
public:
	ExampleGame(const WindowProps& props);
	~ExampleGame() = default;

	virtual void OnUpdate() override;
};

/* Entry Point*/
Application* CreateApplication()
{
	return new ExampleGame(WindowProps("Example Game", 426, 240, 100, 100));
}