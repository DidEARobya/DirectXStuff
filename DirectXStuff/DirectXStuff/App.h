#pragma once
#include "Window.h"
#include "WinTimer.h"

class App
{
public:
	App();

	int StartApp();

private:
	void Update();
	
private:
	Window _window;
	WinTimer _timer;
};

