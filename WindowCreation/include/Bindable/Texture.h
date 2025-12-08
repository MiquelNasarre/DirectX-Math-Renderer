#pragma once
#include "Bindable.h"

class Texture : public Bindable
{
public:
	Texture(Image& image, unsigned slot = 0u);
	~Texture() override;

	void Bind() override;
	void setSlot(unsigned slot);
private:
	void* BindableData = nullptr;
};
