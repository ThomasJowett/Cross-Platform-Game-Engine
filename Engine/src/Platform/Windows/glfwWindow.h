#pragma once

#include "Core/Window.h"
#include "Renderer/GraphicsContext.h"

#include "GLFW/glfw3.h"

class glfwWindow : public Window
{
public:
	glfwWindow();
	virtual ~glfwWindow();
    
    bool Init(const WindowProps& props);

	void OnUpdate() override;

	inline unsigned int GetWidth() const override { return m_Data.width; }
	inline unsigned int GetHeight() const override { return m_Data.height; }

	inline unsigned int GetPosX() const override { return m_Data.posX; }
	inline unsigned int GetPosY() const override { return m_Data.posY; }

	void SetEventCallback(const EventCallbackFn& callback) override { m_Data.eventCallback = callback; }
	void SetVSync(bool enabled) override;
	bool IsVSync() const override;

	void SetIcon(const std::filesystem::path& path) override;
	void SetCursor(Cursors cursorType) override;
	void SetTitle(const char* title) override;
	const char* GetTitle() override;

	virtual void SetWindowMode(WindowMode mode, unsigned int width = 0, unsigned int height = 0) override;

	virtual void MaximizeWindow() override;
	virtual void RestoreWindow() override;

	inline virtual std::any GetNativeWindow() const override { return m_Window; }
	inline virtual Ref<GraphicsContext> GetContext() const override { return m_Context; }

	virtual void DisableCursor() override;
	virtual void EnableCursor() override;
	virtual void SetCursorPosition(double xpos, double ypos) override;

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

