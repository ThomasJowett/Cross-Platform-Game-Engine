#pragma once

#include "stdafx.h"

#include "Core/Window.h"

class WindowsWindow : public Window
{
public:
	WindowsWindow(const WindowProps& props);
	virtual ~WindowsWindow();

	void OnUpdate() override;

	inline unsigned int GetWidth() const override { return m_data.Width; }

private:
	virtual void Init(const WindowProps& props);
	virtual void Shutdown();
private:

	struct WindowData
	{
		std::string Title;
		unsigned int Width, Height;
		bool VSync;

		EventCallbackFn EventCallback;
	};

	WindowData m_data;
};

