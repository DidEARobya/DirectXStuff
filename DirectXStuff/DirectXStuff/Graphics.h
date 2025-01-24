#pragma once
#include "WinInclude.h"
#include <d3d11.h>

#pragma comment(lib, "d3d11.lib")

class Graphics
{
public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator= (const Graphics&) = delete;
	~Graphics();

	void EndFrame();
	void ClearBuffer(float r, float g, float b) noexcept;
private:
	ID3D11Device* _pDevice = nullptr;
	IDXGISwapChain* _pSwapChain = nullptr;
	ID3D11DeviceContext* _pContext = nullptr;
	ID3D11RenderTargetView* _pTarget = nullptr;
};

