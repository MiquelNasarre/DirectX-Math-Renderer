#pragma once
#include "Drawable.h"

#define error_epsilon	0.001f

enum SURFACE_TYPE {
	_EXPLICIT,
	_EXPLICIT_ICOSPHERE,
	_PARAMETRIC,
	_IMPLICIT,
};

struct SURFACE_SHAPE
{
	SURFACE_TYPE Type;

	//	Explicit

	float (*Explicit)(float, float) = nullptr;

	//	Parametric

	Vector3f(*Parametric_V)(float, float) = nullptr;

	//	Implicit

	float (*Implicit)(float, float, float) = nullptr;

	//	Coordinates

	Vector2f minRect, maxRect;
	unsigned numU, numV;

	unsigned ICOSPHERE_DEPHT;

	Vector3f minCube, maxCube;

	//	Constructors

	SURFACE_SHAPE(SURFACE_TYPE Type, float explicit_(float, float), Vector2f minRect = { -1.f, -1.f }, Vector2f maxRect = { 1.f, 1.f }, unsigned numX = 100u, unsigned numY = 100u);
	SURFACE_SHAPE(SURFACE_TYPE Type, float radius(float, float), unsigned ICOSPHERE_DEPTH);
	SURFACE_SHAPE(SURFACE_TYPE Type, Vector3f parametric_v(float, float), Vector2f minRect = { -1.f, -1.f }, Vector2f maxRect = { 1.f, 1.f }, unsigned numU = 100u, unsigned numV = 100u);
	SURFACE_SHAPE(SURFACE_TYPE Type, float implicit(float, float, float), Vector3f minCube = { -2.f, -2.f, -2.f }, Vector3f maxCube = { 2.f, 2.f, 2.f });
};

struct SURFACE_COLORING
{
	bool Textured = false;

	Image* texture0 = nullptr;
	Image* texture1 = nullptr;
	Color color = Color::White;

	bool Lighted = true;
	bool DefaultInitialLights = true;
	bool transparency = false;

	struct LIGHTSOURCE {
		_float4vector intensity = { 0.f,0.f,0.f,0.f };
		_float4color  color = { 1.f,1.f,1.f,1.f };
		_float4vector position = { 0.f,0.f,0.f,0.f };
	}lightsource[8];

	SURFACE_COLORING() {}
	SURFACE_COLORING(Image* ptexture0, Image* ptexture1 = nullptr, bool lighted = true)
	{
		Lighted = lighted;
		Textured = true;
		texture0 = ptexture0;
		if (ptexture1)
			texture1 = ptexture1;
	}
	SURFACE_COLORING(Image* ptexture, bool lighted = true)
	{
		Lighted = lighted;
		Textured = true;
		texture0 = ptexture;
	}
	SURFACE_COLORING(Color col, bool lighted = true)
	{
		Lighted = lighted;
		color = col;
	}
	SURFACE_COLORING(const SURFACE_COLORING& other) :
		Textured		{ other.Textured },
		texture0		{ other.texture0 },
		texture1		{ other.texture1 },
		color			{ other.color },
		Lighted			{ other.Lighted },
		DefaultInitialLights	{ other.DefaultInitialLights }
	{
		if (!Lighted || DefaultInitialLights)
			return;

		lightsource[0] = other.lightsource[0];
		lightsource[1] = other.lightsource[1];
		lightsource[2] = other.lightsource[2];
		lightsource[3] = other.lightsource[3];
		lightsource[4] = other.lightsource[4];
		lightsource[5] = other.lightsource[5];
		lightsource[6] = other.lightsource[6];
		lightsource[7] = other.lightsource[7];
	}
};

class Surface : public Drawable
{
public:
	Surface() {}

	Surface(SURFACE_SHAPE* ss, SURFACE_COLORING* psc = nullptr);

	//	Two to rule them all!!

	void create(SURFACE_SHAPE* ss, SURFACE_COLORING* psc = nullptr);

	//	Public functions

	void updateRotation(Quaternion rotation, bool multiplicative = false);
	void updatePosition(Vector3f position, bool additive = false);

	void updateTexture(unsigned id, Image& image);
	void updateTextures(Image& image0, Image& image1);

	void updateLight(unsigned id, Vector2f intensity, Color color, Vector3f position);
	void clearLights();
	void updateShape(SURFACE_SHAPE* ss);
	void updateColor(Color color);

	Quaternion getRotation();
	Vector3f getPosition();

private:
	void** getShapeBuffers(SURFACE_SHAPE* ss);
	void** generateIcosphere(unsigned int depth);

	struct VSconstBuffer {
		_float4vector translation = { 0.f, 0.f, 0.f, 0.f };
		Quaternion rotation = 1.f;
	}vscBuff;

	struct PSconstBuffer {
		SURFACE_COLORING::LIGHTSOURCE lightsource[8];
	}pscBuff;

	struct PSconstBufferColor {
		_float4color color = {};
	}pscBuffc;

	void* pVSCB = nullptr;
	void* pPSCB = nullptr;
	void* pPSCBc = nullptr;
	SURFACE_COLORING sc;

	struct Vertex {
		Vector3f vector;
		Vector3f norm;
	};

	struct TexVertex {
		Vector3f vector;
		Vector3f norm;
		Vector2f texCoor;
	};

	struct VertexArr
	{
		Vertex* data = nullptr;
		unsigned counter = 0;

		VertexArr(unsigned max)
		{
			data = new Vertex[max];
		}

		void push_back(Vector3f v, Vector3f norm)
		{
			data[counter].vector = v;
			data[counter].norm = norm;
			++counter;
		}

		~VertexArr()
		{
			delete[] data;
		}
	};

	struct TexVertexArr
	{
		TexVertex* data = nullptr;
		unsigned counter = 0;

		TexVertexArr(unsigned max)
		{
			data = new TexVertex[max];
		}

		void push_back(Vector3f v, Vector3f norm, Vector2f tex)
		{
			data[counter].vector = v;
			data[counter].norm = norm;
			data[counter].texCoor = tex;
			++counter;
		}

		~TexVertexArr()
		{
			delete[] data;
		}
	};

	struct IndexArr
	{
		unsigned int* data = nullptr;
		unsigned counter = 0;

		IndexArr(unsigned max)
		{
			data = new unsigned int[max];
		}

		void push_back(unsigned int i)
		{
			data[counter] = i;
			++counter;
		}

		~IndexArr()
		{
			delete[] data;
		}
	};

	Vector3f evalPolar(float r(float, float), float theta, float phi);
	Vector3f makePolar(Vector3f other);
	void addVertexsCube(_float4vector cube[8], VertexArr& vertexs, IndexArr& indexs, bool polar = false);

};