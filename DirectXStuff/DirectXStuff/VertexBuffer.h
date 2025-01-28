#pragma once
#include "Bindable.h"
#include "GraphicsMacros.h"

class VertexBuffer : public Bindable
{
public:
	template<class V> 
	VertexBuffer(Graphics& graphics, const std::vector<V>& vertices) : _stride(sizeof(V))
	{
		GRAPHICSINFO(graphics);

		D3D11_BUFFER_DESC bufferDescriptor = {};
		bufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
		bufferDescriptor.CPUAccessFlags = 0u;
		bufferDescriptor.MiscFlags = 0u;
		bufferDescriptor.ByteWidth = UINT(sizeof(V) * vertices.size());
		bufferDescriptor.StructureByteStride = sizeof(V);

		D3D11_SUBRESOURCE_DATA subData = {};
		subData.pSysMem = vertices.data();

		GRAPHICS_THROW_INFO(GetDevice(graphics)->CreateBuffer(&bufferDescriptor, &subData, &_pVertexBuffer));
	}

	void Bind(Graphics& graphics) noexcept override;
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> _pVertexBuffer;
	UINT _stride;
};
