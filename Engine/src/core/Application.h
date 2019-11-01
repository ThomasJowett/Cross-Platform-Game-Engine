#pragma once

#include "core.h"

namespace Jupiter
{
	class JUPITER_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}