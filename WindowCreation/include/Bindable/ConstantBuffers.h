#pragma once
#include "Bindable.h"

enum TYPE_ConstBuffer {
 VERTEX_CONSTANT_BUFFER_TYPE,
 PIXEL_CONSTANT_BUFFER_TYPE
};

#define CONSTANT_BUFFER_DEFAULT_SLOT -1

class ConstantBuffer : public Bindable
{
public:
	template<typename C>
	void Update(const C& consts)
	{
		_internal_update((void*)&consts, sizeof(C));
	}
	template<typename C>
	void Update(const C* consts)
	{
		_internal_update((void*)consts, sizeof(C));
	}

	template<typename C>
	ConstantBuffer(const C& consts, TYPE_ConstBuffer type, const int slot = CONSTANT_BUFFER_DEFAULT_SLOT)
	{
		_internal_constructor((void*)&consts, sizeof(C), type, slot);
	}
	template<typename C>
	ConstantBuffer(const C* consts, TYPE_ConstBuffer type, const int slot = CONSTANT_BUFFER_DEFAULT_SLOT)
	{
		_internal_constructor((void*)consts, sizeof(C), type, slot);
	}

	~ConstantBuffer() override;

	void Bind() override;
private:
	void _internal_constructor(const void* data, unsigned size, TYPE_ConstBuffer type, const int slot);
	void _internal_update(const void* data, unsigned size_check);
	void* BindableData = nullptr;
};
