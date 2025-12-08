#pragma once
#include "Bindable.h"

class Rasterizer : public Bindable
{
public:
	Rasterizer(bool doubleSided);
	~Rasterizer() override;

	void Bind() override;
private:
	void* BindableData = nullptr;
};