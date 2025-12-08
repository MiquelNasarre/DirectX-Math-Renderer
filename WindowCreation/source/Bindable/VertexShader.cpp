#include "Bindable/VertexShader.h"
#include "WinHeader.h"
#include "Graphics.h"
#include "Exception/_exGraphics.h"

#define _device ((ID3D11Device*)device())
#define _context ((ID3D11DeviceContext*)context())

#include <d3dcompiler.h>

struct VertexShaderInternals
{
	ComPtr<ID3DBlob>			pBytecodeBlob;
	ComPtr<ID3D11VertexShader>	pVertexShader;
};

VertexShader::VertexShader(const wchar_t* path)
{
	BindableData = new VertexShaderInternals;
	VertexShaderInternals& data = *(VertexShaderInternals*)BindableData;

	GFX_THROW_INFO( D3DReadFileToBlob(path,data.pBytecodeBlob.GetAddressOf()));
	GFX_THROW_INFO( _device->CreateVertexShader( 
		data.pBytecodeBlob->GetBufferPointer(),
		data.pBytecodeBlob->GetBufferSize(),
		NULL,
		data.pVertexShader.GetAddressOf()
	) );
}

VertexShader::~VertexShader()
{
	delete (VertexShaderInternals*)BindableData;
}

void VertexShader::Bind()
{
	VertexShaderInternals& data = *(VertexShaderInternals*)BindableData;

	GFX_THROW_INFO_ONLY(_context->VSSetShader(data.pVertexShader.Get(), NULL, 0u));
}

void* VertexShader::GetBytecode() const noexcept
{
	VertexShaderInternals& data = *(VertexShaderInternals*)BindableData;

	return (void*)data.pBytecodeBlob.Get();
}
