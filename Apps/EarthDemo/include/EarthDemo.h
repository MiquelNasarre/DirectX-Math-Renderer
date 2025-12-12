#pragma once

#include "Window.h"
#include "Drawable/Surface.h"
#include "Drawable/Background.h"
#include "Timer.h"

struct IG_DATA {
	static float EARTH_THETA;
	static float EARTH_PHI;
	static float SPEED;
	static float MOON_SPEED;
	static float MOON_POS;
	static float THETA;
	static float PHI;
	static float FOV;

	static int UPDATE_LIGHT;
	static Vector2i UPDATE_TEXTURE;

	struct lightsource {
		bool is_on;
		_float4vector intensities;
		_float4color color;
		_float4vector position;
	};

	static lightsource* LIGHTS;

};

class EarthDemo {

private:
	Window window;
	Timer timer;

	float scale = 280.f;
	Vector3f center = { 0.f,0.f,0.f };
	Vector3f observer;

	bool dragging;
	Vector2i initialDrag;
	Vector2f initialDragAngles;

	Texture TexEarth;
	Texture TexNEarth;
	Texture TexEarthInverted;
	Texture TexNEarthInverted;
	Texture TexEarthChalked;
	Texture TexNEarthChalked;

	Texture TexMoon;
	Texture TexMoonInverted;
	Texture TexMoonChalked;

	Texture TexBack;
	Texture TexBackInverted;
	Texture TexBackEarth;
	Texture TexBackMoon;

	Surface Earth;
	Surface Moon;
	Background back;

public:
	EarthDemo();

	int Run();


	void eventManager();
	void doFrame();
};

//	Surface functions

float EarthRadius(float, float);

float MoonRadius(float, float);