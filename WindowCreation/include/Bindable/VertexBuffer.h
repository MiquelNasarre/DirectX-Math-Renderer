#pragma once
#include "Bindable.h"

class VertexBuffer : public Bindable
{
public:

	template<typename V>
	VertexBuffer(V* vertices, unsigned size)
	{
		_internal_constructor((void*)vertices, sizeof(V), size);
	}

	~VertexBuffer() override;

	void Bind() override;

private:
	void _internal_constructor(void* vertices, unsigned stride, unsigned size);
	void* BindableData = nullptr;
};
