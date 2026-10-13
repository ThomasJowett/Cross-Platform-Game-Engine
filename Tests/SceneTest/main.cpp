#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

#include "Core/Application.h"
#include "Logging/Logger.h"
#include "Core/InputParser.h"
#include "Core/InputActionSystem.h"
#include "Core/InputMappings.h"
#include "Physics/CollisionLayers.h"
#include "Scene/SceneManager.h"
#include "Renderer/RendererAPI.h"

#include "SceneTestLayer.h"

// Runs one scene headless with a fixed time step and reports the result as the exit code, see SceneTestResult
int main(int argc, char* argv[])
{
	InputParser input(argc, argv);

	if (input.CmdOptionExists("-h") || input.CmdOptionExists("--help"))
	{
		std::cout << "usage: SceneTest --project <file.proj> --scene <file.scene> [--frames N] [--timeout S]"
			" [--log <path>] [--expect-errors N] [--require-verdict] [--display]" << std::endl;
		return EXIT_SUCCESS;
	}

	auto option = [&input](const std::string& name)
		{
			return input.CmdOptionExists(name) ? input.GetCmdOption(name) : std::string();
		};

	std::filesystem::path project = option("--project");
	SceneTestOptions options;
	options.scene = option("--scene");
	std::string log = option("--log");
	std::string frames = option("--frames");
	std::string timeout = option("--timeout");
	std::string expectErrors = option("--expect-errors");
	options.requireVerdict = input.CmdOptionExists("--require-verdict");
	options.display = input.CmdOptionExists("--display");

	if (project.empty() || options.scene.empty() || !input.HasFoundAllArguments())
	{
		std::cerr << "SceneTest: --project and --scene are required, see --help" << std::endl;
		return EXIT_FAILURE;
	}

	// Resolved before Application::Init changes the working directory
	project = std::filesystem::absolute(project);
	if (std::filesystem::exists(options.scene))
		options.scene = std::filesystem::absolute(options.scene);
	if (!frames.empty())
		options.maxFrames = std::stoull(frames);
	if (!expectErrors.empty())
		options.expectedErrors = std::stoi(expectErrors);
	double timeoutSeconds = timeout.empty() ? 60.0 : std::stod(timeout);

	std::vector<std::string> appArgs = { argv[0] };
	if (!options.display)
	{
		appArgs.push_back("--headless");
		appArgs.push_back("--fixed-step");
	}
	if (!log.empty())
	{
		appArgs.push_back("--log");
		appArgs.push_back(log);
	}
	std::vector<char*> appArgv;
	for (std::string& arg : appArgs)
		appArgv.push_back(arg.data());

	Ref<Application> app = CreateRef<Application>();
	int rCode = app->Init((int)appArgv.size(), appArgv.data());
	if (rCode != -1)
		return rCode;

	if (options.display){
		std::string windowTitle = options.scene.filename().string() + std::string(" - ") + RendererAPI::GetAPIName();
		Window* window = app->CreateDesktopWindow(WindowProps(windowTitle, 1920, 1080, 100, 100));
	}

	if (!Application::SetOpenDocument(project))
	{
		CLIENT_ERROR("SceneTest: could not open project {0}", project.string());
		return EXIT_FAILURE;
	}

	std::filesystem::path projectDirectory = Application::GetOpenDocumentDirectory();
	InputActionSystem::LoadMappings(projectDirectory / InputMappings::FilePath);
	CollisionLayers::LoadNames(projectDirectory / CollisionLayers::FilePath);

	Ref<SceneTestLayer> layer = CreateRef<SceneTestLayer>(options);
	SceneTestLayer::BindTestTable(layer.get());
	app->GetLayerStack().AddLayer(layer);

	// A safety net for hangs outside Lua, which the script watchdog can't stop
	std::mutex mutex;
	std::condition_variable finished;
	bool done = false;
	std::thread timeoutThread([&]()
		{
			std::unique_lock<std::mutex> lock(mutex);
			if (!finished.wait_for(lock, std::chrono::duration<double>(timeoutSeconds), [&]() { return done; }))
			{
				CLIENT_ERROR("RESULT: FAIL timed out after {0} seconds", timeoutSeconds);
				Logger::GetClientLogger()->flush();
				std::_Exit((int)SceneTestResult::TimedOut);
			}
		});

	app->Run();

	{
		std::lock_guard<std::mutex> lock(mutex);
		done = true;
	}
	finished.notify_one();
	timeoutThread.join();

	int result = (int)layer->GetResult();
	Logger::GetClientLogger()->flush();
	return result;
}
