#include "Bindable/VertexBuffer.h"
#include "WinHeader.h"
#include "Graphics.h"
#include "Exception/_exGraphics.h"

#define _device ((ID3D11Device*)device())
#define _context ((ID3D11DeviceContext*)context())

struct VertexBufferInternals
{
	UINT stride;
	ComPtr<ID3D11Buffer> pVertexBuffer;
};

VertexBuffer::~VertexBuffer()
{
	delete (VertexBufferInternals*)BindableData;
}

void VertexBuffer::Bind()
{
	VertexBufferInternals& data = *(VertexBufferInternals*)BindableData;

	const UINT offset = 0u;
	GFX_THROW_INFO_ONLY(_context->IASetVertexBuffers(0u, 1u, data.pVertexBuffer.GetAddressOf(), &data.stride, &offset));
}

void VertexBuffer::_internal_constructor(void* vertices, unsigned stride, unsigned size)
{
	BindableData = new VertexBufferInternals;
	VertexBufferInternals& data = *(VertexBufferInternals*)BindableData;
	data.stride = stride;

	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = stride * size;
	bd.StructureByteStride = stride;
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertices;

	GFX_THROW_INFO(_device->CreateBuffer(&bd, &sd, data.pVertexBuffer.GetAddressOf()));
}
