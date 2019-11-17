#pragma once

#include "stdafx.h"

#include "core.h"
#include "Events/Event.h"

struct WindowProps
{
	std::string Title;
	unsigned int Width;
	unsigned int Height;

	unsigned int PosX;
	unsigned int PosY;

	WindowProps(const std::string& title = "Example Game",
		unsigned int width = 1920,
		unsigned int height = 1080,
		unsigned int posX = 20,
		unsigned int posY = 50)
		:Title(title), Width(width), Height(height),
		PosX(posX), PosY(posY)
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

	virtual void* GetNativeWindow() const = 0;
	
	static Window* Create(const WindowProps& props = WindowProps());
};

