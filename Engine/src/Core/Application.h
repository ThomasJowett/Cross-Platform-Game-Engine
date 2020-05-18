#pragma once

#include "LayerStack.h"
#include "Window.h"

#include "ImGui/ImGuiLayer.h"

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

	static void ShowImGui(bool showImgui) { Get().m_ImGuiLayer->SetIsUsing(showImgui); }
	static void ToggleImGui() { Get().m_ImGuiLayer->SetIsUsing(!Get().m_ImGuiLayer->IsUsing()); }

	void AddLayer(Layer* layer);
	void AddOverlay(Layer* layer);
	void RemoveLayer(Layer* layer);
	void RemoveOverlay(Layer* layer);

private:
	inline Window& GetWindowImpl() { return *m_Window; }
	void Run();
	bool OnWindowClose(WindowCloseEvent& e);
	bool OnWindowResize(WindowResizeEvent& e);
	bool OnWindowMove(WindowMoveEvent& e);

	void SetDefaultSettings(const WindowProps& props);

	double GetTime();
private:
	Scope<Window> m_Window;
	ImGuiLayer* m_ImGuiLayer;
	bool m_Running = true;
	bool m_Minimized = false;
	LayerStack m_LayerStack;

	std::vector<Layer*> m_WaitingLayers;
	std::vector<Layer*> m_WaitingOverlays;

	std::vector<Layer*> m_DeadLayers;
	std::vector<Layer*> m_DeadOverlays;

	static Application* s_Instance;
	friend int::main(int argc, char* argv[]);

protected:
	void PushLayer(Layer* layer);
	void PushOverlay(Layer* layer);

	void PopLayer(Layer* layer);
	void PopOverlay(Layer* layer);
};

// To be defined in CLIENT
Application* CreateApplication();