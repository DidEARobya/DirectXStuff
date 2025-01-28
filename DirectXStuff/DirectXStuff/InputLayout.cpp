#include "InputLayout.h"
#include "GraphicsMacros.h"

InputLayout::InputLayout(Graphics& graphics,const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,ID3DBlob* pVertexShaderBytecode)
{
	GRAPHICSINFO(graphics);

	GRAPHICS_THROW_INFO(GetDevice(graphics)->CreateInputLayout(layout.data(), (UINT)layout.size(),pVertexShaderBytecode->GetBufferPointer(),pVertexShaderBytecode->GetBufferSize(),&_pInputLayout));
}

void InputLayout::Bind(Graphics& graphics) noexcept
{
	GetContext(graphics)->IASetInputLayout(_pInputLayout.Get());
}