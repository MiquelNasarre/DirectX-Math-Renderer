#include "Bindable/PixelShader.h"
#include "Exception/ExceptionMacros.h"

#include <d3dcompiler.h>

PixelShader::PixelShader(Graphics& gfx, const wchar_t* path)
{
	INFOMAN(gfx);

	pCom<ID3DBlob> pBlob;
	GFX_THROW_INFO(D3DReadFileToBlob(path, &pBlob));
	GFX_THROW_INFO(GetDevice(gfx)->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pPixelShader));
}

void PixelShader::Bind(Graphics& gfx)
{
	INFOMAN(gfx);
	GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetShader(pPixelShader.Get(), NULL, 0u));
}
