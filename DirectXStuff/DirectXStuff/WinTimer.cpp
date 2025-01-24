#include "WinTimer.h"

using namespace std::chrono;

WinTimer::WinTimer()
{
	_last = steady_clock::now();
}

//Gets time since last mark, and resets the mark
float WinTimer::Mark()
{
	const auto old = _last;
	_last = steady_clock::now();
	const duration<float> frameTime = _last - old;

	return frameTime.count();
}
//Gets time since last mark, doesn't reset the mark
float WinTimer::TimeElapsed() const
{
	return duration<float>(steady_clock::now() - _last).count();
}
