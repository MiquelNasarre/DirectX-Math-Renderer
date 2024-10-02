#include "Bindable/IndexBuffer.h"
#include "Exception/ExceptionMacros.h"

IndexBuffer::IndexBuffer(Graphics& gfx, unsigned int* indices, UINT size)
	: count{ size }
{
	INFOMAN(gfx);

	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = count * sizeof(unsigned int);
	ibd.StructureByteStride = sizeof(unsigned int);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices;
	GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&ibd, &isd, &pIndexBuffer));
}

void IndexBuffer::Bind(Graphics& gfx)
{
	INFOMAN(gfx);
	GFX_THROW_INFO_ONLY(GetContext(gfx)->IASetIndexBuffer(pIndexBuffer.Get(),DXGI_FORMAT_R32_UINT,0u));
}

UINT IndexBuffer::GetCount() const noexcept
{
	return count;
}

void IndexBuffer::Deletion()
{
	pIndexBuffer->Release();
}
