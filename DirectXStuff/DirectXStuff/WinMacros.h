#pragma once

//Exception Helper Macros
#define WINDOW_EXCEPT(hResult) Window::WindowException(__LINE__, __FILE__, hResult)
#define WINDOW_LAST_EXCEPT() Window::WindowException(__LINE__, __FILE__, GetLastError())
#define WINDOW_NO_GRAPHICS_EXCEPTION() Window::NoGraphicsException(__LINE__,__FILE__)