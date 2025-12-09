#include "Bindable/Sampler.h"

#include "WinHeader.h"
#include "Exception/_exGraphics.h"

// Handy helper pointers to the device and context.
#define _device ((ID3D11Device*)device())
#define _context ((ID3D11DeviceContext*)context())

// Structure to manage the Sampler internal data.
struct SamplerInternals
{
	ComPtr<ID3D11SamplerState> pSampler;
};

/*
--------------------------------------------------------------------------------------------
 Sampler Functions
--------------------------------------------------------------------------------------------
*/

// Creates the Sampler object and GPU pointer as specified in the arguments.

Sampler::Sampler(SAMPLE_FILTER filter, SAMPLE_ADDRESS_MODE address_mode)
{
	BindableData = new SamplerInternals;
	SamplerInternals& data = *(SamplerInternals*)BindableData;

	// Creates the sampler descriptor from the given arguments.
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = (D3D11_FILTER)filter;
	if (filter == SAMPLE_FILTER_ANISOTROPIC)
		samplerDesc.MaxAnisotropy = 8U;
	samplerDesc.AddressU = (D3D11_TEXTURE_ADDRESS_MODE)address_mode;
	samplerDesc.AddressV = (D3D11_TEXTURE_ADDRESS_MODE)address_mode;
	samplerDesc.AddressW = (D3D11_TEXTURE_ADDRESS_MODE)address_mode;

	// Creates the sampler object on the GPU and stores the pointer.
	GFX_THROW_INFO(_device->CreateSamplerState(&samplerDesc, data.pSampler.GetAddressOf()));
}

// Releases the GPU pointer and deletes the data.

Sampler::~Sampler()
{
	delete (SamplerInternals*)BindableData;
}

// Binds the Sampler to the global context.

void Sampler::Bind()
{
	SamplerInternals& data = *(SamplerInternals*)BindableData;

	GFX_THROW_INFO_ONLY(_context->PSSetSamplers(0, 1, data.pSampler.GetAddressOf()));
}