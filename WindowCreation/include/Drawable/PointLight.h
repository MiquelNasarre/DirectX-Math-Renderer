#pragma once
#include "Drawable.h"

class PointLight : public Drawable
{
public:
	PointLight(Color color, Vector3f Position, float Radius, unsigned circlePoints = 40u);

	void updateRadius(float radius);
	void updatePosition(Vector3f position);
	void updateColor(Color color);

	void Draw(Window& _w) override;

private:

	struct VSconstBuffer {
		_float4vector Position;
		_float4vector observer;
		_float4vector radius;
	}vscBuff;

	struct PSconstBuffer {
		_float4color color;
	}pscBuff;

	struct Vertex {
		Vector2f reference;
		float intensity;
	};

	void* pVSCB = nullptr;
	void* pPSCB = nullptr;
};
