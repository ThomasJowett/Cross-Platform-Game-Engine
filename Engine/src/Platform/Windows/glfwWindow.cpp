#include "stdafx.h"
#include "glfwWindow.h"

#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/JoystickEvent.h"

#include "Core/Joysticks.h"

#include "Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLContext.h"

#include "stb_image.h"

#include "Core/Input.h"
#include "Core/Settings.h"

static uint8_t s_GLFWWindowCount = 0;

static void	GLFWErrorCallback(int error, const char* description)
{
	if (error == 65545) return;
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
	m_Context->SwapBuffers();
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

	Settings::SetBool("Display", "V-Sync", enabled);
	m_Data.VSync = enabled;
}

bool glfwWindow::IsVSync() const
{
	return m_Data.VSync;
}

void glfwWindow::SetIcon(const std::filesystem::path& path)
{
	int width, height, channels;
	stbi_set_flip_vertically_on_load(0);
	stbi_uc* data = stbi_load(path.string().c_str(), &width, &height, &channels, 4);
	ASSERT(data, "Failed to load image!");
	ASSERT(channels == 4, "Icon must be RGBA");

	GLFWimage images[1];
	images[0].width = width;
	images[0].height = height;
	images[0].pixels = data;
	glfwSetWindowIcon(m_Window, 1, images);
	stbi_image_free(data);
}

void glfwWindow::SetCursor(Cursors cursorType)
{
	PROFILE_FUNCTION();

	if (!m_SystemCursors[(int)cursorType])
	{
		switch (cursorType)
		{
		case Cursors::Arrow:
			m_SystemCursors[(int)cursorType] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR); break;
		case Cursors::IBeam:
			m_SystemCursors[(int)cursorType] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR); break;
		case Cursors::CrossHair:
			m_SystemCursors[(int)cursorType] = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR); break;
		case Cursors::PointingHand:
			m_SystemCursors[(int)cursorType] = glfwCreateStandardCursor(GLFW_POINTING_HAND_CURSOR); break;
		case Cursors::ResizeEW:
			m_SystemCursors[(int)cursorType] = glfwCreateStandardCursor(GLFW_RESIZE_EW_CURSOR); break;
		case Cursors::ResizeNS:
			m_SystemCursors[(int)cursorType] = glfwCreateStandardCursor(GLFW_RESIZE_NS_CURSOR); break;
		case Cursors::ResizeNWSE:
			m_SystemCursors[(int)cursorType] = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR); break;
		case Cursors::ResizeNESW:
			m_SystemCursors[(int)cursorType] = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR); break;
		case Cursors::ResizeAll:
			m_SystemCursors[(int)cursorType] = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR); break;
		case Cursors::NotAllowed:
			m_SystemCursors[(int)cursorType] = glfwCreateStandardCursor(GLFW_NOT_ALLOWED_CURSOR); break;
		default:
			return;
		}
	}

	glfwSetCursor(m_Window, m_SystemCursors[(int)cursorType]);
}

void glfwWindow::SetWindowMode(WindowMode mode, unsigned int width, unsigned int height)
{
	if (!m_Window)
		return;
	if (mode == m_Data.Mode)
		return;

	GLFWmonitor* monitor = nullptr;


	if (m_Data.Mode == WindowMode::WINDOWED)
	{
		m_OldWindowedParams.width = m_Data.Width;
		m_OldWindowedParams.Height = m_Data.Height;
		glfwGetWindowPos(m_Window, &(m_OldWindowedParams.XPos), &(m_OldWindowedParams.YPos));
	}

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
	else if(mode != WindowMode::WINDOWED)
	{
		ENGINE_ERROR("Invalid window mode, reverting to default");
		if (width == 0 || height == 0)
		{
			width = m_OldWindowedParams.width;
			height = m_OldWindowedParams.Height;
		}
		mode = (WindowMode)Settings::GetDefaultInt("Display", "Window_Mode");
	}

	m_Data.Width = width;
	m_Data.Height = height;

	if (m_Data.EventCallback)
	{
		WindowResizeEvent event(width, height);
		m_Data.EventCallback(event);
	}

	Settings::SetInt("Display", "Window_Mode", (int)mode);
	m_Data.Mode = mode;

	glfwSetWindowMonitor(m_Window, monitor, m_OldWindowedParams.XPos, m_OldWindowedParams.YPos, width, height, m_BaseVideoMode.refreshRate);
}

