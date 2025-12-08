#pragma once
#include "Bindable.h"

class Topology : public Bindable
{
public:
	Topology(unsigned type);
	void Bind() override;
private:
	unsigned type;
};