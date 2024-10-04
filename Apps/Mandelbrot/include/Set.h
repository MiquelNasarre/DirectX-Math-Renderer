#pragma once
#include "Drawable.h"

class Set : public Drawable
{
public:
	Set(Graphics& gfx);

	void updateGradient(Graphics& gfx, Color color, float tolerance, unsigned char ID);
private:

	struct PSconstBuffer
	{
		_float4color color[20];
	}PcBuff;

	ConstantBuffer<PSconstBuffer>* pscBuff = NULL;
};