#pragma once
#include "Bindable.h"

class IndexBuffer : public Bindable
{
public:
	IndexBuffer(Graphics& gfx, unsigned int* indices, UINT size);
	void Bind(Graphics& gfx) override;
	UINT GetCount() const noexcept;

	void Deletion() override;

private:
	UINT count;
	pCom<ID3D11Buffer> pIndexBuffer;
};