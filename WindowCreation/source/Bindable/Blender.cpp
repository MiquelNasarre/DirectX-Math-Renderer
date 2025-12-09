#include "Bindable/Blender.h"

#include "WinHeader.h"
#include "Exception/_exGraphics.h"

// Handy helper pointers to the device and context.
#define _device ((ID3D11Device*)device())
#define _context ((ID3D11DeviceContext*)context())

// Structure to manage the Blender internal data.
struct BlenderInternals
{
	ComPtr<ID3D11BlendState> pBlender;
	float* factors;
};

/*
--------------------------------------------------------------------------------------------
 Blender Functions
--------------------------------------------------------------------------------------------
*/

// Takes the blending mode as an input and creates the blending state accordingly.

Blender::Blender(BLEND_MODE mode)
{
	BindableData = new BlenderInternals;
	BlenderInternals& data = *(BlenderInternals*)BindableData;
	data.factors = factors;

	D3D11_BLEND_DESC blendDesc = {};
	auto& brt = blendDesc.RenderTarget[0];
	if (blending)
	{
		brt.BlendEnable = false;
		brt.SrcBlend = D3D11_BLEND_ONE;
		brt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		brt.BlendOp = D3D11_BLEND_OP_ADD;
		brt.SrcBlendAlpha = D3D11_BLEND_ZERO;
		brt.DestBlendAlpha = D3D11_BLEND_ZERO;
		brt.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		brt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}
	else
	{
		brt.BlendEnable = false;
		brt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}

	GFX_THROW_INFO(_device->CreateBlendState(&blendDesc, data.pBlender.GetAddressOf()));
}

// Releases the GPU pointer and deletes the data.

Blender::~Blender()
{
	delete (BlenderInternals*)BindableData;
}

// Binds the Blender to the global context.

void Blender::Bind()
{
	BlenderInternals& data = *(BlenderInternals*)BindableData;

	GFX_THROW_INFO_ONLY(_context->OMSetBlendState(data.pBlender.Get(), data.factors, 0xFFFFFFFFu));
}
