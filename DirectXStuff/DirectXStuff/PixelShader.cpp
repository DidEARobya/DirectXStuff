#include "PixelShader.h"
#include "GraphicsMacros.h"

PixelShader::PixelShader(Graphics& graphics, const std::wstring& path)
{
	GRAPHICSINFO(graphics);

	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	GRAPHICS_THROW_INFO(D3DReadFileToBlob(path.c_str(), &pBlob));
	GRAPHICS_THROW_INFO(GetDevice(graphics)->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &_pPixelShader));
}

void PixelShader::Bind(Graphics& graphics) noexcept
{
	GetContext(graphics)->PSSetShader(_pPixelShader.Get(), nullptr, 0u);
}