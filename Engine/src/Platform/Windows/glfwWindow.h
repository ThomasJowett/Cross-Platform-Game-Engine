#pragma once

#include "Core/Window.h"
#include "Renderer/GraphicsContext.h"

#include "GLFW/glfw3.h"

class glfwWindow : public Window
{
public:
	glfwWindow(const WindowProps& props);
	virtual ~glfwWindow();

	void OnUpdate() override;

	inline unsigned int GetWidth() const override { return m_Data.Width; }
	inline unsigned int GetHeight() const override { return m_Data.Height; }

	inline unsigned int GetPosX() const override { return m_Data.PosX; }
	inline unsigned int GetPosY() const override { return m_Data.PosY; }

	void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
	void SetVSync(bool enabled) override;
	bool IsVSync() const override;

	void SetIcon(const std::filesystem::path& path) override;
	void SetCursor(Cursors cursorType) override;

	virtual void SetWindowMode(WindowMode mode, unsigned int width = 0, unsigned int height = 0);

	virtual void MaximizeWindow() override;
	virtual void RestoreWindow() override;

	inline virtual std::any GetNativeWindow() const override { return m_Window; }
	inline virtual Ref<GraphicsContext> GetContext() const override { return m_Context; }

	virtual void DisableCursor() override;
	virtual void EnableCursor() override;
	virtual void SetCursorPosition(double xpos, double ypos) override;

private:
	void Init(const WindowProps& props);
	void Shutdown();
private:

	struct WindowData
	{
		std::string Title;
		unsigned int Width, Height, PosX, PosY;
		bool VSync;
		WindowMode Mode;
		bool Maximized;

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

	Ref<GraphicsContext> m_Context;

	GLFWcursor* m_SystemCursors[(int)Cursors::NumCursors] = {};
};