void glfwWindow::MaximizeWindow()
{
	glfwMaximizeWindow(m_Window);
}

void glfwWindow::RestoreWindow()
{
	glfwRestoreWindow(m_Window);
}

void glfwWindow::DisableCursor()
{
	glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void glfwWindow::EnableCursor()
{
	glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void glfwWindow::SetCursorPosition(double xpos, double ypos)
{
	glfwSetCursorPos(m_Window, xpos, ypos);
}

void glfwWindow::Init(const WindowProps& props)
{
	Input::SetInput(RendererAPI::GetAPI());
	PROFILE_FUNCTION();

	m_Data.Title = props.Title;
	m_Data.Width = Settings::GetInt("Display", "Screen_Width");
	m_Data.Height = Settings::GetInt("Display", "Screen_Height");
	m_Data.PosX = Settings::GetInt("Display", "Window_Position_X");
	m_Data.PosY = Settings::GetInt("Display", "Window_Position_Y");
	m_Data.Mode = WindowMode::WINDOWED;

	ENGINE_INFO("Creating Window {0} {1} {2}", m_Data.Title, m_Data.Width, m_Data.Height);

	if (s_GLFWWindowCount == 0)
	{
		PROFILE_SCOPE("glfw init");
		ENGINE_INFO("Initializing GLFW");
		int success = glfwInit();
		CORE_ASSERT(success == GLFW_TRUE, "Could not initialize GLFW!");
		glfwSetErrorCallback(GLFWErrorCallback);
	}

	m_BaseVideoMode = *(glfwGetVideoMode(glfwGetPrimaryMonitor()));

	RendererAPI::API api = Renderer::GetAPI();

	{
		PROFILE_SCOPE("glfw create window");
		if (api == RendererAPI::API::OpenGL)
		{
#ifdef DEBUG
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif // DEBUG

			//Minimum version that is supported is 3.3
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		}

		m_Window = glfwCreateWindow((int)m_Data.Width, (int)m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);
		++s_GLFWWindowCount;
	}

	if (Renderer::GetAPI() == RendererAPI::API::OpenGL)
	{
		m_Context = CreateRef<OpenGLContext>(m_Window);
	}

	m_Context->Init();

	m_OldWindowedParams.width = m_Data.Width;
	m_OldWindowedParams.Height = m_Data.Height;
	glfwSetWindowPos(m_Window, (int)m_Data.PosX, (int)m_Data.PosY);
	glfwGetWindowPos(m_Window, &(m_OldWindowedParams.XPos), &(m_OldWindowedParams.YPos));

	glfwSetWindowUserPointer(m_Window, &m_Data);
	SetVSync(Settings::GetBool("Display", "V-Sync"));
	SetWindowMode((WindowMode)Settings::GetInt("Display", "Window_Mode"));

	glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

	glfwSetWindowMaximizeCallback(m_Window, [](GLFWwindow* window, int maximized)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Maximized = maximized;

			WindowMaximizedEvent event(maximized);
			data.EventCallback(event);
		});

	glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int posX, int posY)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.PosX = posX;
			data.PosY = posY;

			WindowMoveEvent event(posX, posY);
			data.EventCallback(event);
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

	for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; jid++)
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
				Application::CallEvent(event);
			}
			else if (e == GLFW_DISCONNECTED)
			{
				Joysticks::RemoveJoystick(jid);
				JoystickDisconnected event(jid);
				Application::CallEvent(event);
			}
		});
}

void glfwWindow::Shutdown()
{
	PROFILE_FUNCTION();

	for (size_t i = 0; i < (size_t)Cursors::NumCursors; i++)
	{
		if (m_SystemCursors[i])
			glfwDestroyCursor(m_SystemCursors[i]);
	}

	glfwDestroyWindow(m_Window);

	if (--s_GLFWWindowCount == 0)
	{
		ENGINE_INFO("Terminating GLFW");
		glfwTerminate();
	}

}