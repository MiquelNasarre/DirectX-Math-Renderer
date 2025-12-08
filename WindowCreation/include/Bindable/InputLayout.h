#pragma once
#include "Bindable.h"

class InputLayout : public Bindable
{
public:
	InputLayout(const void* layout, unsigned size, void* pVertexShaderBytecode);
	~InputLayout() override;
	void Bind() override;
private:
	void* BindableData = nullptr;
};