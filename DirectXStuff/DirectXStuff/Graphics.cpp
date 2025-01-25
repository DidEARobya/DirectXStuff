#include "Graphics.h"
#include <sstream>
#include <d3dcompiler.h>

namespace wrl = Microsoft::WRL;

#pragma comment(lib, "D3DCompiler.lib")

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

void Graphics::DrawTestTriangle()
{
	struct Vertex
	{
		struct
		{
			float x;
			float y;
		} pos;

		struct
		{
			unsigned char r;
			unsigned char g;
			unsigned char b;
			unsigned char a;
		} colour;

	};

	//2D triangle at centre of screen
	Vertex vertices[] =
	{
		{0.0f, 0.5f, 255, 0, 0, 0},
		{0.5f, -0.5f, 0, 255, 0, 0},
		{-0.5f, -0.5f, 0, 0, 255, 0},
		{-0.3f, 0.3f, 0, 255, 0, 0},
		{0.3f, 0.3f, 0, 0, 255, 0},
		{0.0f, -0.8f, 255, 0, 0, 0},
	};

	vertices[0].colour.g = 255;

	//Create buffer
	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;

	D3D11_BUFFER_DESC bufferDescriptor = {};
	bufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
	bufferDescriptor.CPUAccessFlags = 0u;
	bufferDescriptor.MiscFlags = 0u;
	bufferDescriptor.ByteWidth = sizeof(vertices);
	bufferDescriptor.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA subData = {};
	subData.pSysMem = vertices;

	HRESULT hResult;
	GRAPHICS_THROW_INFO(_pDevice->CreateBuffer(&bufferDescriptor, &subData, &pVertexBuffer));

	//Bind vertex buffer to pipeline
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	_pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);

	//Setup and bind index buffer
	const unsigned short indices[] =
	{
		0, 1, 2,
		0, 2, 3,
		0, 4, 1,
		2, 1, 5,
	};

	wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = sizeof(indices);
	ibd.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices;

	GRAPHICS_THROW_INFO(_pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer));
	_pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	//Setup and bind pixel shader
	wrl::ComPtr<ID3D11PixelShader> pPixelShader;
	wrl::ComPtr<ID3DBlob> pBlob;
	GRAPHICS_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &pBlob));
	GRAPHICS_THROW_INFO(_pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));
	_pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

	//Setup and bind vertex shader
	wrl::ComPtr<ID3D11VertexShader> pVertexShader;
	GRAPHICS_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &pBlob));
	GRAPHICS_THROW_INFO(_pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));
	_pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

	//Input (vertex) layout (2D)
	wrl::ComPtr<ID3D11InputLayout> pInputLayout;
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOUR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	GRAPHICS_THROW_INFO(_pDevice->CreateInputLayout(ied, (UINT)std::size(ied), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout));
	_pContext->IASetInputLayout(pInputLayout.Get());

	//Render target setup
	_pContext->OMSetRenderTargets(1u, _pTarget.GetAddressOf(), nullptr);

	//Set primitive topology 
	_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Setup viewport
	D3D11_VIEWPORT viewport;
	viewport.Width = 800;
	viewport.Height = 600;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	_pContext->RSSetViewports(1u, &viewport);

	GRAPHICS_THROW_INFO_ONLY(_pContext->DrawIndexed((UINT)std::size(indices), 0u, 0u));
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

