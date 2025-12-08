#include "Bindable/Sampler.h"
#include "WinHeader.h"
#include "Graphics.h"
#include "Exception/_exGraphics.h"

#define _device ((ID3D11Device*)device())
#define _context ((ID3D11DeviceContext*)context())

struct SamplerInternals
{
	ComPtr<ID3D11SamplerState> pSampler;
};

Sampler::Sampler(unsigned filter)
{
	BindableData = new SamplerInternals;
	SamplerInternals& data = *(SamplerInternals*)BindableData;

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = (D3D11_FILTER)filter;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	GFX_THROW_INFO(_device->CreateSamplerState(&samplerDesc, data.pSampler.GetAddressOf()));
}

Sampler::~Sampler()
{
	delete (SamplerInternals*)BindableData;
}

void Sampler::Bind()
{
	SamplerInternals& data = *(SamplerInternals*)BindableData;

	GFX_THROW_INFO_ONLY(_context->PSSetSamplers(0, 1, data.pSampler.GetAddressOf()));
}