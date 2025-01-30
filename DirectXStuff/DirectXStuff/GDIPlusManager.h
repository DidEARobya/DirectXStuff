#pragma once
#include "WinInclude.h"

class GDIPlusManager
{
public:
	GDIPlusManager();
	~GDIPlusManager();
private:
	static ULONG_PTR _token;
	static int _refCount;
};


