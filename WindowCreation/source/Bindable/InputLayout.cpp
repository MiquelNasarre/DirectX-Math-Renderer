#include "Bindable/InputLayout.h"
#include "WinHeader.h"
#include "Graphics.h"
#include "Exception/_exGraphics.h"

#define _device ((ID3D11Device*)device())
#define _context ((ID3D11DeviceContext*)context())

struct InputLayoutInternals
{
	ComPtr<ID3D11InputLayout> pInputLayout;
};

InputLayout::InputLayout(const void* layout, unsigned size, void* pVertexShaderBytecode)
{
	BindableData = new InputLayoutInternals;
	InputLayoutInternals& data = *(InputLayoutInternals*)BindableData;

	GFX_THROW_INFO(_device->CreateInputLayout(
		(D3D11_INPUT_ELEMENT_DESC*)layout, size,
		((ID3DBlob*)pVertexShaderBytecode)->GetBufferPointer(),
		((ID3DBlob*)pVertexShaderBytecode)->GetBufferSize(),
		data.pInputLayout.GetAddressOf()
	));
}

InputLayout::~InputLayout()
{
	delete (InputLayoutInternals*)BindableData;
}

void InputLayout::Bind()
{
	InputLayoutInternals& data = *(InputLayoutInternals*)BindableData;

	GFX_THROW_INFO_ONLY(_context->IASetInputLayout(data.pInputLayout.Get()));
}
