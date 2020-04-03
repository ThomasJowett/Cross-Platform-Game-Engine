#include "stdafx.h"
#include "Application.h"
#include "InputParser.h"
#include "Core/Version.h"

#ifndef DEBUG
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif // !DEBUG

#ifdef __WINDOWS__

extern Application* CreateApplication();

bool AnotherInstance();

int main(int argc, char* argv[])
{
	if (AnotherInstance())
		return 1;

	Logger::Init();

	PROFILE_BEGIN_SESSION("Startup", "Profile-Startup.json");

	InputParser input(argc, argv);

	if (input.CmdOptionExists("-h") || input.CmdOptionExists("--help"))
	{
		std::cout << "usage:"
			<< " [--help] "
			<< " [--version] "
			<< std::endl;
		return EXIT_SUCCESS;
	}

	if (input.CmdOptionExists("--version") || input.CmdOptionExists("-v"))
	{
		std::cout << VERSION << std::endl;
		return EXIT_SUCCESS;
	}

	if (!input.HasFoundAllArguments())
	{
		std::string file = input.OpenFile();
		if (!file.empty())
		{
			ENGINE_INFO("Opening file: {0}", file);
		}

		ENGINE_ERROR("Not a valid input parameter");
		return EXIT_FAILURE;
	}

	ENGINE_INFO("Version: {0}", VERSION);

	Application* app = CreateApplication();
	
	CORE_ASSERT(app != nullptr, "Failed to create application\r\n");
	ENGINE_INFO("Engine Initialised");
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
		ENGINE_ERROR("Application already open");
		return true;
	}
	else
	{
		//TODO: create the server
	}
	return false;
}
#endif // __WINDOWS__