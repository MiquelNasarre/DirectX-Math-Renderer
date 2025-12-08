#include "Bindable/ConstantBuffers.h"
#include "WinHeader.h"
#include "Graphics.h"
#include "Exception/_exGraphics.h"

#define _device ((ID3D11Device*)device())
#define _context ((ID3D11DeviceContext*)context())

struct ConstantBufferInternals
{
	ComPtr<ID3D11Buffer> pConstantBuffer;
	TYPE_ConstBuffer Type;
	unsigned Slot;
	unsigned size;
};

ConstantBuffer::~ConstantBuffer()
{
	delete (ConstantBufferInternals*)BindableData;
}

void ConstantBuffer::Bind()
{
	ConstantBufferInternals& data = *(ConstantBufferInternals*)BindableData;

	if (data.Type == VERTEX_CONSTANT_BUFFER_TYPE)
		GFX_THROW_INFO_ONLY(_context->VSSetConstantBuffers(data.Slot, 1u, data.pConstantBuffer.GetAddressOf()))

	else if (data.Type == PIXEL_CONSTANT_BUFFER_TYPE)
		GFX_THROW_INFO_ONLY(_context->PSSetConstantBuffers(data.Slot, 1u, data.pConstantBuffer.GetAddressOf()))
}

void ConstantBuffer::_internal_constructor(const void* _data, unsigned size, TYPE_ConstBuffer type, const int slot)
{
	BindableData = new ConstantBufferInternals;
	ConstantBufferInternals& data = *(ConstantBufferInternals*)BindableData;

	data.size = size;
	data.Type = type;

	if (slot == CONSTANT_BUFFER_DEFAULT_SLOT) {
		if (type == VERTEX_CONSTANT_BUFFER_TYPE)
			data.Slot = 1u;
		if (type == PIXEL_CONSTANT_BUFFER_TYPE)
			data.Slot = 0u;
	}
	else
		data.Slot = (UINT)slot;

	D3D11_SUBRESOURCE_DATA csd = {};
	csd.pSysMem = _data;

	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.CPUAccessFlags = 0u;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = size;
	cbd.StructureByteStride = 0u;

	GFX_THROW_INFO(_device->CreateBuffer(&cbd, &csd, data.pConstantBuffer.GetAddressOf()));
}

void ConstantBuffer::_internal_update(const void* _data, unsigned size_check)
{
	ConstantBufferInternals& data = *(ConstantBufferInternals*)BindableData;

	if (data.size != size_check)
		throw INFO_EXCEPT("Missmatch in the stored data type and the update data type");

	GFX_THROW_INFO_ONLY(_context->UpdateSubresource(data.pConstantBuffer.Get(), 0u, NULL, _data, 0u, 0u));
}
