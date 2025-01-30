#include "Graphics.h"
#include <sstream>
#include "GraphicsMacros.h"
#include "imgui/backends/imgui_impl_dx11.h"
#include "imgui/backends/imgui_impl_win32.h"

namespace wrl = Microsoft::WRL;
namespace dirx = DirectX;

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib")

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

	//Set up Depth Stencil/Z-Buffer
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	wrl::ComPtr<ID3D11DepthStencilState> pDepthStencilState;
	GRAPHICS_THROW_INFO(_pDevice->CreateDepthStencilState(&depthStencilDesc, &pDepthStencilState));

	_pContext->OMSetDepthStencilState(pDepthStencilState.Get(), 1u);

	wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
	D3D11_TEXTURE2D_DESC depthDesc = {};
	depthDesc.Width = 800u;
	depthDesc.Height = 600u;
	depthDesc.MipLevels = 1u;
	depthDesc.ArraySize = 1u;
	depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthDesc.SampleDesc.Count = 1u;
	depthDesc.SampleDesc.Quality = 0u;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	GRAPHICS_THROW_INFO(_pDevice->CreateTexture2D(&depthDesc, nullptr, &pDepthStencil));

	//Create view of depth stencil texture
	D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc = {};
	depthViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthViewDesc.Texture2D.MipSlice = 0u;

	GRAPHICS_THROW_INFO(_pDevice->CreateDepthStencilView(pDepthStencil.Get(), &depthViewDesc, &_pDepthView));

	_pContext->OMSetRenderTargets(1u, _pTarget.GetAddressOf(), _pDepthView.Get());

	D3D11_VIEWPORT viewport;
	viewport.Width = 800;
	viewport.Height = 600;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	_pContext->RSSetViewports(1u, &viewport);

	//Init imgui d3d 
	ImGui_ImplDX11_Init(_pDevice.Get(), _pContext.Get());
}

Graphics::~Graphics()
{
	ImGui_ImplDX11_Shutdown();
}

void Graphics::BeginFrame(float r, float g, float b) noexcept
{
	if (_isImGuiEnabled == true)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	const float colour[] = { r, g, b, 1.0f };
	_pContext->ClearRenderTargetView(_pTarget.Get(), colour);
	_pContext->ClearDepthStencilView(_pDepthView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void Graphics::EndFrame()
{
	if (_isImGuiEnabled == true)
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

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

void Graphics::DrawIndexed(UINT count) noexcept(!IS_DEBUG)
{
	GRAPHICS_THROW_INFO_ONLY(_pContext->DrawIndexed(count, 0u, 0u));
}

void Graphics::SetProjection(DirectX::FXMMATRIX projection) noexcept
{
	_projection = projection;
}
DirectX::XMMATRIX Graphics::GetProjection() const noexcept
{
	return _projection;
}
void Graphics::SetCamera(DirectX::FXMMATRIX camera) noexcept
{
	_camera = camera;
}
DirectX::XMMATRIX Graphics::GetCamera() const noexcept
{
	return _camera;
}
void Graphics::EnableImGui() noexcept
{
	_isImGuiEnabled = true;
}
void Graphics::DisableImGui() noexcept
{
	_isImGuiEnabled = false;
}
bool Graphics::IsImGuiEnabled() const noexcept
{
	return _isImGuiEnabled;
}
#pragma region "GraphicsException"
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
}

HRESULT Graphics::GraphicsException::GetErrorCode() const noexcept
{
	return _hResult;
}

std::string Graphics::GraphicsException::GetErrorDescription() const noexcept
{
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

#pragma region "InfoException"
Graphics::InfoException::InfoException(int line, const char* file, std::vector<std::string> info) noexcept : Exception(line, file)
{
	for (const auto& message : info)
	{
		_info += message;
		_info.push_back('\n');
	}

	if (_info.empty() == false)
	{
		_info.pop_back();
	}
}

const char* Graphics::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();

	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::InfoException::GetType() const noexcept
{
	return "Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
	return _info;
}
#pragma endregion

