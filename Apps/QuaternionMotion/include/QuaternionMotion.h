#pragma once

#include "Window.h"
#include "Drawable/Surface.h"
#include "Drawable/Polihedron.h"
#include "Drawable/Background.h"
#include "Drawable/Curve.h"
#include "Drawable/Light.h"
#include "IG_QuaternionMotion.h"

enum DRAG_TYPE
{
	RIGID_PLANE,
	RIGID_SPACE,
	DYNAMIC_PLANE,
	DYNAMIC_SPACE,
	MAGNETIC_MOUSE,
};

enum SHAPE
{
	SQUARE,
	WEIRD_SHAPE,
	OCTAHEDRON,
	KLEIN
};

struct IG_DATA 
{

	static inline float THETA = 0.f;
	static inline float PHI = 0.f;

	static inline DRAG_TYPE TYPE = DYNAMIC_SPACE;
	static inline SHAPE FIGURE = SQUARE;

	struct lightsource 
	{
		bool is_on;
		Vector2f intensities;
		Color color;
		Vector3f position;
	};

	static inline lightsource LIGHTS[8] = {};

	static inline int UPDATE_LIGHT = -1;
};

class QuaternionMotion 
{
private:
	Window window;
#ifdef _INCLUDE_IMGUI
	IG_QuaternionMotion imGui;
#endif
	float scale = 280.f;
	Vector3f center   = { 0.f, 0.f, 0.f };
	Quaternion observer = 1.f;

	Quaternion rotation = { 1.f, 0.0025f,-0.0025f, 0.0025f };
	Vector3f axis = Vector3f(1.f, -1.f, 1.f);
	float dangle = 0.01f;

	Vector2i lastPos = {};
	bool dragging = false;

	//	Magnet motion

	bool magnetized = false;
	Vector3f magnetPos = { 1.f, 0.f, 0.f };

	Light		light;
	Curve		curve;
	Background  back;
	Surface		shape_0;
	Polihedron	shape_1;
	Polihedron	shape_2;
	Surface		shape_3;

public:
	QuaternionMotion();

	int Run();


	void eventManager();
	void doFrame();

private:
	void magneticReturn();
	void strictReturn();

	void drag_rigid_plane();
	void drag_rigid_space();
	void drag_dynamic_plane();
	void drag_dynamic_space();
	void drag_magnetic_mouse();
};

//	Functions

float exampleRadius(float, float, float);

Vector3f KleinBottle(float u, float v);