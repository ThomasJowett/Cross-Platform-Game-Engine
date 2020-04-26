#pragma once

#include "Core/Window.h"
#include "Renderer/GraphicsContext.h"
#include <Windows.h>

static uint8_t s_Win32WindowCount = 0;

class Win32Window : public Window
{
public:
	Win32Window(const WindowProps& props);
	virtual ~Win32Window();

	void OnUpdate() override;

	inline unsigned int GetWidth() const override { return m_Data.Width; }
	inline unsigned int GetHeight() const override { return m_Data.Height; }

	inline unsigned int GetPosX() const override { return m_Data.PosX; }
	inline unsigned int GetPosY() const override { return m_Data.PosY; }

	void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
	void SetVSync(bool enabled) override;
	bool IsVSync() const override;

	inline virtual std::any GetNativeWindow() const override { return m_Window; }
	inline virtual Ref<GraphicsContext> GetContext() const override { return m_Context; }

	virtual void SetIcon(const std::string& path) override;

	virtual void SetWindowMode(const WindowMode& mode, unsigned int width = 0, unsigned int height = 0) override;
private:
	virtual HRESULT Init(const WindowProps& props);
	virtual void Shutdown();

	void MessageLoop();

	static LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
	HWND m_Window;
	struct WindowData
	{
		std::string Title;
		unsigned int Width, Height, PosX, PosY;
		bool VSync;
		WindowMode Mode;

		EventCallbackFn EventCallback;
	};

	HINSTANCE m_Instance;

	WindowData m_Data;

	Ref<GraphicsContext> m_Context;

	
};