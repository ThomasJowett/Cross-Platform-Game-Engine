#include "Application.h"

#include "GLFW/glfw3.h"

#include "Logging/Instrumentor.h"
#include "Settings.h"
#include "InputParser.h"
#include "Version.h"

#include "Renderer/Renderer.h"
#include "Renderer/RenderCommand.h"
#include "Asset/Font.h"

#include "Events/JoystickEvent.h"
#include "Events/SceneEvent.h"

#include "Scene/SceneManager.h"

#include "Logging/Logger.h"
#include "Core/Input.h"

#include "Utilities/StringUtils.h"
#include "Scripting/Lua/LuaManager.h"

#ifndef DEBUG
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif // !DEBUG

#if defined(_WIN32)
#include <crtdbg.h>
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

Application* Application::s_Instance = nullptr;

Application::EventCallbackFn Application::s_EventCallback;

/* ------------------------------------------------------------------------------------------------------------------ */

Application::Application()
{
#if defined(_WIN32)
	// Enable memory-leak reports
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG));
#endif

	PROFILE_FUNCTION();
	CORE_ASSERT(!s_Instance, "Application already exists! Cannot create multiple applications");
	s_Instance = this;

	s_EventCallback = BIND_EVENT_FN(Application::OnEvent);
}

/* ------------------------------------------------------------------------------------------------------------------ */

Application::~Application()
{
	PROFILE_FUNCTION();
	m_LayerStack.PushPop();
	SceneManager::Shutdown();
	Settings::SaveSettings();
	if (m_Window) {
		if (m_ImGuiManager) m_ImGuiManager->Shutdown();
		Renderer::Shutdown();
		Font::Shutdown();
	}
	AssetManager::Shutdown();
	LuaManager::Shutdown();
}

