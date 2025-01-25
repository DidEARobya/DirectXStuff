#pragma once
#include "WinInclude.h"
#include "WinException.h"
#include <d3d11.h>
#include <wrl.h>
#include <vector>
#include "DXGIInfoManager.h"

#pragma comment(lib, "d3d11.lib")

class Graphics
{
public:
	class Exception : public WinException
	{
		using WinException::WinException;
	};
	class GraphicsException : public Exception
	{
	public:
		GraphicsException(int line, const char* file, HRESULT hResult, std::vector<std::string> exceptionInfo = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		static std::string TranslateErrorCode(HRESULT hResult) noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;

	private:
		HRESULT _hResult;
		std::string _info;
	};
	class InfoException : public Exception
	{
	public:
		InfoException(int line, const char* file, std::vector<std::string> info) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string _info;
	};
	class DeviceRemovedException : public GraphicsException
	{
		using GraphicsException::GraphicsException;

	public:
		const char* GetType() const noexcept override;
	};
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator= (const Graphics&) = delete;
	~Graphics() = default;

	void EndFrame();
	void ClearBuffer(float r, float g, float b) noexcept;
	void DrawTestTriangle();

private:
#if defined(_DEBUG)
	DXGIInfoManager _infoManager;
#endif
	Microsoft::WRL::ComPtr<ID3D11Device> _pDevice = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain> _pSwapChain = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> _pContext = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _pTarget = nullptr;
};

//Exception Helper Macros
#define GRAPHICS_THROW_FAILED(hrcall) if(FAILED(hResult = (hrcall))) throw Graphics::GraphicsException(__LINE__, __FILE__, hResult)
#define GRAPHICS_DEVICE_REMOVED_EXCEPT(hResult) Graphics::DeviceRemovedException(__LINE__, __FILE__, hResult)

#if defined(_DEBUG)
#define GRAPHICS_EXCEPT(hResult) Graphics::GraphicsException( __LINE__,__FILE__,(hResult),_infoManager.GetMessages() )
#define GRAPHICS_THROW_INFO(hrcall) _infoManager.Set(); if( FAILED( hResult = (hrcall))) throw GRAPHICS_EXCEPT(hResult)
#define GRAPHICS_DEVICE_REMOVED_EXCEPT(hResult) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hResult),_infoManager.GetMessages() )
#define GRAPHICS_THROW_INFO_ONLY(call) _infoManager.Set(); (call); {auto messages = _infoManager.GetMessages(); if(!messages.empty()) {throw Graphics::InfoException(__LINE__,__FILE__, messages);}}
#else
#define GRAPHICS_EXCEPT(hResult) Graphics::GraphicsException( __LINE__,__FILE__,(hResult))
#define GRAPHICS_THROW_INFO(hrcall) GRAPHICS_THROW_NOINFO(hrcall)
#define GRAPHICS_DEVICE_REMOVED_EXCEPT(hResult) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hResult) )
#define GRAPHICS_THROW_INFO_ONLY(call) (call)
#endif
