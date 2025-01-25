#include "App.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	try
	{
		return App{}.StartApp();
	}
	catch (const WinException& exception)
	{
		MessageBox(nullptr, exception.what(), exception.GetType(), MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL);
	}
	catch (const std::exception& exception)
	{
		MessageBox(nullptr, exception.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL);
	}
	catch (...)
	{
		MessageBox(nullptr, "No Details", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL);
	}

	return -1;
}
