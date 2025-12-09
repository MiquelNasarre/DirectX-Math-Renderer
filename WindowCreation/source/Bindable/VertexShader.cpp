#include "Bindable/VertexShader.h"

#include "WinHeader.h"
#include "Exception/_exGraphics.h"

// Handy helper pointers to the device and context.
#define _device ((ID3D11Device*)device())
#define _context ((ID3D11DeviceContext*)context())

#include <d3dcompiler.h> // For D3DReadFileToBlob()

// Structure to manage the Vertex Shader internal data.
struct VertexShaderInternals
{
	ComPtr<ID3DBlob>			pBytecodeBlob;
	ComPtr<ID3D11VertexShader>	pVertexShader;
};

/*
--------------------------------------------------------------------------------------------
 Vertex Shader Functions
--------------------------------------------------------------------------------------------
*/

// Given a (*.cso) file path it creates the bytecode and the vertex shader object.

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

// Releases the pointers and deletes the internal data.

VertexShader::~VertexShader()
{
	delete (VertexShaderInternals*)BindableData;
}

// Binds the Vertex Shader to the global context.

void VertexShader::Bind()
{
	VertexShaderInternals& data = *(VertexShaderInternals*)BindableData;

	GFX_THROW_INFO_ONLY(_context->VSSetShader(data.pVertexShader.Get(), NULL, 0u));
}

// To be used by InputLayout. Returns the shader ID3DBlob* masked as a void*.

void* VertexShader::GetBytecode() const noexcept
{
	VertexShaderInternals& data = *(VertexShaderInternals*)BindableData;

	return (void*)data.pBytecodeBlob.Get();
}
