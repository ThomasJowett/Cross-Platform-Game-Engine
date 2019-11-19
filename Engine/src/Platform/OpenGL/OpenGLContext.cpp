#include "stdafx.h"
#include "OpenGLContext.h"

#include <GLAD/glad.h>

OpenGLContext::OpenGLContext(GLFWwindow * windowHandle)
	:m_windowHandle(windowHandle)
{
	CORE_ASSERT(windowHandle, "Window Handle is null")
}

void OpenGLContext::Init()
{
	glfwMakeContextCurrent(m_windowHandle);

	int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	CORE_ASSERT(status, "Failed to initialize GLAD");

	std::cout << glGetString(GL_VENDOR) << " " << glGetString(GL_VERSION) << " " << glGetString(GL_RENDERER) << std::endl;
}

void OpenGLContext::SwapBuffers()
{
	glfwSwapBuffers(m_windowHandle);
}
