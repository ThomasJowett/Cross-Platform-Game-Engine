#include "stdafx.h"
#include "WebGPUContext.h"
#include "Logging/Instrumentor.h"
#include <glad/glad.h>

#pragma comment(lib, "WebGPU32.lib")

int s_Version;

WebGPUContext::WebGPUContext(GLFWwindow * windowHandle)
	:m_windowHandle(windowHandle)
{
	CORE_ASSERT(windowHandle, "Window Handle is null")
}

void WebGPUContext::Init()
{
	PROFILE_FUNCTION();
	glfwMakeContextCurrent(m_windowHandle);

	int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	CORE_ASSERT(status, "Failed to initialize GLAD");

    const char* version = (const char*)glGetString(GL_VERSION);
    ENGINE_INFO("Graphics Card: {0} {1}", (const char*)glGetString(GL_VENDOR), (const char*)glGetString(GL_RENDERER));
	ENGINE_INFO("WebGPU Version: {0}", version);
    
    int major, minor;
    sscanf(version, "%d.%d", &major, &minor);
    s_Version = major * 100 + minor * 10;
}

void WebGPUContext::SwapBuffers()
{
	PROFILE_FUNCTION();
	glfwSwapBuffers(m_windowHandle);
}

void WebGPUContext::SetSwapInterval(uint32_t interval) 
{
	glfwSwapInterval((int)interval);
}

int WebGPUContext::GetVersion()
{
    return s_Version;
}

void WebGPUContext::MakeCurrent()
{
	glfwMakeContextCurrent(m_windowHandle);
}
