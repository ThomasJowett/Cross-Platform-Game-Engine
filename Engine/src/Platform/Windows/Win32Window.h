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

	inline unsigned int GetWidth() const override { return m_Data.width; }
	inline unsigned int GetHeight() const override { return m_Data.height; }

	inline unsigned int GetPosX() const override { return m_Data.posX; }
	inline unsigned int GetPosY() const override { return m_Data.posY; }

	void SetEventCallback(const EventCallbackFn& callback) override { m_Data.eventCallback = callback; }
	void SetVSync(bool enabled) override;
	bool IsVSync() const override;

	inline virtual std::any GetNativeWindow() const override { return m_Window; }
	inline virtual Ref<GraphicsContext> GetContext() const override { return m_Context; }

	virtual void SetIcon(const std::filesystem::path& path) override;
	virtual void SetCursor(Cursors cursorType) override;
	virtual void SetTitle(const char* title) override;

	virtual void DisableCursor() override;
	virtual void EnableCursor() override;

	virtual void SetCursorPosition(double xpos, double ypos) override;

	virtual void SetWindowMode(WindowMode mode, unsigned int width = 0, unsigned int height = 0) override;

	virtual void MaximizeWindow() override;
	virtual void RestoreWindow() override;
private:
	HRESULT Init(const WindowProps& props);
	void Shutdown();

	void MessageLoop();

	static LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
	HWND m_Window;
	struct WindowData
	{
		std::string title;
		unsigned int width, height, posX, posY;
		bool vSync;
		WindowMode mode;
		bool maximized;

		EventCallbackFn eventCallback;
	};

	HINSTANCE m_Instance;

	WindowData m_Data;

	Ref<GraphicsContext> m_Context;

	
};