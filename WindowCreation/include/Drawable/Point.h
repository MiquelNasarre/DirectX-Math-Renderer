#pragma once
#include "Drawable.h"

class Point : public Drawable
{
public:
	Point(Vector3f position, float radius, Color col = Color::White);

	void updatePosition(Vector3f position);
	void updateRadius(float radius);
	void updateColor(Color col);
	void updateRotation(Quaternion rotation, bool multiplicative = false);

	void Draw(Window& _w) override;
private:

	struct VSconstBuffer {
		_float4vector position;
		Quaternion rotation;
		float radius;
		float scale;
		Vector2f unused;
	}vscBuff;

	struct PSconstBuffer {
		_float4color color;
	}pscBuff;

	struct Vertex {
		Vector3f Normal;
		float zero = 0.f;
	};

	void* pVSCB;
	void* pPSCB;
};