#include "Bindable/Texture.h"
#include "Exception/ExceptionMacros.h"

Texture::Texture(Graphics& gfx, const char* filename, UINT slot) : Slot{ slot }
{
	INFOMAN(gfx);

	Image image(filename);

	//	Create texture resource

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = image.width;
	textureDesc.Height = image.height;
	textureDesc.MipLevels = 1u;
	textureDesc.ArraySize = 1u;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1u;
	textureDesc.SampleDesc.Quality = 0u;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0u;
	textureDesc.MiscFlags = 0u;
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = image.Pixels;
	sd.SysMemPitch = image.width * 4;

	pCom<ID3D11Texture2D> pTexture;
	GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&textureDesc, &sd, &pTexture));

	//	Create the resource view on the texture

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0u;
	srvDesc.Texture2D.MipLevels = 1u;

	GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(pTexture.Get(), &srvDesc, &pTextureView));
}

Texture::Texture(Graphics& gfx, Image& image, UINT slot)
{
	INFOMAN(gfx);

	//	Create texture resource

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = image.width;
	textureDesc.Height = image.height;
	textureDesc.MipLevels = 1u;
	textureDesc.ArraySize = 1u;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1u;
	textureDesc.SampleDesc.Quality = 0u;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0u;
	textureDesc.MiscFlags = 0u;
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = image.Pixels;
	sd.SysMemPitch = image.width * 4;

	pCom<ID3D11Texture2D> pTexture;
	GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&textureDesc, &sd, &pTexture));

	//	Create the resource view on the texture

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0u;
	srvDesc.Texture2D.MipLevels = 1u;

	GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(pTexture.Get(), &srvDesc, &pTextureView));
}

Texture::Texture(const Texture& other, UINT slot)
{
	pTextureView = other.pTextureView;
	Slot = slot;
}

void Texture::Bind(Graphics& gfx)
{
	INFOMAN(gfx);
	GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetShaderResources(Slot, 1u, pTextureView.GetAddressOf()));
}

void Texture::setSlot(UINT slot)
{
	Slot = slot;
}
