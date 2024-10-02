#pragma once
#include "Bindable.h"

class Texture : public Bindable
{
public:
	Texture(Graphics& gfx, const char* filename, UINT slot = 0u);
	Texture(Graphics& gfx, Image& image, UINT slot = 0u);
	Texture(const Texture& other, UINT slot = 0u);
	void Bind(Graphics& gfx) override;
	void setSlot(UINT slot);
private:
	UINT Slot;
	pCom<ID3D11ShaderResourceView> pTextureView;
};
