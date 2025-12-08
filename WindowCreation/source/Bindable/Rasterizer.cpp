#include "Bindable/Rasterizer.h"
#include "WinHeader.h"
#include "Graphics.h"
#include "Exception/_exGraphics.h"

#define _device ((ID3D11Device*)device())
#define _context ((ID3D11DeviceContext*)context())

struct RasterizerInternals
{
	ComPtr<ID3D11RasterizerState> pRasterizer;
};

Rasterizer::Rasterizer(bool doubleSided)
{
	BindableData = new RasterizerInternals;
	RasterizerInternals& data = *(RasterizerInternals*)BindableData;

	D3D11_RASTERIZER_DESC rasterDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
	rasterDesc.CullMode = doubleSided ? D3D11_CULL_NONE : D3D11_CULL_BACK;

	GFX_THROW_INFO(_device->CreateRasterizerState(&rasterDesc, data.pRasterizer.GetAddressOf()));
}

Rasterizer::~Rasterizer()
{
	delete (RasterizerInternals*)BindableData;
}

void Rasterizer::Bind()
{
	RasterizerInternals& data = *(RasterizerInternals*)BindableData;

	GFX_THROW_INFO_ONLY(_context->RSSetState(data.pRasterizer.Get()));
}
