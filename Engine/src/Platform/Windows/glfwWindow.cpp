#include "stdafx.h"
#include "glfwWindow.h"

#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/JoystickEvent.h"

#include "Core/Joysticks.h"
#include "Core/Input.h"

#include "Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLContext.h"
#include "Platform/Vulkan/VulkanContext.h"
#ifdef _WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"   // for glfwGetWin32Window
#include "Platform/DirectX/DirectX11Context.h"
#endif

#include "stb_image.h"

#include "Core/Settings.h"

static uint8_t s_GLFWWindowCount = 0;

static void	GLFWErrorCallback(int error, const char* description)
{
	if (error == 65545) return;
	ENGINE_ERROR("GLFW Error {0} {1}", error, description);
}

glfwWindow::glfwWindow()
{
	PROFILE_FUNCTION();
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

	m_Context->SetSwapInterval(enabled ? 1 : 0);

	Settings::SetBool("Display", "V-Sync", enabled);
	m_Data.vSync = enabled;
}

bool glfwWindow::IsVSync() const
{
	return m_Data.vSync;
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

void glfwWindow::SetTitle(const char* title)
{
	glfwSetWindowTitle(m_Window, title);
}

const char* glfwWindow::GetTitle()
{
	return m_Data.title.c_str();
}

void glfwWindow::SetWindowMode(WindowMode mode, unsigned int width, unsigned int height)
{
	if (!m_Window)
		return;
	if (mode == m_Data.mode)
		return;

	GLFWmonitor* monitor = nullptr;

	if (m_Data.mode == WindowMode::WINDOWED)
	{
		m_OldWindowedParams.width = m_Data.width;
		m_OldWindowedParams.Height = m_Data.height;
		glfwGetWindowPos(m_Window, &(m_OldWindowedParams.xPos), &(m_OldWindowedParams.yPos));
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
			width = m_Data.width;
			height = m_Data.height;
		}
		monitor = glfwGetPrimaryMonitor();
	}
	else if (mode != WindowMode::WINDOWED)
	{
		ENGINE_ERROR("Invalid window mode, reverting to default");
		if (width == 0 || height == 0)
		{
			width = m_OldWindowedParams.width;
			height = m_OldWindowedParams.Height;
		}
		mode = (WindowMode)Settings::GetDefaultInt(m_Data.title.c_str(), "Window_Mode");
	}

	m_Data.width = width;
	m_Data.height = height;

	if (m_Data.eventCallback)
	{
		WindowResizeEvent event(width, height);
		m_Data.eventCallback(event);
	}

	Settings::SetInt(m_Data.title.c_str(), "Window_Mode", (int)mode);
	m_Data.mode = mode;

	glfwSetWindowMonitor(m_Window, monitor, m_OldWindowedParams.xPos, m_OldWindowedParams.yPos, width, height, m_BaseVideoMode.refreshRate);
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

bool glfwWindow::Init(const WindowProps& props)
{
	PROFILE_FUNCTION();

	m_Data.title = props.title;
	m_Data.width = Settings::GetInt(props.title.c_str(), "Window_Width");
	m_Data.height = Settings::GetInt(props.title.c_str(), "Window_Height");
	m_Data.posX = Settings::GetInt(props.title.c_str(), "Window_Position_X");
	m_Data.posY = Settings::GetInt(props.title.c_str(), "Window_Position_Y");
	m_Data.mode = WindowMode::WINDOWED;

	ENGINE_INFO("Creating Window {0} {1} {2}", m_Data.title, m_Data.width, m_Data.height);

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

			//Minimum version that is supported is 4.6
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		}

		else if (api == RendererAPI::API::Directx11 || api == RendererAPI::API::Vulkan)
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		}

		m_Window = glfwCreateWindow((int)m_Data.width, (int)m_Data.height, m_Data.title.c_str(), nullptr, nullptr);
		if (m_Window)
			++s_GLFWWindowCount;
		else
		{
			ENGINE_CRITICAL("Could not create a window with graphics API {0}", Settings::GetValue("Renderer", "API"));
			return false;
		}
	}

	if (api == RendererAPI::API::OpenGL)
	{
		m_Context = CreateRef<OpenGLContext>(m_Window);
	}
	else if (api == RendererAPI::API::Directx11)
	{
#ifdef _WINDOWS
		m_Context = CreateRef<DirectX11Context>(glfwGetWin32Window(m_Window));
#endif
	}
	else if (api == RendererAPI::API::Vulkan)
	{
		m_Context = CreateRef<VulkanContext>(m_Window);
	}

	m_Context->Init();

	m_OldWindowedParams.width = m_Data.width;
	m_OldWindowedParams.Height = m_Data.height;
	glfwSetWindowPos(m_Window, (int)m_Data.posX, (int)m_Data.posY);
	glfwGetWindowPos(m_Window, &(m_OldWindowedParams.xPos), &(m_OldWindowedParams.yPos));

	glfwSetWindowUserPointer(m_Window, &m_Data);
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(m_Window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	SetVSync(Settings::GetBool("Display", "V-Sync"));
	SetWindowMode((WindowMode)Settings::GetInt(m_Data.title.c_str(), "Window_Mode"));

	{
		PROFILE_SCOPE("Window callbacks");

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.width = width;
				data.height = height;

				WindowResizeEvent event(width, height);
				data.eventCallback(event);
				Application::GetWindow()->GetContext()->ResizeBuffers(width, height);
			});

		glfwSetWindowMaximizeCallback(m_Window, [](GLFWwindow* window, int maximized)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.maximized = maximized;

				WindowMaximizedEvent event(maximized);
				data.eventCallback(event);
			});

		glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int posX, int posY)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.posX = posX;
				data.posY = posY;

				WindowMoveEvent event(posX, posY);
				data.eventCallback(event);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.eventCallback(event);
			});

		glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int focused)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				if (focused == GLFW_TRUE)
				{
					WindowFocusEvent event;
					data.eventCallback(event);
				}
				else
				{
					WindowFocusLostEvent event;
					data.eventCallback(event);
				}
			});
	}
	{
		PROFILE_SCOPE("Keyboard Callbacks");

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, false);
					data.eventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.eventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, true);
					data.eventCallback(event);
					break;
				}
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event(keycode);
				data.eventCallback(event);
			});
	}
	{
		PROFILE_SCOPE("Mouse Callbacks");

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.eventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.eventCallback(event);
					break;
				}
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseWheelEvent event((float)xOffset, (float)yOffset);
				data.eventCallback(event);

				Input::SetMouseWheel(xOffset, yOffset);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseMotionEvent event((float)xPos, (float)yPos);
				data.eventCallback(event);
			});
	}
	{
		PROFILE_SCOPE("Joysticks")
			for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; jid++)
			{
				PROFILE_SCOPE("Checking Joystick");
				if (glfwJoystickPresent(jid))
				{
					PROFILE_SCOPE("Adding Joystick");
					GLFWgamepadstate state;
					Joysticks::Joystick joystick;
					joystick.id = jid;
					joystick.name = glfwGetJoystickName(joystick.id);
					joystick.isMapped = glfwGetGamepadState(joystick.id, &state);
					glfwGetJoystickAxes(joystick.id, &joystick.axes);
					glfwGetJoystickButtons(joystick.id, &joystick.buttons);
					glfwGetJoystickHats(joystick.id, &joystick.hats);
					Joysticks::AddJoystick(joystick);
				}
			}
		{
			PROFILE_SCOPE("Joystick Callback");
			glfwSetJoystickCallback([](int jid, int e)
				{
					if (e == GLFW_CONNECTED)
					{
						GLFWgamepadstate state;
						Joysticks::Joystick joystick;
						joystick.id = jid;
						joystick.name = glfwGetJoystickName(joystick.id);
						joystick.isMapped = glfwGetGamepadState(joystick.id, &state);
						glfwGetJoystickAxes(joystick.id, &joystick.axes);
						glfwGetJoystickButtons(joystick.id, &joystick.buttons);
						glfwGetJoystickHats(joystick.id, &joystick.hats);
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
	}
	{
		glfwSetDropCallback(m_Window, [](GLFWwindow* window, int numDropped, const char** filenames)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				std::filesystem::path filepath = filenames[0];
				FileDropEvent event(filepath);
				data.eventCallback(event);
			});
	}
	return true;
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