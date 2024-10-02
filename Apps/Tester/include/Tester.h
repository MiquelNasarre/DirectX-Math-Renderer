#pragma once

#include "Window.h"

#include "Drawable/Surface.h"
#include "Drawable/PointLight.h"
#include "Drawable/Curve.h"
#include "Drawable/Point.h"
#include "Drawable/Polihedron.h"

struct IG_DATA {
	static float THETA;
	static float PHI;
};

class Tester {

private:
	Window window;
	Timer timer;

	//	Add drawables you want to test here

	Surface surf;
	PointLight light;
	Curve curve;
	Point point;

	Surface impl;

	Surface Klein;

	Surface test;

	Polihedron poli;

	Surface torus;

	//	...

	Vector3f axis = Vector3f(1.f,-1.f,1.f);
	float dangle = 0.01f;

	Vector2i lastPos;

	float scale = 250.f;
	float theta = 0.f;
	float phi = 0.f;
	float speed = 0.f;
	Vector3f center = {};

	bool dragging = false;
	Vector2i initialDrag;
	Vector2f initialDragAngles;

	float var = 3.f;
	PARAM_SURFACE_SHAPE<float> shape;

public:
	Tester();
	int Run();

	void eventManager();
	void doFrame();
};

//	Surface functions

Vector3f curveF(float);

Vector3f KleinBottle(float, float);

float curveX(float);

float curveY(float);

float curveZ(float);

float SincFunction(float, float, const float&);

float constantRadius(float, float);

float constantRadius03(float, float);

float weirdRadius(float, float);

float returnX(float, float);

float returnY(float, float);

float sphere(float, float, float);

float weirdRadiusDynamic(float, float, const float&);

Vector3f thorus(float phi, float theta);