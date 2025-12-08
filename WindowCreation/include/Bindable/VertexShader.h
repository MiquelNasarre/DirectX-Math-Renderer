#pragma once
#include "Bindable.h"

class VertexShader : public Bindable
{
public:
	VertexShader(const wchar_t* path);
	~VertexShader();
	void Bind() override;
	void* GetBytecode() const noexcept;
private:
	void* BindableData = nullptr;
};