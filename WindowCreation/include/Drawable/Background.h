#pragma once
#include "Drawable.h"

enum PROJECTION_TYPES {
	PT_MERCATOR,
	PT_AZIMUTHAL
};

class Background : public Drawable
{
public:
	Background(Graphics& gfx, const char* filename, bool MakeDynamic = false, PROJECTION_TYPES ProjectionType = PT_MERCATOR);
	Background(Graphics& gfx, Image& texture, bool MakeDynamic = false, PROJECTION_TYPES ProjectionType = PT_MERCATOR);
	Background(Graphics& gfx, Texture texture, bool MakeDynamic = false, PROJECTION_TYPES ProjectionType = PT_MERCATOR);

	void create(Graphics& gfx, const char* filename, bool MakeDynamic = false, PROJECTION_TYPES ProjectionType = PT_MERCATOR);
	void create(Graphics& gfx, Image& texture, bool MakeDynamic = false, PROJECTION_TYPES ProjectionType = PT_MERCATOR);
	void create(Graphics& gfx, Texture texture, bool MakeDynamic = false, PROJECTION_TYPES ProjectionType = PT_MERCATOR);

	void updateTexture(Graphics& gfx, const char* filename);
	void updateTexture(Graphics& gfx, Image& texture);
	void updateTexture(Graphics& gfx, Texture texture);
	void updateObserver(Graphics& gfx, Vector3f obs);
	void updateWideness(Graphics& gfx, float FOV, Vector2f WindowDimensions);
	void updateRectangle(Graphics& gfx, _float4vector X0_Y0_X1_Y1);

private:

	struct PSconstBuffer {
		_float4vector obs;
		_float4vector ei;
		_float4vector zp;
	}cBuff0;

	ConstantBuffer<PSconstBuffer>* pscBuff0 = NULL;
	ConstantBuffer<_float4vector>* pscBuff1 = NULL;
	ConstantBuffer<_float4vector>* vscBuff = NULL;
};