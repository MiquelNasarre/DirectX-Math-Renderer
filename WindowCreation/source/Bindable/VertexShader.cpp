#include "Bindable/VertexShader.h"
#include "Exception/ExceptionMacros.h"

#include <d3dcompiler.h>

VertexShader::VertexShader(Graphics& gfx, const wchar_t* path)
{
	INFOMAN(gfx);

	GFX_THROW_INFO( D3DReadFileToBlob(path,&pBytecodeBlob));
	GFX_THROW_INFO( GetDevice(gfx)->CreateVertexShader( 
		pBytecodeBlob->GetBufferPointer(),
		pBytecodeBlob->GetBufferSize(),
		NULL,
		&pVertexShader 
	) );
}

void VertexShader::Bind(Graphics& gfx)
{
	INFOMAN(gfx);
	GFX_THROW_INFO_ONLY(GetContext(gfx)->VSSetShader(pVertexShader.Get(), NULL, 0u));
}

ID3DBlob* VertexShader::GetBytecode() const noexcept
{
	return pBytecodeBlob.Get();
}
