#pragma once
#include "Drawable.h"
#include "Bindable/Texture.h"

enum PROJECTION_TYPES {
	PT_MERCATOR,
	PT_AZIMUTHAL
};

class Background : public Drawable
{
public:
	Background(Image& texture, bool MakeDynamic = false, PROJECTION_TYPES ProjectionType = PT_MERCATOR);

	void updateTexture(Image& texture);
	void updateObserver(Vector3f obs);
	void updateWideness(float FOV, Vector2f WindowDimensions);
	void updateRectangle(_float4vector X0_Y0_X1_Y1);

private:

	struct PSconstBuffer {
		_float4vector obs;
		_float4vector ei;
		_float4vector zp;
	}cBuff0;

	void* pscBuff0 = nullptr;
	void* pscBuff1 = nullptr;
	void* vscBuff = nullptr;
};