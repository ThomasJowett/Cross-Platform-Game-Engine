#pragma once

#include <functional>
#include <any>
#include <filesystem>

#include <GLFW/glfw3.h>

#include "core.h"
#include "Events/Event.h"
#include "Renderer/GraphicsContext.h"
#include "Cursor.h"

enum class WindowMode
{
	WINDOWED = 0,
	FULL_SCREEN,
	BORDERLESS
};

struct WindowProps
{
	std::string title;
	unsigned int width;
	unsigned int height;

	unsigned int posX;
	unsigned int posY;

	WindowMode windowMode;

	bool maximized;

	WindowProps(const std::string& title = "Example Game",
		unsigned int width = 1920,
		unsigned int height = 1080,
		unsigned int posX = 20,
		unsigned int posY = 50,
		WindowMode windowMode = WindowMode::WINDOWED,
		bool maximized = false)
		:title(title), width(width), height(height),
		posX(posX), posY(posY), windowMode(windowMode)
	{
	}
};

// Desktop system window
class Window
{
public:
	using EventCallbackFn = std::function<void(Event&)>;

	Window(const WindowProps& props);
	~Window();

	bool Init(const WindowProps& props);

	void OnUpdate();

	unsigned int GetWidth() const { return m_Data.width; }
	unsigned int GetHeight() const { return m_Data.height; }

	unsigned int GetPosX() const { return m_Data.posX; }
	unsigned int GetPosY() const { return m_Data.posY; }

	// Window attributes
	void SetEventCallback(const EventCallbackFn& callback) { m_Data.eventCallback = callback; }
	void SetVSync(bool enabled);
	bool IsVSync() const;

	void SetIcon(const std::filesystem::path& path);
	void SetCursor(Cursors cursorType);
	void SetTitle(const char* title);
	const char* GetTitle();

	void DisableCursor();
	void EnableCursor();

	void SetCursorPosition(double xpos, double ypos);

	void SetWindowMode(WindowMode mode, unsigned int width = 0, unsigned int height = 0);

	void MaximizeWindow(); 
	void RestoreWindow();

	GLFWwindow* GetNativeWindow() const;
	Ref<GraphicsContext> GetContext() const;

private:
	void Shutdown();

private:
	struct WindowData
	{
		std::string title;
		unsigned int width, height, posX, posY;
		bool vSync;
		WindowMode mode;
		bool maximized;

		EventCallbackFn eventCallback;
	};

	GLFWwindow* m_Window;
	GLFWvidmode m_BaseVideoMode;
	WindowData m_Data;

	struct WindowModeParams {
		unsigned int width, Height;
		int xPos, yPos;
	};

	WindowModeParams m_OldWindowedParams;

	Ref<GraphicsContext> m_Context;

	GLFWcursor* m_SystemCursors[(int)Cursors::NumCursors] = {};
};

