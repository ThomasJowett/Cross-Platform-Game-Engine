#pragma once

#include "stdafx.h"

#include "Core/Window.h"
#include "Renderer/GraphicsContext.h"

#include "GLFW/glfw3.h"

class WindowsWindow : public Window
{
public:
	WindowsWindow(const WindowProps& props);
	virtual ~WindowsWindow();

	void OnUpdate() override;

	inline unsigned int GetWidth() const override { return m_data.Width; }
	inline unsigned int GetHeight() const override { return m_data.Height; }

	inline unsigned int GetPosX() const override { return m_data.PosX; }
	inline unsigned int GetPosY() const override { return m_data.PosY; }

	void SetEventCallback(const EventCallbackFn& callback) override { m_data.EventCallback = callback; }
	void SetVSync(bool enabled) override;
	bool IsVSync() const override;

	inline virtual void* GetNativeWindow() const override { return m_window; }

private:
	virtual void Init(const WindowProps& props);
	virtual void Shutdown();
private:

	struct WindowData
	{
		std::string Title;
		unsigned int Width, Height, PosX, PosY;
		bool VSync;

		EventCallbackFn EventCallback;
	};

	GLFWwindow* m_window;
	WindowData m_data;

	Scope<GraphicsContext> m_context;
};

