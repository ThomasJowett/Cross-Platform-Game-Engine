#pragma once
#ifndef DEBUG
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif // !DEBUG

#ifdef __WINDOWS__

extern Application* CreateApplication();

bool AnotherInstance();

#include "Logging/Debug.h"

int main(int argc, char* argv[])
{
	if (AnotherInstance())
		return 1;

	PROFILE_BEGIN_SESSION("Startup", "Profile-Startup.json");

	Application* app = CreateApplication();

	CORE_ASSERT(app != nullptr, "Failed to create application\r\n");
	OUTPUT("Engine Initialised\r\n");
	PROFILE_END_SESSION("Startup");

	PROFILE_BEGIN_SESSION("Runtime", "Profile-Runtime.json");
	app->Run();
	PROFILE_END_SESSION("Runtime");

	PROFILE_BEGIN_SESSION("Shutdown", "Profile-Shutdown.json");
	delete app;
	PROFILE_END_SESSION("Shutdown");
	return EXIT_SUCCESS;
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