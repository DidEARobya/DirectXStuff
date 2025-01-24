#include "Graphics.h"

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

	D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &swapChainDescriptor, &_pSwapChain, &_pDevice, nullptr, &_pContext);

	ID3D11Resource* pBackBuffer = nullptr;
	_pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer));
	_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pTarget);

	pBackBuffer->Release();
}

Graphics::~Graphics()
{
	if (_pTarget != nullptr)
	{
		_pTarget->Release();
	}

	if (_pContext != nullptr)
	{
		_pContext->Release();
	}

	if (_pSwapChain != nullptr)
	{
		_pSwapChain->Release();
	}

	if (_pDevice != nullptr)
	{
		_pDevice->Release();
	}
}

void Graphics::EndFrame()
{
	_pSwapChain->Present(1u, 0u);
}

void Graphics::ClearBuffer(float r, float g, float b) noexcept
{
	const float colour[] = { r, g, b, 1.0f };
	_pContext->ClearRenderTargetView(_pTarget, colour);
}
