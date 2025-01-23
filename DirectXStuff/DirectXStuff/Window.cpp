#include "Window.h"
#include <sstream>
#include "resource.h"

#pragma region "WindowClass"
Window::WindowClass Window::WindowClass::_windowClass;

Window::WindowClass::WindowClass() noexcept : _hInstance(GetModuleHandle(nullptr))
{
	WNDCLASSEX wClass = { 0 };

	wClass.cbSize = sizeof(wClass);
	wClass.style = CS_OWNDC;
	wClass.lpfnWndProc = HandleMessageSetup;
	wClass.cbClsExtra = 0;
	wClass.cbWndExtra = 0;
	wClass.hInstance = GetInstance();
	wClass.hIcon = static_cast<HICON>(LoadImage(_hInstance, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 32, 32, 0));
	wClass.hIconSm = static_cast<HICON>(LoadImage(_hInstance, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 16, 16, 0));
	wClass.hCursor = nullptr;
	wClass.hbrBackground = nullptr;
	wClass.lpszMenuName = nullptr;
	wClass.lpszClassName = GetName();

	RegisterClassEx(&wClass);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(_windowClassName, GetInstance());
}

const char* Window::WindowClass::GetName() noexcept
{
	return _windowClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return _windowClass._hInstance;
}
#pragma endregion

#pragma region "Window"
Window::Window(int width, int height, const char* name) : _width(width), _height(height)
{
	RECT windowRect;
	windowRect.left = 100;
	windowRect.right = _width + windowRect.left;
	windowRect.top = 100;
	windowRect.bottom = _height + windowRect.top;

	if (AdjustWindowRect(&windowRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
	{
		throw WINDOW_LAST_EXCEPT();
	}

	_hWnd = CreateWindow(WindowClass::GetName(), name, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT,
		windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, nullptr, nullptr, WindowClass::GetInstance(), this);

	if (_hWnd == nullptr)
	{
		throw WINDOW_LAST_EXCEPT();
	}

	ShowWindow(_hWnd, SW_SHOWDEFAULT);
}

Window::~Window()
{
	DestroyWindow(_hWnd);
}

void Window::SetTitle(const std::string& title)
{
	if (SetWindowText(_hWnd, title.c_str()) == 0)
	{
		throw WINDOW_LAST_EXCEPT();
	}
}

LRESULT WINAPI Window::HandleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg != WM_NCCREATE)
	{
		//If there is a message before the creation is complete (WM_NCCREATE message), handle with the default handler
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
	Window* const pWindow = static_cast<Window*>(pCreate->lpCreateParams);

	// Set WinAPI-managed User Data to store ptr to window class
	SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
	// Set message proc to normal (non-setup) handler now that setup is finished
	SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMessageInvoke));

	//Forward message to window class handler
	return pWindow->HandleMessage(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI Window::HandleMessageInvoke(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window* const pWindow = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWindow->HandleMessage(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (msg)
	{
		//----WINDOW STATES----
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
		case WM_KILLFOCUS:
		{
			keyboard.ClearKeyStates();
			break;
		}

		//----KEYBOARD MESSAGES----
		case WM_KEYDOWN:
		{
			if ((lParam & 0x40000000) == false || keyboard.IsAutoRepeateEnabled() == true)
			{
				keyboard.OnKeyPressed(static_cast<unsigned char>(wParam));
			}
			break;
		}
		case WM_KEYUP:
		{
			keyboard.OnKeyReleased(static_cast<unsigned char>(wParam));
			break;
		}
		case WM_SYSKEYDOWN:
		{
			if ((lParam & 0x40000000) == false || keyboard.IsAutoRepeateEnabled() == true)
			{
				keyboard.OnKeyPressed(static_cast<unsigned char>(wParam));
			}
			break;
		}
		case WM_SYSKEYUP:
		{
			keyboard.OnKeyReleased(static_cast<unsigned char>(wParam));
			break;
		}
		case WM_CHAR:
		{
			keyboard.OnChar(static_cast<unsigned char>(wParam));
			break;
		}

		//----MOUSE MESSAGES----
		case WM_MOUSEMOVE:
		{
			const POINTS pt = MAKEPOINTS(lParam);

			if (pt.x >= 0 && pt.x < _width && pt.y >= 0 && pt.y < _height)
			{
				mouse.OnMouseMove(pt.x, pt.y);

				if (mouse.IsInWindow() == false)
				{
					SetCapture(hWnd);
					mouse.OnMouseEnter();
				}
			}
			else
			{
				if (mouse.IsLeftPressed() == true || mouse.IsRightPressed() == true)
				{
					mouse.OnMouseMove(pt.x, pt.y);
				}
				else
				{
					ReleaseCapture();
					mouse.OnMouseLeave();
				}
			}
			break;
		}
		case WM_LBUTTONDOWN:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnLeftPressed(pt.x, pt.y);
			break;
		}
		case WM_RBUTTONDOWN:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnRightPressed(pt.x, pt.y);
			break;
		}
		case WM_MBUTTONDOWN:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnWheelPressed(pt.x, pt.y);
			break;
		}
		case WM_LBUTTONUP:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnLeftReleased(pt.x, pt.y);
			break;
		}
		case WM_RBUTTONUP:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnRightReleased(pt.x, pt.y);
			break;
		}
		case WM_MBUTTONUP:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnWheelReleased(pt.x, pt.y);
			break;
		}
		case WM_MOUSEWHEEL:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
			mouse.OnWheelDelta(pt.x, pt.y, delta);
			break;
		}
	}
	
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

#pragma endregion

#pragma region "Exception"
Window::Exception::Exception(int line, const char* file, HRESULT hResult) noexcept : WinException(line, file), _hResult(hResult)
{
}

const char* Window::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] " << GetErrorCode() << std::endl
		<< "[Description] " << GetErrorString() << std::endl
		<< GetOriginString();

	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Window::Exception::GetType() const noexcept
{
	return "Window Exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT hResult) noexcept
{
	char* pMessageBuffer = nullptr;
	DWORD nMessageLength = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, hResult,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&pMessageBuffer), 0, nullptr);

	if (nMessageLength == 0)
	{
		return "Unidentified Error Code";
	}

	std::string errorString = pMessageBuffer;
	LocalFree(pMessageBuffer);
	return errorString;
}

HRESULT Window::Exception::GetErrorCode() const noexcept
{
	return _hResult;
}

std::string Window::Exception::GetErrorString() const noexcept
{
	return TranslateErrorCode(_hResult);
}
#pragma endregion