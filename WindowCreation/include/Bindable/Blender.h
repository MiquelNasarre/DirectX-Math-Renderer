#pragma once
#include "Bindable.h"

class Blender : public Bindable
{
public:
	Blender(bool blending, float* factors = nullptr);
	~Blender() override;
	void Bind() override;

private:
	void* BindableData = nullptr;
};