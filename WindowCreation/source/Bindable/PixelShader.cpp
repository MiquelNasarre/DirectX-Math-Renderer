#include "Bindable/PixelShader.h"
#include "WinHeader.h"
#include "Graphics.h"
#include "Exception/_exGraphics.h"

#define _device ((ID3D11Device*)device())
#define _context ((ID3D11DeviceContext*)context())

#include <d3dcompiler.h>

struct PixelShaderInternals
{
	ComPtr<ID3D11PixelShader> pPixelShader;
};

PixelShader::PixelShader(const wchar_t* path)
{
	BindableData = new PixelShaderInternals;
	PixelShaderInternals& data = *(PixelShaderInternals*)BindableData;

	ComPtr<ID3DBlob> pBlob;
	GFX_THROW_INFO(D3DReadFileToBlob(path, &pBlob));
	GFX_THROW_INFO(_device->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, data.pPixelShader.GetAddressOf()));
}

PixelShader::~PixelShader()
{
	delete (PixelShaderInternals*)BindableData;
}

void PixelShader::Bind()
{
	PixelShaderInternals& data = *(PixelShaderInternals*)BindableData;

	GFX_THROW_INFO_ONLY(_context->PSSetShader(data.pPixelShader.Get(), NULL, 0u));
}
