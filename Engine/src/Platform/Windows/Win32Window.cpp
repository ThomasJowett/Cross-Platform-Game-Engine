#include "stdafx.h"
#include "Win32Window.h"

#include "Platform/DirectX/DirectX11Context.h"

#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"

#include "Core/Application.h"
#include "Core/Settings.h"
#include "Core/Input.h"

//extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK Win32Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WindowData* data = (WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (data == nullptr)
		return DefWindowProc(hWnd, message, wParam, lParam);

	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
	{
		int width = LOWORD(lParam);
		int height = HIWORD(lParam);

		data->Width = width;
		data->Height = height;

		WindowResizeEvent event(width, height);
		data->EventCallback(event);
		break;
	}
	case WM_CLOSE:
	{
		WindowCloseEvent event;
		data->EventCallback(event);
		break;
	}
	case WM_SETFOCUS:
	{
		WindowFocusEvent event;
		data->EventCallback(event);
		break;
	}
	case WM_KILLFOCUS:
	{
		WindowFocusLostEvent event;
		data->EventCallback(event);
		break;
	}
	case WM_MOVE:
	{
		int posX = LOWORD(lParam);
		int posY = HIWORD(lParam);

		data->PosX = posX;
		data->PosY = posY;

		WindowMoveEvent event(posX, posY);
		data->EventCallback(event);
		break;
	}

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

Win32Window::Win32Window(const WindowProps& props)
{
	PROFILE_FUNCTION();
	if (FAILED(Init(props)))
	{
		CORE_ASSERT(false, "Win32 Window failed to initialise");
	}
}

Win32Window::~Win32Window()
{
	Shutdown();
}

void Win32Window::OnUpdate()
{
	MessageLoop();
	m_Context->SwapBuffers();
}

void Win32Window::SetVSync(bool enabled)
{
	//TODO: set vsync
	m_Data.VSync = enabled;
}

bool Win32Window::IsVSync() const
{
	return m_Data.VSync;
}

void Win32Window::SetIcon(const std::string& path)
{
	//TODO: change icon
}

void Win32Window::SetCursor(Cursors cursorType)
{
	//TODO: set cursor
}

void Win32Window::DisableCursor()
{
	//TODO: disable cursor
}

void Win32Window::EnableCursor()
{
	//TODO: enable cursor
}

void Win32Window::SetWindowMode(const WindowMode& mode, unsigned int width, unsigned int height)
{
	//TODO: change window mode
}

HRESULT Win32Window::Init(const WindowProps& props)
{
	Input::SetInput(RendererAPI::GetAPI());
	m_Instance = GetModuleHandle(0);

	m_Data.Title = props.Title;
	m_Data.Width = Settings::GetInt("Display", "Screen_Width");
	m_Data.Height = Settings::GetInt("Display", "Screen_Height");
	m_Data.PosX = Settings::GetInt("Display", "Window_Position_X");
	m_Data.PosY = Settings::GetInt("Display", "Window_Position_Y");
	m_Data.Mode = WindowMode::WINDOWED;

	//Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = m_Instance;
	wcex.hIcon = nullptr;
	//wcex.hIcon = LoadIcon(m_Instance, (LPCTSTR)IDI_ICON1)//TODO:: generate the resource file and include to here to get the Icon.ico
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW); // TODO: make a cursor management class to allow the switching of the cursor from the default arrow
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"EngineWindowClass";
	wcex.hIconSm = nullptr;
	//wcex.hIconSm = LoadIcon(wcex.hInstance, (LPSTR)IDI_ICON1);
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	int len;
	int slength = (int)m_Data.Title.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, m_Data.Title.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, m_Data.Title.c_str(), slength, buf, len);
	std::wstring wTitle(buf);
	delete[] buf;

	//create the window
	RECT rc = { 0,0, (LONG)m_Data.Width, (LONG)m_Data.Height };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	m_Window = CreateWindow(wcex.lpszClassName, wTitle.c_str(), WS_OVERLAPPEDWINDOW,
		m_Data.PosX, m_Data.PosY, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, m_Instance, nullptr);

	if (!m_Window)
		return E_FAIL;


	ENGINE_INFO("Creating Window {0} {1} {2}", m_Data.Title, m_Data.Width, m_Data.Height);

	ShowWindow(m_Window, 1);
	SetWindowLongPtr(m_Window, GWLP_USERDATA, (LONG_PTR)&m_Data);

	++s_Win32WindowCount;

	m_Context = CreateRef<DirectX11Context>(m_Window);

	m_Context->Init();

	return S_OK;
}

void Win32Window::Shutdown()
{
	--s_Win32WindowCount;
}

void Win32Window::MessageLoop()
{
	MSG msg = { 0 };

	//if (ImGui_ImplWin32_WndProcHandler(msg.hwnd, msg.message, msg.wParam, msg.lParam))
	//	return;

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		bool handled = false;
		if (msg.message >= WM_KEYFIRST && msg.message <= WM_KEYLAST)
		{
			KeyPressedEvent event(msg.message, 0);
			m_Data.EventCallback(event);
			handled = true;
		}
		else if (msg.message == WM_QUIT)
		{
			WindowCloseEvent event;
			m_Data.EventCallback(event);
			handled = true;
		}
		else if (msg.message == WM_SETFOCUS)
		{
			WindowFocusEvent event;
			m_Data.EventCallback(event);
			handled = true;
		}

		if (!handled)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}
