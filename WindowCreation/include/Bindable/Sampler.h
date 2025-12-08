#pragma once
#include "Bindable.h"

class Sampler : public Bindable
{
public:
	Sampler(unsigned filter);
	~Sampler() override;
	void Bind() override;
private:
	void* BindableData = nullptr;
};
