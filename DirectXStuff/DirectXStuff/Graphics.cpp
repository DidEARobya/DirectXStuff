#include "Graphics.h"
//#include "dxerr.h"
#include <sstream>

namespace wrl = Microsoft::WRL;

Graphics::Graphics(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC swapChainDescriptor = {};
	swapChainDescriptor.BufferDesc.Width = 0;
	swapChainDescriptor.BufferDesc.Height = 0;
	swapChainDescriptor.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapChainDescriptor.BufferDesc.RefreshRate.Numerator = 0;
	swapChainDescriptor.BufferDesc.RefreshRate.Denominator = 0;
	swapChainDescriptor.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDescriptor.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDescriptor.SampleDesc.Count = 1;
	swapChainDescriptor.SampleDesc.Quality = 0;
	swapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDescriptor.BufferCount = 1;
	swapChainDescriptor.OutputWindow = hWnd;
	swapChainDescriptor.Windowed = TRUE;
	swapChainDescriptor.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDescriptor.Flags = 0;

	UINT swapCreateFlags = 0u;
#if defined(_DEBUG)
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hResult;

	GRAPHICS_THROW_INFO(
		D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, swapCreateFlags, nullptr, 0, D3D11_SDK_VERSION, &swapChainDescriptor, &_pSwapChain, &_pDevice, nullptr, &_pContext));

	wrl::ComPtr<ID3D11Resource> pBackBuffer = nullptr;
	GRAPHICS_THROW_INFO(_pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
	GRAPHICS_THROW_INFO(_pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &_pTarget));
}

void Graphics::EndFrame()
{
	HRESULT hResult;

#if defined (_DEBUG)
	_infoManager.Set();
#endif

	if (FAILED(hResult = _pSwapChain->Present(1u, 0u)))
	{
		if (hResult == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GRAPHICS_DEVICE_REMOVED_EXCEPT(_pDevice->GetDeviceRemovedReason());
			return;
		}

		GRAPHICS_THROW_INFO(hResult);
	}
}

void Graphics::ClearBuffer(float r, float g, float b) noexcept
{
	const float colour[] = { r, g, b, 1.0f };
	_pContext->ClearRenderTargetView(_pTarget.Get(), colour);
}

#pragma region "Exception"
Graphics::GraphicsException::GraphicsException(int line, const char* file, HRESULT hResult, std::vector<std::string> exceptionInfo) noexcept : Exception(line, file), _hResult(hResult)
{
	for (const auto& message : exceptionInfo)
	{
		_info += message;
		_info.push_back('\n');
	}

	if (_info.empty() == false)
	{
		_info.pop_back();
	}
}
const char* Graphics::GraphicsException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] " << std::hex << std::uppercase << GetErrorCode() << std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		//<< "[Error String ]" << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;

	if (_info.empty() == false)
	{
		oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	}

	oss	<< GetOriginString();

	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::GraphicsException::GetType() const noexcept
{
	return "Graphics Exception";
}

std::string Graphics::GraphicsException::TranslateErrorCode(HRESULT hResult) noexcept
{
	char* pMessageBuffer = nullptr;
	DWORD nMessageLength = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, hResult,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&pMessageBuffer), 0, nullptr);

	if (nMessageLength == 0)
	{
		return "Unidentified Error Code";
	}

	std::string errorString = pMessageBuffer;
	LocalFree(pMessageBuffer);
	return errorString;
	//return DXGetErrorString(_hResult);
}

HRESULT Graphics::GraphicsException::GetErrorCode() const noexcept
{
	return _hResult;
}

std::string Graphics::GraphicsException::GetErrorDescription() const noexcept
{
	//char buffer[512];
	//DXGetErrorDescription(_hResult, buffer, sizeof(buffer));
	return TranslateErrorCode(_hResult);
}

std::string Graphics::GraphicsException::GetErrorInfo() const noexcept
{
	return _info;
}

const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "Device Removed Exception";
}
#pragma endregion