#include <Windows.h>
#include <sstream>
#include "WindowsMessageMap.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static WindowsMessageMap messageMap;
	OutputDebugString(messageMap(msg, lParam, wParam).c_str());

	switch (msg)
	{
		case WM_KEYDOWN:
			if (wParam == 'F')
			{
				SetWindowText(hWnd, "F DOWN");
			}

			break;
		case WM_KEYUP:
			if (wParam == 'F')
			{
				SetWindowText(hWnd, "F UP");
			}

			break;
		case WM_CHAR:
			{
				static std::string title;
				title.push_back((char)wParam);
				SetWindowText(hWnd, title.c_str());
			
			}
			break;
		case WM_LBUTTONDOWN:
			{
				const POINTS points = MAKEPOINTS(lParam);
				std::ostringstream oss;
				oss << "(" << points.x << "," << points.y << ")";
				SetWindowText(hWnd, oss.str().c_str());
			}

			break;
		case WM_CLOSE:
			PostQuitMessage(1);

			break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	const auto pClassName = "wcClassName";

	//Register Window Class
	WNDCLASSEX windowsClass = { 0 };
	windowsClass.cbSize = sizeof(windowsClass);
	windowsClass.style = CS_OWNDC;
	windowsClass.lpfnWndProc = WndProc;
	windowsClass.cbClsExtra = 0;
	windowsClass.cbWndExtra = 0;
	windowsClass.hInstance = hInstance;
	windowsClass.hIcon = nullptr;
	windowsClass.hCursor = nullptr;
	windowsClass.hbrBackground = nullptr;
	windowsClass.lpszMenuName = nullptr;
	windowsClass.lpszClassName = pClassName;
	RegisterClassEx(&windowsClass);

	HWND handle = CreateWindowEx(0, pClassName, "Main Window", WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, 200, 200, 640, 480, nullptr, nullptr, hInstance, nullptr);

	ShowWindow(handle, SW_SHOW);
	MSG message;
	BOOL messageResult;

	while ((messageResult = GetMessage(&message, nullptr, 0, 0)) > 0)
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	if (messageResult == -1)
	{
		return -1;
	}

	return message.wParam;
}
