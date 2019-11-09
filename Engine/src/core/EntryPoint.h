#pragma once

#ifdef __WINDOWS__

extern Application* CreateApplication();

#include "Logging/Debug.h"

int main(int argc, char* argv[])
{
	auto app = CreateApplication();
	CORE_ASSERT(app != nullptr, "Failed to create application\r\n");
	OUTPUT("Engine Initialised\r\n");
	app->Run();
	delete app;
	return 0;
}
#endif // __WINDOWS__
