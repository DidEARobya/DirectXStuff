#pragma once
#include <chrono>

class WinTimer
{
public:
	WinTimer();
	float Mark();
	float TimeElapsed() const;

private:
	std::chrono::steady_clock::time_point _last;
};

