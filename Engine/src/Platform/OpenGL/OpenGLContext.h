#pragma once

#include "Core/core.h"

#include "Renderer/GraphicsContext.h"

#include "GLFW/glfw3.h"

class OpenGLContext : public GraphicsContext
{
public:
	OpenGLContext(GLFWwindow* windowHandle);
	virtual void Init() override;
	virtual void SwapBuffers() override;
	virtual void ResizeBuffers(uint32_t width, uint32_t height) override {};
	virtual void SetSwapInterval(uint32_t interval) override;
    
    static int GetVersion();
private:
	GLFWwindow* m_windowHandle;
};