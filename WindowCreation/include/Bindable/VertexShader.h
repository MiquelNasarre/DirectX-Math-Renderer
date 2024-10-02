#pragma once
#include "Bindable.h"

class VertexShader : public Bindable
{
public:
	VertexShader(Graphics& gfx,const wchar_t* path);
	void Bind(Graphics& gfx) override;
	ID3DBlob* GetBytecode() const noexcept;
private:
	pCom<ID3DBlob>				pBytecodeBlob;
	pCom<ID3D11VertexShader>	pVertexShader;
};