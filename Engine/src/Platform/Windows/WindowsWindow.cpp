#include "stdafx.h"
#include "WindowsWindow.h"

#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"

#include "Renderer/Renderer.h"

#include "Logging/Debug.h"

#include "Platform/OpenGL/OpenGLContext.h"

static uint8_t s_GLFWWindowCount = 0;

static void	GLFWErrorCallback(int error, const char* description)
{
	DBG_OUTPUT(std::string("GLFW Error %i " + std::string(description)).c_str(), error, description);
}

Window* Window::Create(const WindowProps& props)
{
	return new WindowsWindow(props);
}

WindowsWindow::WindowsWindow(const WindowProps& props)
{
	PROFILE_FUNCTION();
	Init(props);
}

WindowsWindow::~WindowsWindow()
{
	PROFILE_FUNCTION();
	Shutdown();
}

void WindowsWindow::OnUpdate()
{
	PROFILE_FUNCTION();
	glfwPollEvents();
	m_context->SwapBuffers();
}

void WindowsWindow::SetVSync(bool enabled)
{
	PROFILE_FUNCTION();

	if (enabled)
	{
		glfwSwapInterval(1);
	}
	else
	{
		glfwSwapInterval(0);
	}
	m_data.VSync = enabled;
}

bool WindowsWindow::IsVSync() const
{
	return m_data.VSync;
}

void WindowsWindow::Init(const WindowProps & props)
{
	PROFILE_FUNCTION();

	m_data.Title = props.Title;
	m_data.Width = props.Width;
	m_data.Height = props.Height;
	m_data.PosX = props.PosX;
	m_data.PosY = props.PosY;

	OUTPUT(std::string("Creating Window " + props.Title + " %d %d \n").c_str(), props.Width, props.Height);

	if (s_GLFWWindowCount == 0)
	{
		PROFILE_SCOPE("glfw init");
		OUTPUT("Initializing GLFW\r\n");
		int success = glfwInit();
		CORE_ASSERT(success, "Could not initialize GLFW!");
		glfwSetErrorCallback(GLFWErrorCallback);
	}
	{
		PROFILE_SCOPE("glfw create window");
#ifdef DEBUG
		if (Renderer::GetAPI() == RendererAPI::API::OpenGL)
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif // DEBUG
		m_window = glfwCreateWindow((int)props.Width, (int)props.Height, m_data.Title.c_str(), nullptr, nullptr);
		++s_GLFWWindowCount;
	}
	m_context = CreateScope<OpenGLContext>(m_window);

	m_context->Init();

	glfwSetWindowPos(m_window, (int)props.PosX, (int)props.PosY);

	glfwSetWindowUserPointer(m_window, &m_data);
	SetVSync(true);

	glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});
	
	glfwSetWindowPosCallback(m_window, [](GLFWwindow* window, int posX, int posY)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.PosX = posX;
			data.PosY = posY;
		});
	
	glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});
	
	glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				KeyPressedEvent event(key, 0);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				KeyReleasedEvent event(key);
				data.EventCallback(event);
				break;
			}
			case GLFW_REPEAT:
			{
				KeyPressedEvent event(key, 1);
				data.EventCallback(event);
				break;
			}
			}
		});

	glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int keycode)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event(keycode);
			data.EventCallback(event);
		});

	glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent event(button);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent event(button);
				data.EventCallback(event);
				break;
			}
			}
		});

	glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseWheelEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

	glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMotionEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});
}

void WindowsWindow::Shutdown()
{
	PROFILE_FUNCTION();

	glfwDestroyWindow(m_window);

	if (--s_GLFWWindowCount == 0)
	{
		OUTPUT("Terminating GLFW");
		glfwTerminate();
	}
}
