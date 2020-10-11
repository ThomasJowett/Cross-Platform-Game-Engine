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

	/// <summary>
	/// Called when an event triggers
	/// </summary>
	/// <param name="e"></param>
	void OnEvent(Event& e);
	/// <summary>
	/// Called once per frame
	/// </summary>
	virtual void OnUpdate() {};
	/// <summary>
	/// Called 100 times a second
	/// </summary>
	virtual void OnFixedUpdate() {};

	/// <summary>
	/// Gets the static instance of the application
	/// </summary>
	/// <returns>The instance of the application</returns>
	static inline Application& Get() { return *s_Instance; }
	/// <summary>
	/// Gets the applications window
	/// </summary>
	/// <returns></returns>
	static Window& GetWindow() { return Get().GetWindowImpl(); }

	static void ShowImGui(bool showImgui) { Get().m_ImGuiManager->SetIsUsing(showImgui); }
	static void ToggleImGui() { Get().m_ImGuiManager->SetIsUsing(!Get().m_ImGuiManager->IsUsing()); }

	/// <summary>
	/// Add a layer to the layer stack
	/// </summary>
	/// <param name="layer"></param>
	void AddLayer(Layer* layer);
	/// <summary>
	/// Add an overlay to the  layer stack
	/// </summary>
	/// <param name="layer"></param>
	void AddOverlay(Layer* layer);
	/// <summary>
	/// Removes a layer from the layer stack
	/// </summary>
	/// <param name="layer"></param>
	void RemoveLayer(Layer* layer);
	/// <summary>
	/// Removes an overlay from the layer stack
	/// </summary>
	/// <param name="layer"></param>
	void RemoveOverlay(Layer* layer);

	/// <summary>
	/// Stops the application from running and closes the window
	/// </summary>
	void Close() { m_Running = false; }

	/// <summary>
	/// Sets the main document that the application has open
	/// </summary>
	/// <param name="filepath"></param>
	static void SetOpenDocument(const std::filesystem::path& filepath);
	/// <summary>
	/// Gets the document that the application has open
	/// </summary>
	/// <returns></returns>
	static const std::filesystem::path& GetOpenDocument();
	/// <summary>
	/// Gets the directory of the open document
	/// </summary>
	/// <returns></returns>
	static const std::filesystem::path& GetOpenDocumentDirectory();

	/// <summary>
	/// Get the directory that the application was launched from
	/// </summary>
	/// <returns></returns>
	static const std::filesystem::path& GetWorkingDirectory() { return s_WorkingDirectory; }

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
	friend int::main(int argc, char* argv[]);

	static std::filesystem::path s_OpenDocument;
	static std::filesystem::path s_OpenDocumentDirectory;
	static std::filesystem::path s_WorkingDirectory;

	static EventCallbackFn s_EventCallback;

protected:
	/// <summary>
	/// Immediately pushes a new layer to the layer stack, Should not be called during a frame
	/// </summary>
	/// <param name="layer"></param>
	void PushLayer(Layer* layer);
	/// <summary>
	/// Immediately pushes a new overlay to the layer stack, Should not be called during a frame
	/// </summary>
	/// <param name="layer"></param>
	void PushOverlay(Layer* layer);
	/// <summary>
	/// Immediately removes a layer from the layer stack, Should not be called during a frame
	/// </summary>
	/// <param name="layer"></param>
	void PopLayer(Layer* layer);
	/// <summary>
	/// Immediately removes an overlay from the layer stack, Should not be called during a frame
	/// </summary>
	/// <param name="layer"></param>
	void PopOverlay(Layer* layer);
};

// To be defined in CLIENT
Application* CreateApplication();