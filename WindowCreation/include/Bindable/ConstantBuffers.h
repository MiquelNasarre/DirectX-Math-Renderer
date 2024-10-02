#pragma once
#include "Bindable.h"

enum TYPE_ConstBuffer {
 VERTEX_CONSTANT_BUFFER_TYPE,
 PIXEL_CONSTANT_BUFFER_TYPE
};


#define CONSTANT_BUFFER_DEFAULT_SLOT -1

template<typename C>
class ConstantBuffer : public Bindable
{
public:
	void Update(Graphics& gfx, const C& consts);
	void Update(Graphics& gfx, const C* consts);
	ConstantBuffer(Graphics& gfx, const C& consts, TYPE_ConstBuffer type, const int slot = CONSTANT_BUFFER_DEFAULT_SLOT);
	ConstantBuffer(Graphics& gfx, TYPE_ConstBuffer type, const int slot = CONSTANT_BUFFER_DEFAULT_SLOT);
	void Bind(Graphics& gfx) override;

private:
	pCom<ID3D11Buffer> pConstantBuffer;
	TYPE_ConstBuffer Type;
	UINT Slot;
};

// Function Implementations

template<typename C>
void ConstantBuffer<C>::Update(Graphics& gfx, const C& consts)
{
#ifdef _DEBUG
	DxgiInfoManager& infoManager = GetInfoManager((gfx));
	infoManager.Set();
	GetContext(gfx)->UpdateSubresource(pConstantBuffer.Get(), 0u, NULL, &consts, 0u, 0u);
	auto v = infoManager.GetMessages();
	if (v[0])
		throw Graphics::InfoException(__LINE__, __FILE__, v);
	else free(v);
#else
	GetContext(gfx)->UpdateSubresource(pConstantBuffer.Get(), 0u, NULL, &consts, 0u, 0u);
#endif
}

template<typename C>
void ConstantBuffer<C>::Update(Graphics& gfx, const C* consts)
{
#ifdef _DEBUG
	DxgiInfoManager& infoManager = GetInfoManager((gfx));
	infoManager.Set();
	GetContext(gfx)->UpdateSubresource(pConstantBuffer.Get(), 0u, NULL, consts, 0u, 0u);
	auto v = infoManager.GetMessages();
	if (v[0])
		throw Graphics::InfoException(__LINE__, __FILE__, v);
	else free(v);
#else
	GetContext(gfx)->UpdateSubresource(pConstantBuffer.Get(), 0u, NULL, consts, 0u, 0u);
#endif
}

template<typename C>
ConstantBuffer<C>::ConstantBuffer(Graphics& gfx, const C& consts, TYPE_ConstBuffer type, const int slot)
	:Type{ type }
{
	if (slot == CONSTANT_BUFFER_DEFAULT_SLOT) {
		if (type == VERTEX_CONSTANT_BUFFER_TYPE)
			Slot = 1u;
		if (type == PIXEL_CONSTANT_BUFFER_TYPE)
			Slot = 0u;
	}
	else
		Slot = (UINT)slot;

	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.CPUAccessFlags = 0u;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(consts);
	cbd.StructureByteStride = 0u;

	D3D11_SUBRESOURCE_DATA csd = {};
	csd.pSysMem = &consts;
	
#ifdef _DEBUG
	DxgiInfoManager& infoManager = GetInfoManager((gfx));
	infoManager.Set();
	GetDevice(gfx)->CreateBuffer(&cbd, &csd, &pConstantBuffer);
	auto v = infoManager.GetMessages();
	if (v[0])
		throw Graphics::InfoException(__LINE__, __FILE__, v);
	else free(v);
#else
	GetDevice(gfx)->CreateBuffer(&cbd, &csd, &pConstantBuffer);
#endif
}

template<typename C>
ConstantBuffer<C>::ConstantBuffer(Graphics& gfx, TYPE_ConstBuffer type, const int slot)
	:Type{ type }
{
	if (slot == CONSTANT_BUFFER_DEFAULT_SLOT) {
		if (type == VERTEX_CONSTANT_BUFFER_TYPE)
			Slot = 1u;
		if (type == PIXEL_CONSTANT_BUFFER_TYPE)
			Slot = 0u;
	}
	else
		Slot = (UINT)slot;

	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.CPUAccessFlags = 0u;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(C);
	cbd.StructureByteStride = 0u;

#ifdef _DEBUG
	DxgiInfoManager& infoManager = GetInfoManager((gfx));
	infoManager.Set();
	GetDevice(gfx)->CreateBuffer(&cbd, NULL, &pConstantBuffer);
	auto v = infoManager.GetMessages();
	if (v[0])
		throw Graphics::InfoException(__LINE__, __FILE__, v);
	else free(v);
#else
	GetDevice(gfx)->CreateBuffer(&cbd, NULL, &pConstantBuffer);
#endif
}

template<typename C>
void ConstantBuffer<C>::Bind(Graphics& gfx)
{
#ifdef _DEBUG
	DxgiInfoManager& infoManager = GetInfoManager((gfx));
	infoManager.Set();

	if (Type == VERTEX_CONSTANT_BUFFER_TYPE)
		GetContext(gfx)->VSSetConstantBuffers(Slot, 1u, pConstantBuffer.GetAddressOf());
	else if (Type == PIXEL_CONSTANT_BUFFER_TYPE)
		GetContext(gfx)->PSSetConstantBuffers(Slot, 1u, pConstantBuffer.GetAddressOf());

	auto v = infoManager.GetMessages();
	if (v[0])
		throw Graphics::InfoException(__LINE__, __FILE__, v);
	else free(v);
#else
	if (Type == VERTEX_CONSTANT_BUFFER_TYPE)
		GetContext(gfx)->VSSetConstantBuffers(Slot, 1u, pConstantBuffer.GetAddressOf());
	else if (Type == PIXEL_CONSTANT_BUFFER_TYPE)
		GetContext(gfx)->PSSetConstantBuffers(Slot, 1u, pConstantBuffer.GetAddressOf());
#endif
}
