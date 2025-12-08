#include "Bindable/IndexBuffer.h"
#include "WinHeader.h"
#include "Graphics.h"
#include "Exception/_exGraphics.h"

#define _device ((ID3D11Device*)device())
#define _context ((ID3D11DeviceContext*)context())

struct IndexBufferInternals
{
	unsigned count;
	ComPtr<ID3D11Buffer> pIndexBuffer;
};

IndexBuffer::IndexBuffer(unsigned* indices, unsigned size)
{
	BindableData = new IndexBufferInternals;
	IndexBufferInternals& data = *(IndexBufferInternals*)BindableData;
	data.count = size;

	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = data.count * sizeof(unsigned);
	ibd.StructureByteStride = sizeof(unsigned);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices;
	GFX_THROW_INFO(_device->CreateBuffer(&ibd, &isd, data.pIndexBuffer.GetAddressOf()));
}

IndexBuffer::~IndexBuffer()
{
	delete (IndexBufferInternals*)BindableData;
}

void IndexBuffer::Bind()
{
	IndexBufferInternals& data = *(IndexBufferInternals*)BindableData;

	GFX_THROW_INFO_ONLY(_context->IASetIndexBuffer(data.pIndexBuffer.Get(),DXGI_FORMAT_R32_UINT,0u));
}

UINT IndexBuffer::GetCount() const noexcept
{
	IndexBufferInternals& data = *(IndexBufferInternals*)BindableData;

	return data.count;
}
