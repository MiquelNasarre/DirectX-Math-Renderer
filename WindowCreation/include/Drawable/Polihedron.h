#pragma once
#include "Drawable.h"

class Polihedron : public Drawable
{
public:

	Polihedron() {}
	Polihedron(const Vector3f* vertexs, const Vector3i* triangles, unsigned numT, const Color* colors = nullptr, bool vertexColor = false, bool transparency = true, bool doubleSided = true);

	void create(const Vector3f* vertexs, const Vector3i* triangles, unsigned numT, const Color* colors = nullptr, bool vertexColor = false, bool transparency = true, bool doubleSided = true);

	void updateShape(const Vector3f* vertexs, const Vector3i* triangles, unsigned numT, const Color* colors = nullptr, bool vertexColor = false);
	void updateRotation(Quaternion rotation, bool multiplicative = false);
	void updatePosition(Vector3f position, bool additive = false);
	void updateScreenPosition(Vector2f screenDisplacement);
	void updateLight(unsigned id, Vector2f intensity, Color color, Vector3f position);
	void clearLights();

	Quaternion getRotation();
	Vector3f getPosition();
	
private:
	struct Vertex {
		Vector3f vector;
		Vector3f norm;
		Color color;
	};

	struct VSconstBuffer {
		_float4vector translation = { 0.f, 0.f, 0.f, 0.f };
		Quaternion rotation = 1.f;
		_float4vector screenDisplacement = { 0.f, 0.f, 0.f, 0.f };
	}vscBuff;

	struct PSconstBuffer {
		struct {
			_float4vector intensity = { 0.f,0.f,0.f,0.f };
			_float4color  color		= { 1.f,1.f,1.f,1.f };
			_float4vector position	= { 0.f,0.f,0.f,0.f };
		}lightsource[8];
	}pscBuff;

	void* pVSCB = nullptr;
	void* pPSCB = nullptr;

};