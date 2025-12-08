#pragma once
#include "Drawable.h"

class Curve : public Drawable
{
public:
	Curve(Vector3f(*F)(float), Vector2f rangeT, unsigned Npoints, Color(*color)(float) = [](float){return Color::White;}, bool transparency = false);

	void updateShape(Vector3f(*F)(float), Vector2f rangeT, unsigned Npoints, Color(*color)(float) = [](float){return Color::White;});

	void updateRotation(Quaternion rotation, bool multiplicative = false);
	void updatePosition(Vector3f position, bool additive = false);

	Quaternion getRotation();
	Vector3f getPosition();

private:

	void addDefaultBinds(bool transparency);

	struct Vertex {
		_float4vector position;
		_float4color color;
	};

	struct VSconstBuffer {
		_float4vector translation = { 0.f, 0.f, 0.f, 0.f };
		Quaternion rotation = 1.f;
	}vscBuff;

	void* pVSCB;
};
