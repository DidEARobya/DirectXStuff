#include "Texture.h"
#include "Surface.h"
#include "GraphicsMacros.h"

namespace wrl = Microsoft::WRL;

Texture::Texture(Graphics& graphics, const Surface& surface)
{
	GRAPHICSINFO(graphics);

	//Create Texture
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = surface.GetWidth();
	textureDesc.Height = surface.GetHeight();
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pSysMem = surface.GetBufferPtrConst();
	subResourceData.SysMemPitch = surface.GetWidth() * sizeof(Surface::Colour);
	wrl::ComPtr<ID3D11Texture2D> pTexture;

	GRAPHICS_THROW_INFO(GetDevice(graphics)->CreateTexture2D(&textureDesc, &subResourceData, &pTexture));

	//Create view for texture
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	GRAPHICS_THROW_INFO(GetDevice(graphics)->CreateShaderResourceView(pTexture.Get(), &srvDesc, &pTextureView));
}

void Texture::Bind(Graphics& graphics) noexcept
{
	GetContext(graphics)->PSSetShaderResources(0u, 1u, pTextureView.GetAddressOf());
}
