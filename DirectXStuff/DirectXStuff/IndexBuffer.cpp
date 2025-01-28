#include "IndexBuffer.h"
#include "GraphicsMacros.h"

IndexBuffer::IndexBuffer(Graphics& graphics, const std::vector<unsigned short>& indices) : _count((UINT)indices.size())
{
	GRAPHICSINFO(graphics);

	D3D11_BUFFER_DESC bufferDescriptor = {};
	bufferDescriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
	bufferDescriptor.CPUAccessFlags = 0u;
	bufferDescriptor.MiscFlags = 0u;
	bufferDescriptor.ByteWidth = UINT(_count * sizeof(unsigned short));
	bufferDescriptor.StructureByteStride = sizeof(unsigned short);

	D3D11_SUBRESOURCE_DATA subData = {};
	subData.pSysMem = indices.data();

	GRAPHICS_THROW_INFO(GetDevice(graphics)->CreateBuffer(&bufferDescriptor, &subData, &_pIndexBuffer));
}

void IndexBuffer::Bind(Graphics& graphics) noexcept
{
	GetContext(graphics)->IASetIndexBuffer(_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
}

UINT IndexBuffer::GetCount() const noexcept
{
	return _count;
}
