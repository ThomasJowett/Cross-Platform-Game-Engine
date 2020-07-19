#pragma once
#include <include.h>
#include "Core/EntryPoint.h"

class ExampleGame : public Application
{
public:
	explicit ExampleGame(const WindowProps& props);
	~ExampleGame() = default;

	virtual void OnUpdate() override;
};

/* Entry Point*/
Application* CreateApplication()
{
	return new ExampleGame(WindowProps("Example Game", 1920, 1080, 100, 100));
}