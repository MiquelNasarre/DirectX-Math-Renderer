#pragma once
#include "Bindable.h"

class IndexBuffer : public Bindable
{
public:
	IndexBuffer(unsigned* indices, unsigned size);
	~IndexBuffer() override;

	void Bind() override;
	unsigned GetCount() const noexcept;

private:
	void* BindableData = nullptr;
};