#pragma once

#include "Window.h"
#include "Set.h"

//  Struct for variables shared with ImGui

struct IG_DATA {
};

//  Main class for running the Application

class Mandelbrot {

private:
	Window window;

	//  Variables for the FOV

	bool mpressed = false;
	bool menu = false;
	float scale = 600.f;
	Vector3f center   = { -0.5f, 0.f, 0.f };

	//  Variables for the mouse interaction

	bool dragging = false;
	Vector2i lastPos;
	Vector3f axis = Vector3f(1.f, -1.f, 1.f);
	float dangle = 0.01f;

	//  Drawables

	Set set;

public:
	Mandelbrot();

	int Run();

	void eventManager();
	void doFrame();
};