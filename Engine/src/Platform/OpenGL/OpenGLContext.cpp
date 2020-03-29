#include "stdafx.h"
#include "OpenGLContext.h"

#include <glad/glad.h>

OpenGLContext::OpenGLContext(GLFWwindow * windowHandle)
	:m_windowHandle(windowHandle)
{
	CORE_ASSERT(windowHandle, "Window Handle is null")
}

void OpenGLContext::Init()
{
	PROFILE_FUNCTION();
	glfwMakeContextCurrent(m_windowHandle);

	int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	CORE_ASSERT(status, "Failed to initialize GLAD");

	ENGINE_INFO("Graphics Card: {0} {1}", glGetString(GL_VENDOR), glGetString(GL_RENDERER));
	ENGINE_INFO("OpenGl Version: {0}", glGetString(GL_VERSION));
}

void OpenGLContext::SwapBuffers()
{
	PROFILE_FUNCTION();
	glfwSwapBuffers(m_windowHandle);
}
