#pragma once
#include "WinInclude.h"
#include "WinException.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include <optional>
#include <memory>
#include "WinMacros.h"

class Window
{
public:
	class Exception : public WinException
	{
		using WinException::WinException;

	public:
		static std::string TranslateErrorCode(HRESULT hResult) noexcept;
	};
	class WindowException : public Exception
	{
	public:
		WindowException(int line, const char* file, HRESULT hResult) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorDescription() const noexcept;

	private:
		HRESULT _hResult;
	};
	class NoGraphicsException : public Exception
	{
	public:
		using Exception::Exception;
		const char* GetType() const noexcept override;
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
	static std::optional<int> ProcessMessages();
	Graphics& GetGraphics();
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
	std::unique_ptr<Graphics> _graphics;
};
