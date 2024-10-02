#pragma once
#include "Bindable.h"

class InputLayout : public Bindable
{
public:
	InputLayout(Graphics& gfx, const D3D11_INPUT_ELEMENT_DESC* layout, UINT size, ID3DBlob* pVertexShaderBytecode);
	void Bind(Graphics& gfx) override;
private:
	pCom<ID3D11InputLayout> pInputLayout;
};