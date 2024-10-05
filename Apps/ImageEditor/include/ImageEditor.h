#pragma once

#include "Window.h"
#include "Drawable/Background.h"
#include "Drawable/Surface.h"

#define INPUT_FOLDER(file)	(std::string(PROJECT_DIR) + std::string(R"(images\input\)")  + file).c_str()
#define OUTPUT_FOLDER(file) (std::string(PROJECT_DIR) + std::string(R"(images\output\)") + file).c_str()

struct IG_DATA {
	static float THETA;
	static float PHI;
	static float SPEED;

	enum TYPE{
		BACK,
		SPHERE,
		WEIRD,
	};

	static TYPE SHOW;
};

class ImageEditor {

private:
	Window window;

	float scale = 280.f;
	Vector3f center   = { 0.f, 0.f, 0.f };
	Vector3f observer = { 0.f,-1.f, 0.f };

	bool dragging;
	Vector2i initialDrag;
	Vector2f initialDragAngles;

	Image		image;
	Texture		imageTex;
	SURFACE_COLORING sc;

	Background	imageBG;
	Surface		imageSP;
	Surface		imageWD;

public:
	ImageEditor();
	Image initialImage(const char* filename) const;

	int Run();

	void imageChanges();
	void eventManager();
	void doFrame();
};

//	Shapes

float exampleRadius(float, float);

template <float r>
float Radius(float, float)
{
	return r;
}
