#include "VertexShader.h"
#include "GraphicsMacros.h"

VertexShader::VertexShader(Graphics& graphics, const std::wstring& path)
{
	GRAPHICSINFO(graphics);

	GRAPHICS_THROW_INFO(D3DReadFileToBlob(path.c_str(), &_pBytecodeBlob));
	GRAPHICS_THROW_INFO(GetDevice(graphics)->CreateVertexShader(_pBytecodeBlob->GetBufferPointer(), _pBytecodeBlob->GetBufferSize(), nullptr, &_pVertexShader));
}

void VertexShader::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->VSSetShader(_pVertexShader.Get(), nullptr, 0u);
}

ID3DBlob* VertexShader::GetByteCode() const noexcept
{
	return _pBytecodeBlob.Get();
}
