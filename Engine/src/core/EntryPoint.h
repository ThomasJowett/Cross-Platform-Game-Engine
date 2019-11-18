#pragma once

#ifdef __WINDOWS__

extern Application* CreateApplication();

bool AnotherInstance();

#include "Logging/Debug.h"

int main(int argc, char* argv[])
{
	if (AnotherInstance())
		return 1;

	Application* app = CreateApplication();
	CORE_ASSERT(app != nullptr, "Failed to create application\r\n");
	OUTPUT("Engine Initialised\r\n");
	app->Run();
	delete app;
	return 0;
}

//Checks if another instance of the game is already running
bool AnotherInstance()
{
	HANDLE ourMutex;
	ourMutex = CreateMutex(NULL, true, L"Use_a_different_string_here_for_each_program_48161-XYZZY");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		//TODO: use second instance as a client to the first server instance
		DBG_OUTPUT("Error: Application Already Open\n");
		return true;
	}
	else
	{
		//TODO: create the server
	}
	return false;
}

#endif // __WINDOWS__