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

	inline unsigned int GetWidth() const override { return m_Data.Width; }
	inline unsigned int GetHeight() const override { return m_Data.Height; }

	inline unsigned int GetPosX() const override { return m_Data.PosX; }
	inline unsigned int GetPosY() const override { return m_Data.PosY; }

	void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
	void SetVSync(bool enabled) override;
	bool IsVSync() const override;

	void SetIcon(const std::string& path) override;

	virtual void SetWindowMode(const WindowMode& mode, unsigned int width = 0, unsigned int height = 0);

	inline virtual std::any GetNativeWindow() const override { return m_Window; }

private:
	virtual void Init(const WindowProps& props);
	virtual void Shutdown();
private:

	struct WindowData
	{
		std::string Title;
		unsigned int Width, Height, PosX, PosY;
		bool VSync;
		WindowMode Mode;

		EventCallbackFn EventCallback;
	};

	GLFWwindow* m_Window;
	GLFWvidmode m_BaseVideoMode;
	WindowData m_Data;

	struct WindowModeParams {
		unsigned int width, Height;
		int XPos, YPos;
	};

	WindowModeParams m_OldWindowedParams;

	Scope<GraphicsContext> m_context;
};

