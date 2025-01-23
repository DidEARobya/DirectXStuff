#include "Window.h"
#include <sstream>
#include "resource.h"

#pragma region "WindowClass"
Window::WindowClass Window::WindowClass::windowClass;

Window::WindowClass::WindowClass() noexcept : hInstance(GetModuleHandle(nullptr))
{
	WNDCLASSEX wClass = { 0 };

	wClass.cbSize = sizeof(wClass);
	wClass.style = CS_OWNDC;
	wClass.lpfnWndProc = HandleMessageSetup;
	wClass.cbClsExtra = 0;
	wClass.cbWndExtra = 0;
	wClass.hInstance = GetInstance();
	wClass.hIcon = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 32, 32, 0));
	wClass.hIconSm = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 16, 16, 0));
	wClass.hCursor = nullptr;
	wClass.hbrBackground = nullptr;
	wClass.lpszMenuName = nullptr;
	wClass.lpszClassName = GetName();

	RegisterClassEx(&wClass);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(windowClassName, GetInstance());
}

const char* Window::WindowClass::GetName() noexcept
{
	return windowClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return windowClass.hInstance;
}
#pragma endregion

#pragma region "Window"
Window::Window(int width, int height, const char* name)
{
	RECT windowRect;
	windowRect.left = 100;
	windowRect.right = width + windowRect.left;
	windowRect.top = 100;
	windowRect.bottom = height + windowRect.top;

	if (FAILED(AdjustWindowRect(&windowRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE)))
	{
		throw WINDOW_LAST_EXCEPT();
	}

	hWnd = CreateWindow(WindowClass::GetName(), name, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT,
		windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, nullptr, nullptr, WindowClass::GetInstance(), this);

	if (hWnd == nullptr)
	{
		throw WINDOW_LAST_EXCEPT();
	}

	ShowWindow(hWnd, SW_SHOWDEFAULT);
}

Window::~Window()
{
	DestroyWindow(hWnd);
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
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

#pragma endregion

#pragma region "Exception"
Window::Exception::Exception(int line, const char* file, HRESULT hResult) noexcept : WinException(line, file), hResult(hResult)
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
	return hResult;
}

std::string Window::Exception::GetErrorString() const noexcept
{
	return TranslateErrorCode(hResult);
}
#pragma endregion