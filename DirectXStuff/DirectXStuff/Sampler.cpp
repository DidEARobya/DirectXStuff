#include "Sampler.h"
#include "GraphicsMacros.h"

Sampler::Sampler(Graphics& graphics)
{
	GRAPHICSINFO(graphics);

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	GRAPHICS_THROW_INFO(GetDevice(graphics)->CreateSamplerState(&samplerDesc, &pSamplerState));
}

void Sampler::Bind(Graphics& graphics) noexcept
{
	GetContext(graphics)->PSSetSamplers(0u, 1u, pSamplerState.GetAddressOf());
}