int Application::Init(int argc, char* argv[])
{
	m_WorkingDirectory = std::filesystem::weakly_canonical(std::filesystem::path(argv[0])).parent_path();
#ifdef __APPLE__
	if (m_WorkingDirectory.filename() == "MacOS")
	{
		m_WorkingDirectory = m_WorkingDirectory.parent_path() / "Resources";
	}
#endif
	std::filesystem::current_path(m_WorkingDirectory);
	Logger::Init();

	InputParser input(argc, argv);

	if (input.CmdOptionExists("-h") || input.CmdOptionExists("--help"))
	{
		std::cout << "usage:"
			<< " [--help] "
			<< " [--version] "
			<< " [--profile] "
			<< " [--auto-play] "
			<< " [--exit-after <seconds>] "
			<< " [--scene <path>] "
			<< std::endl;
		return EXIT_SUCCESS;
	}

	if (input.CmdOptionExists("-v") || input.CmdOptionExists("--version"))
	{
		std::cout << VERSION_MAJOR << '.' << VERSION_MINOR << '.' << VERSION_PATCH << std::endl;
		return EXIT_SUCCESS;
	}

	if (input.CmdOptionExists("-p") || input.CmdOptionExists("--profile"))
	{
	}

	// Scripted/headless testing flags - see the matching getters in Application.h.
	m_AutoPlay = input.CmdOptionExists("--auto-play");

	if (input.CmdOptionExists("--exit-after"))
	{
		const std::string& value = input.GetCmdOption("--exit-after");
		if (!value.empty())
			m_ExitAfterSeconds = std::stod(value);
	}

	if (input.CmdOptionExists("--scene"))
	{
		const std::string& value = input.GetCmdOption("--scene");
		if (!value.empty())
			m_SceneOverride = value;
	}

	Settings::Init();
	SetDefaultSettings();

	std::string file;
	bool hasFile = input.File(file);
	if (hasFile)
		SetOpenDocument(file);

	if (!input.HasFoundAllArguments() && !hasFile)
	{
		std::cerr << "Not a valid input parameter" << std::endl;
		return EXIT_FAILURE;
	}

	Random::Init();
	LuaManager::Init();

	if (RenderCommand::CreateRendererAPI() != 0)
		return EXIT_FAILURE;

	ENGINE_INFO("Engine Version: {0}.{1}.{2}", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

	ENGINE_INFO("Engine Initialised");
	return -1;
}

/* ------------------------------------------------------------------------------------------------------------------ */

Window* Application::CreateDesktopWindowImpl(const WindowProps& props)
{
	const char* windowStr = props.title.c_str();
	Settings::SetDefaultInt(windowStr, "Window_Width", props.width);
	Settings::SetDefaultInt(windowStr, "Window_Height", props.height);
	Settings::SetDefaultInt(windowStr, "Window_Position_X", props.posX);
	Settings::SetDefaultInt(windowStr, "Window_Position_Y", props.posY);
	Settings::SetDefaultInt(windowStr, "Window_Mode", (int)props.windowMode);
	Settings::SetDefaultBool(windowStr, "Window_Maximized", props.maximized);

	m_Window = CreateScope<Window>(props);
	if (m_Window) {
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		if (Settings::GetBool(windowStr, "Window_Maximized"))
		{
			m_Window->MaximizeWindow();
			Renderer::OnWindowResize(m_Window->GetWidth(), m_Window->GetHeight());
		}

		Renderer::Init();
		Font::Init();
		Input::Init(m_Window->GetNativeWindow());

		if (!m_ImGuiManager) {
			m_ImGuiManager = CreateScope<ImGuiManager>();
			m_ImGuiManager->Init();
		}
		return m_Window.get();
	}
	else
		ENGINE_ERROR("Could not create window");
	return nullptr;
}

void Application::Tick() {
	PROFILE_FUNCTION();
	PROFILE_FRAME();

	double newTime = GetTime();
	double frameTime = newTime - m_CurrentTime;
	m_CurrentTime = newTime;

	// --exit-after: let this frame finish normally, just don't schedule another one.
	if (m_ExitDeadline >= 0.0 && m_CurrentTime >= m_ExitDeadline)
		Close();

	m_Accumulator += frameTime;

	m_DeltaTime = (float)frameTime;

	// On Fixed update
	while (m_Accumulator >= m_FixedUpdateInterval)
	{
		PROFILE_SCOPE("Layer Stack Fixed Update");

		if (!m_Minimized)
		{
			for (Ref<Layer> layer : m_LayerStack)
			{
				layer->OnFixedUpdate();
			}

			SceneManager::FixedUpdate();
		}

		m_Accumulator -= m_FixedUpdateInterval;
	}

	m_Window->GetContext()->MakeCurrent();
	m_Window->OnUpdate();

	// On Update 
	{
		PROFILE_SCOPE("Layer Stack Update");

		if (!m_Minimized)
		{
			for (Ref<Layer> layer : m_LayerStack)
			{
				layer->OnUpdate((float)frameTime);
			}
		}

		SceneManager::Update((float)frameTime);
	}

	// Render the imgui of each of the layers
	if (m_ImGuiManager->IsUsing())
	{
		m_ImGuiManager->Begin();
		{
			PROFILE_SCOPE("Layer Stack ImGui Render");
			for (Ref<Layer> layer : m_LayerStack)
			{
				layer->OnImGuiRender();
			}
		}
		m_ImGuiManager->End();
	}

	m_LayerStack.PushPop();

	Input::ClearInputData();
}

void Application::Run()
{
	PROFILE_FUNCTION();

	if (IsRunning()) {
		ENGINE_ERROR("Application is already running");
	}

	m_CurrentTime = GetTime();
	m_Accumulator = 0.0;

	if (m_ExitAfterSeconds >= 0.0)
		m_ExitDeadline = m_CurrentTime + m_ExitAfterSeconds;

	m_Running = true;
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg([](void* arg) { static_cast<Application*>(arg)->Tick(); }, this, 0, 1);
#else
	while (m_Running)
	{
		Tick();
	}
#endif
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Application::OnEvent(Event& e)
{
	PROFILE_FUNCTION();

	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
	if (!m_Running)
		return;
	dispatcher.Dispatch<WindowMaximizedEvent>(BIND_EVENT_FN(Application::OnMaximize));
	dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));
	dispatcher.Dispatch<WindowMoveEvent>(BIND_EVENT_FN(Application::OnWindowMove));

	dispatcher.Dispatch<JoystickConnected>([](JoystickConnected& event) {
		ENGINE_INFO(event.to_string());
		return false;
		});
	dispatcher.Dispatch<JoystickDisconnected>([](JoystickDisconnected& event) {
		ENGINE_INFO(event.to_string());
		return false;
		});
	dispatcher.Dispatch<SceneChangedEvent>([](SceneChangedEvent& event) {
		ENGINE_INFO(event.to_string());
		return false;
		});

	m_ImGuiManager->OnEvent(e);

	for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
	{
		if (e.Handled)
			break;
		(*it)->OnEvent(e);
	}
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool Application::OnWindowClose(WindowCloseEvent& e)
{
	Close();
	return true;
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool Application::OnWindowResize(WindowResizeEvent& e)
{
	PROFILE_FUNCTION();

	if (e.GetWidth() == 0 || e.GetHeight() == 0)
	{
		m_Minimized = true;
		return false;
	}
	m_Minimized = false;

	if (!Settings::GetBool(m_Window->GetTitle(), "Window_Maximized"))
	{
		Settings::SetInt(m_Window->GetTitle(), "Window_Width", e.GetWidth());
		Settings::SetInt(m_Window->GetTitle(), "Window_Height", e.GetHeight());
	}

	Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
	return false;
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool Application::OnWindowMove(WindowMoveEvent& e)
{
	if (!Settings::GetBool(m_Window->GetTitle(), "Window_Maximized"))
	{
		Settings::SetInt(m_Window->GetTitle(), "Window_Position_X", e.GetTopLeftX());
		Settings::SetInt(m_Window->GetTitle(), "Window_Position_Y", e.GetTopLeftY());
	}
	return false;
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool Application::OnMaximize(WindowMaximizedEvent& e)
{
	Settings::SetBool(m_Window->GetTitle(), "Window_Maximized", e.IsMaximized());
	return false;
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool Application::SetOpenDocumentImpl(const std::filesystem::path& filepath)
{
	SceneManager::ChangeSceneState(SceneState::Edit);

	if (std::filesystem::exists(filepath))
	{
		m_OpenDocument = filepath;

		std::string fileDirectory = filepath.string();

		fileDirectory = fileDirectory.substr(0, fileDirectory.find_last_of(std::filesystem::path::preferred_separator));

		m_OpenDocumentDirectory = fileDirectory;

		if (filepath.extension() != ".proj")
			return true;

		std::string recentFiles = Settings::GetValue("Files", "Recent_Files");

		std::vector<std::string> recentFilesList = SplitString(recentFiles, ',');

		std::string fileStr = filepath.string();

		bool containsFile = false;

		for (std::string file : recentFilesList)
		{
			if (file == fileStr)
			{
				containsFile = true;
				break;
			}
		}

		if (!containsFile)
		{
			recentFiles.append(filepath.string() + ',');
			Settings::SetValue("Files", "Recent_Files", recentFiles.c_str());
		}

		AssetManager::CleanUp();
		AssetManager::Init(m_OpenDocumentDirectory);

		if (s_Instance)
		{
			AppOpenDocumentChangedEvent event;
			s_EventCallback(event);
		}

		return true;
	}
	return false;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Application::SetDefaultSettings()
{
	const char* display = "Display";
	const char* audio = "Audio";

	Settings::SetDefaultBool(display, "V-Sync", true);

	Settings::SetDefaultDouble(audio, "Master", 100.0);

	Settings::SetDefaultValue("Files", "Recent_Files", "");
}

/* ------------------------------------------------------------------------------------------------------------------ */

double Application::GetTime() const
{
#ifdef _WINDOWS
	static LARGE_INTEGER s_frequency;
	//check to see if the application can read the frequency
	static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
	if (s_use_qpc)
	{
		//most accurate method of getting system time
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		return (double)((now.QuadPart * 1000) / s_frequency.QuadPart) / 1000.0;
	}
	else
	{
		//same value but only updates 64 times a second
		return (double)GetTickCount64();
	}
#endif // _WINDOWS

	return glfwGetTime();
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool Application::SetOpenDocument(const std::filesystem::path& filepath)
{
	return s_Instance->SetOpenDocumentImpl(filepath);
}

/* ------------------------------------------------------------------------------------------------------------------ */

const std::filesystem::path& Application::GetOpenDocument()
{
	return s_Instance->m_OpenDocument;
}

/* ------------------------------------------------------------------------------------------------------------------ */

const std::filesystem::path& Application::GetOpenDocumentDirectory()
{
	return s_Instance->m_OpenDocumentDirectory;
}

const std::filesystem::path& Application::GetWorkingDirectory()
{
	return s_Instance->m_WorkingDirectory;
}
