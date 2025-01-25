#pragma once
#include "WinInclude.h"
#include <wrl.h>
#include <vector>
#include <string>
#include <dxgidebug.h>

class DXGIInfoManager
{
public:
	DXGIInfoManager();
	DXGIInfoManager(const DXGIInfoManager&) = delete;
	DXGIInfoManager& operator= (const DXGIInfoManager&) = delete;
	~DXGIInfoManager() = default;

	void Set() noexcept;
	std::vector<std::string>GetMessages() const;

private:
	unsigned long long _next = 0u;
	Microsoft::WRL::ComPtr<IDXGIInfoQueue> _pDXGIInfoQueue = nullptr;
};

#define GRAPHICS_THROW_NOINFO(hrcall) if(FAILED(hResult = (hrcall))) throw Graphics::GraphicsException(__LINE__,__FILE__, hResult)
