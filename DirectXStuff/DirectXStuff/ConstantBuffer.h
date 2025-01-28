#pragma once
#include "Bindable.h"
#include "GraphicsMacros.h"

template<typename C>
class ConstantBuffer : public Bindable
{
public:
	void Update(Graphics& graphics, const C& consts)
	{
		GRAPHICSINFO(graphics);

		//Map the subresource
		D3D11_MAPPED_SUBRESOURCE mappedSubResource;
		GRAPHICS_THROW_INFO(GetContext(graphics)->Map(pConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedSubResource));

		//Copy the subresource to memory
		memcpy(mappedSubResource.pData, &consts, sizeof(consts));
		//Unmap subresource
		GetContext(graphics)->Unmap(pConstantBuffer.Get(), 0u);
	}

	ConstantBuffer(Graphics& graphics, const C& consts)
	{
		GRAPHICSINFO(graphics);

		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(consts);
		cbd.StructureByteStride = 0u;

		D3D11_SUBRESOURCE_DATA csd = {};
		csd.pSysMem = &consts;

		GRAPHICS_THROW_INFO(GetDevice(graphics)->CreateBuffer(&cbd, &csd, &pConstantBuffer));
	}

	ConstantBuffer(Graphics& graphics)
	{
		GRAPHICSINFO(graphics);

		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(C);
		cbd.StructureByteStride = 0u;

		GRAPHICS_THROW_INFO(GetDevice(graphics)->CreateBuffer(&cbd, nullptr, &pConstantBuffer));
	}

protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
};

template<typename C>
class VertexConstantBuffer : public ConstantBuffer<C>
{
	using ConstantBuffer<C>::pConstantBuffer;
	using Bindable::GetContext;

public:
	using ConstantBuffer<C>::ConstantBuffer;

	void Bind(Graphics& graphics) noexcept override
	{
		GetContext(graphics)->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());
	}
};

template<typename C>
class PixelConstantBuffer : public ConstantBuffer<C>
{
	using ConstantBuffer<C>::pConstantBuffer;
	using Bindable::GetContext;

public:
	using ConstantBuffer<C>::ConstantBuffer;

	void Bind(Graphics& graphics) noexcept override
	{
		GetContext(graphics)->PSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());
	}
};
