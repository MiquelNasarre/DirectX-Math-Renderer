#include "Bindable/Texture.h"
#include "WinHeader.h"
#include "Graphics.h"
#include "Exception/_exGraphics.h"

// Handy helper pointers to the device and context.
#define _device ((ID3D11Device*)device())
#define _context ((ID3D11DeviceContext*)context())

// Structure to manage the Texture internal data.
struct TextureInternals
{
	unsigned Slot;
	ComPtr<ID3D11ShaderResourceView> pTextureView;
};

/*
--------------------------------------------------------------------------------------------
 Texture Functions
--------------------------------------------------------------------------------------------
*/

// Takes the Images reference and creates the texture in the GPU.

Texture::Texture(Image& image, unsigned slot)
{
	BindableData = new TextureInternals;
	TextureInternals& data = *(TextureInternals*)BindableData;
	data.Slot = slot;

	//	Create texture resource

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = image.width();
	textureDesc.Height = image.height();
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
	sd.pSysMem = image.pixels();
	sd.SysMemPitch = image.width() * 4u;

	ComPtr<ID3D11Texture2D> pTexture;
	GFX_THROW_INFO(_device->CreateTexture2D(&textureDesc, &sd, &pTexture));

	//	Create the resource view on the texture

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0u;
	srvDesc.Texture2D.MipLevels = 1u;

	GFX_THROW_INFO(_device->CreateShaderResourceView(pTexture.Get(), &srvDesc, data.pTextureView.GetAddressOf()));
}

// Releases the GPU pointer and deletes the data.

Texture::~Texture()
{
	delete (TextureInternals*)BindableData;
}

// Binds the Texture to the global context at the specified slot.

void Texture::Bind()
{
	TextureInternals& data = *(TextureInternals*)BindableData;

	GFX_THROW_INFO_ONLY(_context->PSSetShaderResources(data.Slot, 1u, data.pTextureView.GetAddressOf()));
}

// Sets the slot at which the Texture will be bound.

void Texture::setSlot(unsigned slot)
{
	TextureInternals& data = *(TextureInternals*)BindableData;

	data.Slot = slot;
}
