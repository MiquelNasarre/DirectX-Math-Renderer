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
	ComPtr<ID3D11Texture2D> pTexture;
	ComPtr<ID3D11ShaderResourceView> pTextureView;
	Vector2i dimensions;
	unsigned slot;
	TEXTURE_USAGE usage;
};

/*
--------------------------------------------------------------------------------------------
 Texture Functions
--------------------------------------------------------------------------------------------
*/

// Takes the Images reference and creates the texture in the GPU.

Texture::Texture(Image* image, TEXTURE_USAGE usage, unsigned slot)
{
	if (!image)
		throw INFO_EXCEPT("Found nullptr when expecting an Image to create a Texture.");

	BindableData = new TextureInternals;
	TextureInternals& data = *(TextureInternals*)BindableData;
	data.slot = slot;
	data.usage = usage;
	data.dimensions = { image->width(), image->height() };

	//	Create texture resource

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width				= image->width();
	textureDesc.Height				= image->height();
	textureDesc.Usage				= (usage == TEXTURE_USAGE_DYNAMIC) ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	textureDesc.CPUAccessFlags		= (usage == TEXTURE_USAGE_DYNAMIC) ? D3D11_CPU_ACCESS_WRITE	: 0u;
	textureDesc.MipLevels			= 1u;
	textureDesc.ArraySize			= 1u;
	textureDesc.Format				= DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count	= 1u;
	textureDesc.SampleDesc.Quality	= 0u;
	textureDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
	textureDesc.MiscFlags			= 0u;
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem		= image->pixels();
	sd.SysMemPitch	= image->width() * sizeof(Color);

	GFX_THROW_INFO(_device->CreateTexture2D(&textureDesc, &sd, data.pTexture.GetAddressOf()));

	//	Create the resource view on the texture

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0u;
	srvDesc.Texture2D.MipLevels = 1u;

	GFX_THROW_INFO(_device->CreateShaderResourceView(data.pTexture.Get(), &srvDesc, data.pTextureView.GetAddressOf()));
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

	GFX_THROW_INFO_ONLY(_context->PSSetShaderResources(data.slot, 1u, data.pTextureView.GetAddressOf()));
}

// Sets the slot at which the Texture will be bound.

void Texture::setSlot(unsigned slot)
{
	TextureInternals& data = *(TextureInternals*)BindableData;

	data.slot = slot;
}

// If usage is dynamic updates the texture with the new image.
// Dimensions must match the initial image dimensions.

void Texture::update(Image* image)
{
	if (!image)
		throw INFO_EXCEPT("Found nullptr when expecting an Image to update a Texture.");

	TextureInternals& data = *(TextureInternals*)BindableData;

	if (data.usage != TEXTURE_USAGE_DYNAMIC)
		throw INFO_EXCEPT(
			"Trying to update a texture without dynamic usage.\n"
			"To use the update function on a Texture you should set TEXTURE_USAGE_DYNAMIC on the constructor."
		);

	if (data.dimensions != Vector2i{ image->width(), image->height() })
		throw INFO_EXCEPT("Trying to update a texture with an image of different dimensions to the one used in the constructor.");

	// Create the mapping
	D3D11_MAPPED_SUBRESOURCE msr;
	GFX_THROW_INFO(_context->Map(data.pTexture.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr));

	// Copy the new image pixels
	const unsigned rowBytes = image->width() * sizeof(Color);
	for (unsigned y = 0; y < image->height(); y++)
		memcpy((uint8_t*)msr.pData + y * msr.RowPitch, (uint8_t*)image->pixels() + y * rowBytes, rowBytes);

	// Unmap the data
	GFX_THROW_INFO_ONLY(_context->Unmap(data.pTexture.Get(), 0u));
}
