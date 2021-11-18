#pragma once

#include "Core/core.h"

#include "Renderer/GraphicsContext.h"

#include "GLFW/glfw3.h"

class OpenGLContext : public GraphicsContext
{
public:
	OpenGLContext(GLFWwindow* windowHandle);
	virtual void Init()override;
	virtual void SwapBuffers()override;
	virtual void SetSwapInterval(uint32_t interval)override;
private:
	GLFWwindow* m_windowHandle;
};