#pragma once
#include "Drawable.h"

class Polihedron : public Drawable
{
public:

	Polihedron() {}
	Polihedron(Graphics& gfx, const Vector3f* vertexs, const Vector3i* triangles, UINT numT, const Color* colors = NULL, bool vertexColor = false, bool transparency = true, bool doubleSided = true);
	Polihedron(Graphics& gfx, const Vector3f* vertexs, const unsigned short* triangles, UINT numT, const Color* colors = NULL, bool vertexColor = false, bool transparency = true, bool doubleSided = true);

	void create(Graphics& gfx, const Vector3f* vertexs, const Vector3i* triangles, UINT numT, const Color* colors = NULL, bool vertexColor = false, bool transparency = true, bool doubleSided = true);
	void create(Graphics& gfx, const Vector3f* vertexs, const unsigned short* triangles, UINT numT, const Color* colors = NULL, bool vertexColor = false, bool transparency = true, bool doubleSided = true);


	void updateShape(Graphics& gfx, const Vector3f* vertexs, const Vector3i* triangles, UINT numT, const Color* colors = NULL, bool vertexColor = false);
	void updateRotation(Graphics& gfx, float rotationX, float rotationY, float rotationZ);
	void updateRotation(Graphics& gfx, Vector3f axis, float angle, bool multiplicative = false);
	void updateRotation(Graphics& gfx, Quaternion rotation, bool multiplicative = false);
	void updatePosition(Graphics& gfx, Vector3f position, bool additive = false);
	void updateScreenPosition(Graphics& gfx, Vector2f screenDisplacement);
	void updateLight(Graphics& gfx, UINT id, Vector2f intensity, Color color, Vector3f position);
	void updateLight(Graphics& gfx, UINT id, _float4vector intensity, _float4color color, _float4vector position);
	void clearLights(Graphics& gfx);

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

	ConstantBuffer<VSconstBuffer>* pVSCB = NULL;
	ConstantBuffer<PSconstBuffer>* pPSCB = NULL;

};