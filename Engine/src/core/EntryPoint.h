#pragma once

#ifdef JPT_PLATFORM_WINDOWS

extern Jupiter::Application* Jupiter::CreateApplication();

#include "Debug.h"

int main(int argc, char* argv[])
{
	OUTPUT("Jupiter Engine Initialised");
	auto app = Jupiter::CreateApplication();
	app->Run();
	delete app;
	return 0;
}
#endif // JPt_PLATFORM_WINDOWS
