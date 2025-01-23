#include "Window.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	try
	{
		Window window(800, 300, "Main Window");

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
	catch (const WinException& exception)
	{
		MessageBox(nullptr, exception.what(), exception.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& exception)
	{
		MessageBox(nullptr, exception.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, "No Details", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}

	return -1;
}
