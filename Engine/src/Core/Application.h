#pragma once

#include <functional>
#include <filesystem>

#include "LayerStack.h"
#include "Window.h"

#include "ImGui/ImGuiManager.h"

int main(int argc, char* argv[]);

class Application
{
	using EventCallbackFn = std::function<void(Event&)>;

public:
	Application(const WindowProps& props);
	Application(const Application&) = delete;
	virtual ~Application();

	// Gets the static instance of the application
	static inline Application& Get() { return *s_Instance; }

	// Get the applications Window object
	static Window& GetWindow() { return Get().GetWindowImpl(); }

	// Set whether to show Dear ImGui
	static void ShowImGui(bool showImgui) { Get().m_ImGuiManager->SetIsUsing(showImgui); }

	// Toggle whether Dear ImGui to shown
	static void ToggleImGui() { Get().m_ImGuiManager->SetIsUsing(!Get().m_ImGuiManager->IsUsing()); }

	static LayerStack& GetLayerStack() { return Get().m_LayerStack; }

	// Stops the application from running and closes the window
	void Close() { m_Running = false; }

	// Is the application currently in the run function
	bool IsRunning() { return m_Running; }

	// fixed update delta time. Default of 10ms seconds (100 times a second)
	float GetFixedUpdateInterval() { return m_FixedUpdateInterval; }

	// Sets the main document that the application has open, returns if successful
	static bool SetOpenDocument(const std::filesystem::path& filepath);

	// Gets the document that the application has open
	static const std::filesystem::path& GetOpenDocument();

	// Get the Open Document Directory object
	static const std::filesystem::path& GetOpenDocumentDirectory();

	// Get the directory that the application was launched from
	static const std::filesystem::path& GetWorkingDirectory() { return s_WorkingDirectory; }

	// Calls an event
	static void CallEvent(Event& event) { s_EventCallback(event); }

	LayerStack m_LayerStack;
private:
	inline Window& GetWindowImpl() { return *m_Window; }
	void Run();
	void OnEvent(Event& e);
	bool OnWindowClose(WindowCloseEvent& e);
	bool OnWindowResize(WindowResizeEvent& e);
	bool OnWindowMove(WindowMoveEvent& e);
	bool OnMaximize(WindowMaximizedEvent& e);

	void SetDefaultSettings(const WindowProps& props);

	double GetTime() const;

private:
	Scope<Window> m_Window;
	Scope<ImGuiManager> m_ImGuiManager;
	bool m_Running = false;
	bool m_Minimized = false;
	float m_FixedUpdateInterval = 0.01f;

	static Application* s_Instance;
	friend int ::main(int argc, char* argv[]);

	static std::filesystem::path s_OpenDocument;
	static std::filesystem::path s_OpenDocumentDirectory;
	static std::filesystem::path s_WorkingDirectory;

	static EventCallbackFn s_EventCallback;
};

// To be defined in CLIENT
Ref<Application> CreateApplication();