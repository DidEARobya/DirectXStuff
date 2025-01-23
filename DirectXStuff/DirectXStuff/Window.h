#pragma once
#include "WinInclude.h"
#include "WinException.h"
#include "Keyboard.h"
#include "Mouse.h"

class Window
{
public:
	class Exception : public WinException
	{
	public:
		Exception(int line, const char* file, HRESULT hResult) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept override;
		static std::string TranslateErrorCode(HRESULT hResult) noexcept;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;

	private:
		HRESULT _hResult;
	};
private:
	class WindowClass
	{
	public:
		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;

	private:
		WindowClass() noexcept;
		~WindowClass();

		WindowClass(const WindowClass&) = delete;
		WindowClass& operator = (const WindowClass&) = delete;

	private:
		static constexpr const char* _windowClassName = "DirectX Window";
		static WindowClass _windowClass;
		HINSTANCE _hInstance;
	};

public:
	Window(int width, int height, const char* name);
	~Window();

	Window(const Window&) = delete;
	Window& operator = (const Window&) = delete;

	void SetTitle(const std::string& title);
private:
	static LRESULT CALLBACK HandleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK HandleMessageInvoke(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

public:
	Keyboard keyboard;
	Mouse mouse;
private:
	int _width;
	int _height;
	HWND _hWnd;
};

//Exception Helper Macros
#define WINDOW_EXCEPT(hResult) Window::Exception(__LINE__, __FILE__, hResult)
#define WINDOW_LAST_EXCEPT() Window::Exception(__LINE__, __FILE__, GetLastError())
