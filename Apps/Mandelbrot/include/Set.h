#pragma once
#include "Drawable.h"

#include "Math/Complex.h"

class Set : public Drawable
{
public:
	Set(Graphics& gfx);

	void updateGradient(Graphics& gfx, Color color, float tolerance, unsigned char ID);
	void saveFrame(Graphics& gfx, const char* filename, Vector2i Dimensions, bool adapt);
private:

	struct PSconstBuffer
	{
		_float4color color[20];
	}PcBuff;

	static void saveFrameAsync(PSconstBuffer cb, const char* filename, Complex start, float dx, float dy, Vector2i Dimensions);

	ConstantBuffer<PSconstBuffer>* pscBuff = NULL;
};