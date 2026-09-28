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
	Application();
	Application(const Application&) = delete;
	virtual ~Application();

	// Gets the static instance of the application
	static inline Application& Get() { return *s_Instance; }

	static Window* CreateDesktopWindow(const WindowProps& props) { return Get().CreateDesktopWindowImpl(props); }

	// Get the applications Window object
	static Window* GetWindow() { return Get().GetWindowImpl(); }

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
	static const std::filesystem::path& GetWorkingDirectory();

	// Calls an event
	static void CallEvent(Event& event) { s_EventCallback(event); }

	static float GetDeltaTime() { return Get().m_DeltaTime; }

	// True if launched with --auto-play - starts directly in Play state instead of Edit, for
	// scripted/headless testing that needs the scene running without a manual Play click.
	static bool ShouldAutoPlay() { return Get().m_AutoPlay; }

	// Scene path to load instead of the project's own default scene, from --scene <path>.
	// Empty if not set.
	static const std::string& GetSceneOverride() { return Get().m_SceneOverride; }

	static void SetGameViewportOverride(double mouseX, double mouseY, uint32_t width, uint32_t height)
	{
		Get().m_HasGameViewportOverride = true;
		Get().m_GameViewportMouseX = mouseX;
		Get().m_GameViewportMouseY = mouseY;
		Get().m_GameViewportWidth = width;
		Get().m_GameViewportHeight = height;
	}
	static void ClearGameViewportOverride() { Get().m_HasGameViewportOverride = false; }
	static bool HasGameViewportOverride() { return Get().m_HasGameViewportOverride; }
	static std::pair<double, double> GetGameViewportMousePos() { return { Get().m_GameViewportMouseX, Get().m_GameViewportMouseY }; }
	static uint32_t GetGameViewportWidth() { return Get().m_HasGameViewportOverride ? Get().m_GameViewportWidth : Get().GetWindowImpl()->GetWidth(); }
	static uint32_t GetGameViewportHeight() { return Get().m_HasGameViewportOverride ? Get().m_GameViewportHeight : Get().GetWindowImpl()->GetHeight(); }

private:
	inline Window* GetWindowImpl() { return m_Window.get(); }
	Window* CreateDesktopWindowImpl(const WindowProps& props);
	void Tick();
	void Run();
	int Init(int argc, char* argv[]);
	void OnEvent(Event& e);
	bool OnWindowClose(WindowCloseEvent& e);
	bool OnWindowResize(WindowResizeEvent& e);
	bool OnWindowMove(WindowMoveEvent& e);
	bool OnMaximize(WindowMaximizedEvent& e);

	bool SetOpenDocumentImpl(const std::filesystem::path& filepath);
	void SetDefaultSettings();

	double GetTime() const;

protected:
	LayerStack m_LayerStack;
private:
	Scope<Window> m_Window;
	Scope<ImGuiManager> m_ImGuiManager;
	bool m_Running = false;
	bool m_Minimized = false;
	float m_FixedUpdateInterval = 0.01f;

	static Application* s_Instance;
	friend int ::main(int argc, char* argv[]);

	std::filesystem::path m_OpenDocument;
	std::filesystem::path m_OpenDocumentDirectory;
	std::filesystem::path m_WorkingDirectory;

	static EventCallbackFn s_EventCallback;

	float m_DeltaTime = 0.0f;
	double m_CurrentTime = 0.0;
	double m_Accumulator = 0.0;

	bool m_AutoPlay = false;
	std::string m_SceneOverride;
	// Duration in seconds from --exit-after; negative means disabled. m_ExitDeadline is the
	// absolute GetTime() value computed from it once Run() starts.
	double m_ExitAfterSeconds = -1.0;
	double m_ExitDeadline = -1.0;
	bool m_HasGameViewportOverride = false;
	double m_GameViewportMouseX = 0.0;
	double m_GameViewportMouseY = 0.0;
	uint32_t m_GameViewportWidth = 0;
	uint32_t m_GameViewportHeight = 0;
};

// To be defined in CLIENT
Ref<Application> CreateApplication();