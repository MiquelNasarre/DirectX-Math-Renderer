#pragma once
#include "Bindable.h"

/* TEXTURE BINDABLE CLASS
-------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------
A texture from a graphics engine point of view it is simply a grid of texels sent to
the GPU that the Pixel Shader can sample from and use it to output its pixel color.

For the Pixel Shader to sample from a texture a sampler state needs to be defined, refer
to the Sampler bindable header for more information on how to set a sampler state.

To be able to send your Images to the GPU this class handles the creation of textures and 
their GPU binding. To create a texture the Image class is to be used. Internally contains
a pointer to an array of colors and the image dimensions. It supports transparencies and 
image loading and saving from memory.

For more information on how to use the Image class I strongly suggest checking its header.
For more information on how textures are used in Pixel Shaders and DX11 in general you can 
check the microsoft learn documentation at:
https://learn.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-resources-textures-intro
-------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------
*/

// Texture bindable class, from an Image it creates a texture and sends it to the GPU to 
// be red by the Pixel Shader at the specified slot.
class Texture : public Bindable
{
public:
	// Takes the Images reference and creates the texture in the GPU.
	Texture(Image& image, unsigned slot = 0u);

	// Releases the GPU pointer and deletes the data.
	~Texture() override;

	// Binds the Texture to the global context at the specified slot.
	void Bind() override;

	// Sets the slot at which the Texture will be bound.
	void setSlot(unsigned slot);

private:
	// Pointer to the internal Texture data.
	void* BindableData = nullptr;
};
