#include "stdafx.h"
#include "Win32Window.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
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

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

Win32Window::Win32Window(const WindowProps& props)
{
	RECT rc;
	GetClientRect(m_Window, &rc);
}

Win32Window::~Win32Window()
{
}

void Win32Window::OnUpdate()
{
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

void Win32Window::SetWindowMode(const WindowMode& mode, unsigned int width, unsigned int height)
{
	//TODO: change window mode
}

HRESULT Win32Window::Init(const WindowProps& props)
{
	//Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = m_Instance;
	//wcex.hIcon = LoadIcon(m_Instance, (LPCTSTR)IDI_ICON1)//TODO:: generate the resource file and include to here to get the Icon.ico
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW); // TODO: make a cursor management class to allow the switching of the cursor from the default arrow
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"EngineWindowClass";
	//wcex.hIconSm = LoadIcon(wcex.hInstance, (LPSTR)IDI_ICON1);
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	int len;
	int slength = (int)props.Title.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, props.Title.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, props.Title.c_str(), slength, buf, len);
	std::wstring wTitle(buf);
	delete[] buf;

	//create the window
	RECT rc = { 0,0, (LONG)props.Width, (LONG)props.Height };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	m_Window = CreateWindow(wcex.lpszClassName, wTitle.c_str(), WS_OVERLAPPEDWINDOW,
		CW_DEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, m_Instance, nullptr);

	if (!m_Window)
		return E_FAIL;

	ShowWindow(m_Window, 1);

	return S_OK;
}

void Win32Window::Shutdown()
{
}
