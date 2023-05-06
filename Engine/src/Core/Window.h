#pragma once

#include <functional>
#include <any>
#include <filesystem>

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

	virtual ~Window() = default;

	virtual void OnUpdate() = 0;

	virtual unsigned int GetWidth() const = 0;
	virtual unsigned int GetHeight() const = 0;

	virtual unsigned int GetPosX() const = 0;
	virtual unsigned int GetPosY() const = 0;

	// Window attributes
	virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
	virtual void SetVSync(bool enabled) = 0;
	virtual bool IsVSync() const = 0;

	virtual void SetIcon(const std::filesystem::path& path) = 0;
	virtual void SetCursor(Cursors cursorType) = 0;
	virtual void SetTitle(const char* title) = 0;
	virtual const char* GetTitle() = 0;

	virtual void DisableCursor() = 0;
	virtual void EnableCursor() = 0;

	virtual void SetCursorPosition(double xpos, double ypos) = 0;

	virtual void SetWindowMode(WindowMode mode, unsigned int width = 0, unsigned int height = 0) = 0;

	virtual void MaximizeWindow() = 0; 
	virtual void RestoreWindow() = 0;

	virtual std::any GetNativeWindow() const = 0;
	virtual Ref<GraphicsContext> GetContext() const = 0;
	
	static Scope<Window> Create(const WindowProps& props = WindowProps());
};

