#pragma once

#include "LayerStack.h"
#include "Window.h"

#include "ImGui/ImGuiManager.h"

int main(int argc, char* argv[]);

class Application
{
public:
	Application(const WindowProps& props);
	Application(const Application&) = delete;
	virtual ~Application();

	void OnEvent(Event& e);
	virtual void OnUpdate() = 0;

	static inline Application& Get() { return *s_Instance; }
	static Window& GetWindow() { return Get().GetWindowImpl(); }

	static void ShowImGui(bool showImgui) { Get().m_ImGuiManager->SetIsUsing(showImgui); }
	static void ToggleImGui() { Get().m_ImGuiManager->SetIsUsing(!Get().m_ImGuiManager->IsUsing()); }

	void AddLayer(Layer* layer);
	void AddOverlay(Layer* layer);
	void RemoveLayer(Layer* layer);
	void RemoveOverlay(Layer* layer);

	void Close() { m_Running = false; }

	static void SetOpenDocument(const std::string& filepath);
	static const std::filesystem::path& GetOpenDocument();

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
	static std::filesystem::path s_WorkingDirectory;

protected:
	void PushLayer(Layer* layer);
	void PushOverlay(Layer* layer);

	void PopLayer(Layer* layer);
	void PopOverlay(Layer* layer);
};

// To be defined in CLIENT
Application* CreateApplication();