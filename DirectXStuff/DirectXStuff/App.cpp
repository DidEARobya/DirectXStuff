#include "App.h"

App::App() : _window(800, 600, "Main App")
{

}

int App::StartApp()
{
	while (true)
	{
		//If ecode has a value, there is an error or the application wants to quit, else call Update()
		if (const auto ecode = Window::ProcessMessages())
		{
			return *ecode;
		}

		Update();
	}
}
//Executes frame logic
void App::Update()
{
	const float time = _timer.TimeElapsed();

	std::ostringstream oss;
	oss << "Time Elapsed: " << std::setprecision(1) << std::fixed << time << "s";
	_window.SetTitle(oss.str());

	const float cVal = sin(time) / 2.0f + 0.5f;
	_window.GetGraphics().ClearBuffer(cVal, cVal, 1);
	_window.GetGraphics().DrawTestTriangle();
	_window.GetGraphics().EndFrame();
}
