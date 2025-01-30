#define FULL_WIN
#include "WinInclude.h"
#include "GDIPlusManager.h"
#include <algorithm>
namespace Gdiplus
{
	using std::min;
	using std::max;
}
#include <gdiplus.h>

ULONG_PTR GDIPlusManager::_token = 0;
int GDIPlusManager::_refCount = 0;

GDIPlusManager::GDIPlusManager()
{
	if (_refCount++ == 0)
	{
		Gdiplus::GdiplusStartupInput input;
		input.GdiplusVersion = 1;
		input.DebugEventCallback = nullptr;
		input.SuppressBackgroundThread = false;
		Gdiplus::GdiplusStartup(&_token, &input, nullptr);
	}
}

GDIPlusManager::~GDIPlusManager()
{
	if (--_refCount == 0)
	{
		Gdiplus::GdiplusShutdown(_token);
	}
}