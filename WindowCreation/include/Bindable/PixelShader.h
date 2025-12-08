#pragma once
#include "Bindable.h"

class PixelShader : public Bindable
{
public:
	PixelShader(const wchar_t* path);
	~PixelShader() override;
	void Bind() override;
private:
	void* BindableData = nullptr;
};