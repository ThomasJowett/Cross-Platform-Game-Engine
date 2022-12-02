#include "stdafx.h"
#include "Application.h"

#include "GLFW/glfw3.h"

#include "Settings.h"

#include "Renderer/Renderer.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Font.h"

#include "Events/JoystickEvent.h"
#include "Events/SceneEvent.h"

#include "Scene/SceneManager.h"

#include "Logging/Logger.h"
#include "Core/Input.h"

#include "Utilities/StringUtils.h"
#include "Scripting/Lua/LuaManager.h"

Application* Application::s_Instance = nullptr;

std::filesystem::path Application::s_OpenDocument;
std::filesystem::path Application::s_OpenDocumentDirectory;
std::filesystem::path Application::s_WorkingDirectory;

Application::EventCallbackFn Application::s_EventCallback;

/* ------------------------------------------------------------------------------------------------------------------ */

Application::Application(const WindowProps& props)
{
	CORE_ASSERT(!s_Instance, "Application already exists! Cannot create multiple applications");
	s_Instance = this;

	Settings::Init();
	Random::Init();
	LuaManager::Init();
	Input::Init();
	Font::Init();

	SetDefaultSettings(props);

	RenderCommand::CreateRendererAPI();

	m_Window = Scope<Window>(Window::Create(props));
	m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

	Renderer::Init();

	s_EventCallback = BIND_EVENT_FN(Application::OnEvent);

	m_ImGuiManager = CreateScope<ImGuiManager>();
	m_ImGuiManager->Init();

	if (Settings::GetBool("Display", "Maximized"))
	{
		m_Window->MaximizeWindow();
		Renderer::OnWindowResize(m_Window->GetWidth(), m_Window->GetHeight());
	}
}

/* ------------------------------------------------------------------------------------------------------------------ */

Application::~Application()
{
	SceneManager::Shutdown();
	m_ImGuiManager->Shutdown();
	Settings::SaveSettings();
	Renderer::Shutdown();
	LuaManager::Shutdown();
	Font::Shutdown();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Application::AddLayer(Layer* layer)
{
	m_WaitingLayers.push_back(layer);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Application::AddOverlay(Layer* layer)
{
	m_WaitingOverlays.push_back(layer);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Application::RemoveLayer(Layer* layer)
{
	m_DeadLayers.push_back(layer);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Application::RemoveOverlay(Layer* layer)
{
	m_DeadOverlays.push_back(layer);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Application::Run()
{
	PROFILE_FUNCTION();

	double currentTime = GetTime();
	double accumulator = 0.0f;

	m_Running = true;

	while (m_Running)
	{
		PROFILE_SCOPE("Run Loop");

		double newTime = GetTime();
		double frameTime = newTime - currentTime;
		currentTime = newTime;

		accumulator += frameTime;

		// On Fixed update
		while (accumulator >= m_FixedUpdateInterval)
		{
			PROFILE_SCOPE("Layer Stack Fixed Update");

			if (!m_Minimized)
			{
				OnFixedUpdate();

				for (Layer* layer : m_LayerStack)
				{
					layer->OnFixedUpdate();
				}

				SceneManager::FixedUpdate();
			}
			accumulator -= m_FixedUpdateInterval;
		}

		m_Window->OnUpdate();

		// On Update
		{
			OnUpdate();

			PROFILE_SCOPE("Layer Stack Update");

			if (!m_Minimized)
			{
				for (Layer* layer : m_LayerStack)
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
			for (Layer* layer : m_LayerStack)
			{
				layer->OnImGuiRender();
			}
			m_ImGuiManager->End();
		}


		// Push any layers that were created during the update to the stack
		for (Layer* layer : m_WaitingLayers)
		{
			PushLayer(layer);
		}

		for (Layer* overlay : m_WaitingOverlays)
		{
			PushOverlay(overlay);
		}

		m_WaitingLayers.clear();
		m_WaitingOverlays.clear();

		// Remove any layers that were deleted during the update
		for (Layer* layer : m_DeadLayers)
		{
			PopLayer(layer);
		}

		for (Layer* layer : m_DeadOverlays)
		{
			PopOverlay(layer);
		}

		m_DeadLayers.clear();
		m_DeadOverlays.clear();

		Input::ClearInputData();
	}
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
	dispatcher.Dispatch<SceneChanged>([](SceneChanged& event) {
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

void Application::PushLayer(Layer* layer)
{
	PROFILE_FUNCTION();

	m_LayerStack.PushLayer(layer);
	layer->OnAttach();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Application::PushOverlay(Layer* layer)
{
	PROFILE_FUNCTION();

	m_LayerStack.PushOverlay(layer);
	layer->OnAttach();
}
/* ------------------------------------------------------------------------------------------------------------------ */

void Application::PopLayer(Layer* layer)
{
	PROFILE_FUNCTION();

	if (m_LayerStack.PopLayer(layer))
		layer->OnDetach();
	if (layer)
		delete layer;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Application::PopOverlay(Layer* layer)
{
	PROFILE_FUNCTION();

	if (m_LayerStack.PopOverlay(layer))
		layer->OnDetach();
	if (layer)
		delete layer;
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

	if (!Settings::GetBool("Display", "Maximized"))
	{
		Settings::SetInt("Display", "Window_Width", e.GetWidth());
		Settings::SetInt("Display", "Window_Height", e.GetHeight());
	}

	Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
	return false;
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool Application::OnWindowMove(WindowMoveEvent& e)
{
	if (!Settings::GetBool("Display", "Maximized"))
	{
		Settings::SetInt("Display", "Window_Position_X", e.GetTopLeftX());
		Settings::SetInt("Display", "Window_Position_Y", e.GetTopLeftY());
	}
	return false;
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool Application::OnMaximize(WindowMaximizedEvent& e)
{
	Settings::SetBool("Display", "Maximized", e.IsMaximized());
	return false;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Application::SetDefaultSettings(const WindowProps& props)
{
	const char* display = "Display";
	const char* audio = "Audio";

	Settings::SetDefaultInt(display, "Window_Width", props.width);
	Settings::SetDefaultInt(display, "Window_Height", props.height);
	Settings::SetDefaultInt(display, "Window_Position_X", props.posX);
	Settings::SetDefaultInt(display, "Window_Position_Y", props.posY);
	Settings::SetDefaultInt(display, "Window_Mode", (int)WindowMode::WINDOWED);
	Settings::SetDefaultBool(display, "Maximized", true);
	Settings::SetDefaultBool(display, "V-Sync", true);
}

/* ------------------------------------------------------------------------------------------------------------------ */

double Application::GetTime() const
{
#ifdef __WINDOWS__
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
#endif // __WINDOWS__

	return glfwGetTime();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Application::SetOpenDocument(const std::filesystem::path& filepath)
{
	if (std::filesystem::exists(filepath))
	{
		s_OpenDocument = filepath;

		std::string fileDirectory = filepath.string();

		fileDirectory = fileDirectory.substr(0, fileDirectory.find_last_of(std::filesystem::path::preferred_separator));

		s_OpenDocumentDirectory = fileDirectory;

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

		if (s_Instance)
		{
			AppOpenDocumentChange event;
			s_EventCallback(event);
		}
	}
}

/* ------------------------------------------------------------------------------------------------------------------ */

const std::filesystem::path& Application::GetOpenDocument()
{
	return s_OpenDocument;
}

/* ------------------------------------------------------------------------------------------------------------------ */

const std::filesystem::path& Application::GetOpenDocumentDirectory()
{
	return s_OpenDocumentDirectory;
}