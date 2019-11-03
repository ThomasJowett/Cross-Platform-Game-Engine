#pragma once

#include "core.h"


class DLLIMPEXP_CORE Application
{
public:
	Application();
	virtual ~Application();

	void Run();
};

// To be defined in CLIENT
Application* CreateApplication();