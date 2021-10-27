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

	/**
	 * Called when an event triggers
	 * @param e event
	 */
	void OnEvent(Event& e);

	/**
	 * Called once per frame
	 */
	virtual void OnUpdate() {};

	/**
	 * Called 100 times a second
	 */
	virtual void OnFixedUpdate() {};

	/**
	 * Gets the static instance of the application
	 * @return Application& The instance of the application
	 */
	static inline Application& Get() { return *s_Instance; }

	/**
	 * Get the applications Window object
	 *
	 * @return Window&
	 */
	static Window& GetWindow() { return Get().GetWindowImpl(); }

	/**
	 * Set wether to show Dear ImGui
	 * @param showImgui
	 */
	static void ShowImGui(bool showImgui) { Get().m_ImGuiManager->SetIsUsing(showImgui); }

	/**
	 * Toggle wether Dear ImGui to shown
	 */
	static void ToggleImGui() { Get().m_ImGuiManager->SetIsUsing(!Get().m_ImGuiManager->IsUsing()); }

	/**
	 * Add a layer to the layer stack
	 * @param layer
	 */
	void AddLayer(Layer* layer);

	/**
	 * Add an overlay to the  layer stack
	 * @param layer
	 */
	void AddOverlay(Layer* layer);

	/**
	 * Removes a layer from the layer stack
	 * @param layer
	 */
	void RemoveLayer(Layer* layer);

	/**
	 * Removes an overlay from the layer stack
	 * @param layer
	 */
	void RemoveOverlay(Layer* layer);

	/**
	 * Stops the application from running and closes the window
	 */
	void Close() { m_Running = false; }

	/**
	 * Is the application currently in the run function
	 * @return true
	 * @return false
	 */
	bool IsRunning() { return m_Running; }

	/**
	 * Sets the main document that the application has open
	 * @param filepath
	 */
	static void SetOpenDocument(const std::filesystem::path& filepath);

	/**
	 * Gets the document that the application has open
	 * @return const std::filesystem::path&
	 */
	static const std::filesystem::path& GetOpenDocument();

	/**
	 * Get the Open Document Directory object
	 * @return const std::filesystem::path&
	 */
	static const std::filesystem::path& GetOpenDocumentDirectory();

	/**
	 * Get the directory that the application was launched from
	 * @return const std::filesystem::path&
	 */
	static const std::filesystem::path& GetWorkingDirectory() { return s_WorkingDirectory; }

	/**
	 * Calls an event
	 * @param event
	 */
	static void CallEvent(Event& event) { s_EventCallback(event); }

private:
	inline Window& GetWindowImpl() { return *m_Window; }
	void Run();
	bool OnWindowClose(WindowCloseEvent& e);
	bool OnWindowResize(WindowResizeEvent& e);
	bool OnWindowMove(WindowMoveEvent& e);
	bool OnMaximize(WindowMaximizedEvent& e);

	void SetDefaultSettings(const WindowProps& props);

	double GetTime();

private:
	Scope<Window> m_Window;
	ImGuiManager* m_ImGuiManager;
	bool m_Running = true;
	bool m_Minimized = false;
	LayerStack m_LayerStack;

	std::vector<Layer*> m_WaitingLayers;
	std::vector<Layer*> m_WaitingOverlays;

	std::vector<Layer*> m_DeadLayers;
	std::vector<Layer*> m_DeadOverlays;

	static Application* s_Instance;
	friend int ::main(int argc, char* argv[]);

	static std::filesystem::path s_OpenDocument;
	static std::filesystem::path s_OpenDocumentDirectory;
	static std::filesystem::path s_WorkingDirectory;

	static EventCallbackFn s_EventCallback;

protected:
	/**
	 * Immediately pushes a new layer to the layer stack, Should not be called during a frame
	 * @param layer
	 */
	void PushLayer(Layer* layer);

	/**
	 * Immediately pushes a new overlay to the layer stack, Should not be called during a frame
	 * @param layer
	 */
	void PushOverlay(Layer* layer);

	/**
	 * Immediately removes a layer from the layer stack, Should not be called during a frame
	 * @param layer
	 */
	void PopLayer(Layer* layer);

	/**
	 * Immediately removes an overlay from the layer stack, Should not be called during a frame
	 * @param layer
	 */
	void PopOverlay(Layer* layer);
};

// To be defined in CLIENT
Application* CreateApplication();