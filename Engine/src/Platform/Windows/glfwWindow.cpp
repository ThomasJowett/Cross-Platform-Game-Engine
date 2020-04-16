#include "stdafx.h"
#include "glfwWindow.h"

#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"

#include "Core/Joysticks.h"

#include "Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLContext.h"

#include "stb_image.h"

static uint8_t s_GLFWWindowCount = 0;

static void	GLFWErrorCallback(int error, const char* description)
{
	ENGINE_ERROR("GLFW Error {0} {1}", error, description);
}

glfwWindow::glfwWindow(const WindowProps& props)
{
	PROFILE_FUNCTION();
	Init(props);
}

glfwWindow::~glfwWindow()
{
	PROFILE_FUNCTION();
	Shutdown();
}

void glfwWindow::OnUpdate()
{
	PROFILE_FUNCTION();
	glfwPollEvents();
	m_context->SwapBuffers();
}

void glfwWindow::SetVSync(bool enabled)
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
	m_Data.VSync = enabled;
}

bool glfwWindow::IsVSync() const
{
	return m_Data.VSync;
}

void glfwWindow::SetIcon(const std::string & path)
{
	int width, height, channels;
	stbi_set_flip_vertically_on_load(0);
	stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 4);
	ASSERT(data, "Failed to load image!");
	ASSERT(channels == 4, "Icon must be RGBA");

	GLFWimage images[1];
	images[0].width = width;
	images[0].height = height;
	images[0].pixels = data;
	glfwSetWindowIcon(m_Window, 1, images);
	stbi_image_free(data);
}

void glfwWindow::SetWindowMode(const WindowMode & mode, unsigned int width, unsigned int height)
{
	if (!m_Window)
		return;
	if (mode == m_Data.Mode)
		return;

	if (m_Data.Mode == WindowMode::WINDOWED)
	{
		m_OldWindowedParams.width = m_Data.Width;
		m_OldWindowedParams.Height = m_Data.Height;
		glfwGetWindowPos(m_Window, &(m_OldWindowedParams.XPos), &(m_OldWindowedParams.YPos));
	}

	GLFWmonitor* monitor = nullptr;

	if (mode == WindowMode::BORDERLESS)
	{
		width = m_BaseVideoMode.width;
		height = m_BaseVideoMode.height;
		monitor = glfwGetPrimaryMonitor();
	}
	else if (mode == WindowMode::WINDOWED && (width == 0 || height == 0))
	{
		width = m_OldWindowedParams.width;
		height = m_OldWindowedParams.Height;
	}
	else if (mode == WindowMode::FULL_SCREEN)
	{
		if (width == 0 || height == 0)
		{
			width = m_Data.Width;
			height = m_Data.Height;
		}
		monitor = glfwGetPrimaryMonitor();
	}

	m_Data.Width = width;
	m_Data.Height = height;

	if (m_Data.EventCallback)
	{
		WindowResizeEvent event(width, height);
		m_Data.EventCallback(event);
	}

	m_Data.Mode = mode;

	glfwSetWindowMonitor(m_Window, monitor, m_OldWindowedParams.XPos, m_OldWindowedParams.YPos, width, height, m_BaseVideoMode.refreshRate);
}

void glfwWindow::Init(const WindowProps & props)
{
	PROFILE_FUNCTION();

	m_Data.Title = props.Title;
	m_Data.Width = props.Width;
	m_Data.Height = props.Height;
	m_Data.PosX = props.PosX;
	m_Data.PosY = props.PosY;
	m_Data.Mode = WindowMode::WINDOWED;

	ENGINE_INFO("Creating Window {0} {1} {2}", props.Title, props.Width, props.Height);

	if (s_GLFWWindowCount == 0)
	{
		PROFILE_SCOPE("glfw init");
		ENGINE_INFO("Initializing GLFW");
		int success = glfwInit();
		CORE_ASSERT(success, "Could not initialize GLFW!");
		glfwSetErrorCallback(GLFWErrorCallback);
	}

	m_BaseVideoMode = *(glfwGetVideoMode(glfwGetPrimaryMonitor()));

	RendererAPI::API api = Renderer::GetAPI();

	{
		PROFILE_SCOPE("glfw create window");
#ifdef DEBUG
		if (api == RendererAPI::API::OpenGL)
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif // DEBUG
		if (api == RendererAPI::API::OpenGL)
		{
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		}

		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
		++s_GLFWWindowCount;
	}

	if (Renderer::GetAPI() == RendererAPI::API::OpenGL)
	{
		m_context = CreateScope<OpenGLContext>(m_Window);
	}

	m_context->Init();

	m_OldWindowedParams.width = props.Width;
	m_OldWindowedParams.Height = props.Height;
	glfwSetWindowPos(m_Window, (int)props.PosX, (int)props.PosY);
	glfwGetWindowPos(m_Window, &(m_OldWindowedParams.XPos), &(m_OldWindowedParams.YPos));

	glfwSetWindowUserPointer(m_Window, &m_Data);
	SetVSync(true);

	glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});
	
	glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int posX, int posY)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.PosX = posX;
			data.PosY = posY;
		});
	
	glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});

	glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int focused)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			if (focused == GLFW_TRUE)
			{
				WindowFocusEvent event;
				data.EventCallback(event);
			}
			else
			{
				WindowFocusLostEvent event;
				data.EventCallback(event);
			}
		});
	
	glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
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

	glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event(keycode);
			data.EventCallback(event);
		});

	glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
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

	glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseWheelEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

	glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMotionEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});

	for (int jid = GLFW_JOYSTICK_1;  jid <= GLFW_JOYSTICK_LAST; jid++)
	{
		if (glfwJoystickPresent(jid))
		{
			GLFWgamepadstate state;
			Joysticks::Joystick joystick;
			joystick.ID = jid;
			joystick.Name = glfwGetJoystickName(joystick.ID);
			joystick.IsMapped = glfwGetGamepadState(joystick.ID, &state);
			glfwGetJoystickAxes(joystick.ID, &joystick.Axes);
			glfwGetJoystickButtons(joystick.ID, &joystick.Buttons);
			glfwGetJoystickHats(joystick.ID, &joystick.Hats);
			Joysticks::AddJoystick(joystick);
		}
	}

	glfwSetJoystickCallback([](int jid, int e)
		{
			if (e == GLFW_CONNECTED)
			{
				GLFWgamepadstate state;
				Joysticks::Joystick joystick;
				joystick.ID = jid;
				joystick.Name = glfwGetJoystickName(joystick.ID);
				joystick.IsMapped = glfwGetGamepadState(joystick.ID, &state);
				glfwGetJoystickAxes(joystick.ID, &joystick.Axes);
				glfwGetJoystickButtons(joystick.ID, &joystick.Buttons);
				glfwGetJoystickHats(joystick.ID, &joystick.Hats);
				Joysticks::AddJoystick(joystick);
				JoystickConnected event(jid);
				Joysticks::CallEvent(event);
			}
			else if (e == GLFW_DISCONNECTED)
			{
				Joysticks::RemoveJoystick(jid);
				JoystickDisconnected event(jid);
				Joysticks::CallEvent(event);
			}
		});
}

void glfwWindow::Shutdown()
{
	PROFILE_FUNCTION();

	glfwDestroyWindow(m_Window);

	if (--s_GLFWWindowCount == 0)
	{
		ENGINE_INFO("Terminating GLFW");
		glfwTerminate();
	}
}
