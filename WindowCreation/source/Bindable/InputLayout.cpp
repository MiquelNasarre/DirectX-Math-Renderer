#include "Bindable/InputLayout.h"
#include "Exception/ExceptionMacros.h"

InputLayout::InputLayout(Graphics& gfx, const D3D11_INPUT_ELEMENT_DESC* layout, UINT size, ID3DBlob* pVertexShaderBytecode)
{
	INFOMAN(gfx);

	GFX_THROW_INFO(GetDevice(gfx)->CreateInputLayout(
		layout, size,
		pVertexShaderBytecode->GetBufferPointer(),
		pVertexShaderBytecode->GetBufferSize(),
		&pInputLayout
	));
}

void InputLayout::Bind(Graphics& gfx)
{
	INFOMAN(gfx);
	GFX_THROW_INFO_ONLY(GetContext(gfx)->IASetInputLayout(pInputLayout.Get()));
}
