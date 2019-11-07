#pragma once

#ifdef __WINDOWS__

extern Application* CreateApplication();

#include "Logging/Debug.h"

int main(int argc, char* argv[])
{
	OUTPUT("Engine Initialised\r\n");
	auto app = CreateApplication();
	app->Run();
	delete app;
	return 0;
}
#endif // __WINDOWS__
