#include "Mandelbrot.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "IG_Mandelbrot.h"

#include <string>

bool		IG::MENU_OPEN		= false;
bool		IG::SAVE_MENU_OPEN	= false;
Vector2i	IG::WindowDim		= { 0,0 };
bool		IG::SAVE			= false;
Vector2i	IG::QUALITY			= { 0,0 };
const char* IG::PATH			= (const char*)calloc(200, sizeof(char));
bool		IG::ADAPT			= true;

//  Public

Mandelbrot::Mandelbrot()
	: window(1080, 720, "Mandelbrot"),

	set(window.graphics)
{
	window.setFramerateLimit(60);
	window.setIcon(R"(C:\Users\PC\Desktop\LearnDirectX\Apps\Mandelbrot\resources\mandelbrot.ico)");

	memcpy((void*)IG::PATH, SAVE_DIR, sizeof(SAVE_DIR));
}

int Mandelbrot::Run()
{
	while (window.processEvents())
		doFrame();
	return 0;
}

void Mandelbrot::eventManager()
{
	// Full screen

	if (Keyboard::isKeyPressed(VK_F11))
		window.setFullScreen(true);

	if (Keyboard::isKeyPressed(VK_ESCAPE))
		window.setFullScreen(false);

	//  Keyboard events

	if (!mpressed && Keyboard::isKeyPressed('M'))
	{
		mpressed = true;
		if (IG::MENU_OPEN)
			IG::MENU_OPEN = false;
		else
			IG::MENU_OPEN = true;
	}
	if (mpressed && !Keyboard::isKeyPressed('M'))
		mpressed = false;

	static bool enable_save = false;
	if (Keyboard::isKeyPressed(VK_CONTROL) && !Keyboard::isKeyPressed('S'))
		enable_save = true;
	else if (!Keyboard::isKeyPressed(VK_CONTROL))
		enable_save = false;
	if (Keyboard::isKeyPressed('S') && enable_save)
	{
		enable_save = false;
		IG::SAVE_MENU_OPEN = true;
	}


	//	Mouse events

	if (Mouse::isButtonPressed(Mouse::Left) && !dragging) {
		dragging = true;
		lastPos = Mouse::getPosition();
	}
	if (dragging && !Mouse::isButtonPressed(Mouse::Left))
		dragging = false;

	if (dragging) {
		Vector2i MousePos = Mouse::getPosition();
		Vector2i movement = MousePos - lastPos;
		lastPos = MousePos;
		center += 2.f * Vector3f(-movement.x, movement.y, 0) / scale;
	}

	float factor = powf(1.1f, Mouse::getWheel() / 120.f);
	scale *= factor;
	Vector2f centerDisplacement = (1.f - factor) * (Mouse::getPosition() - window.getDimensions() / 2) / scale;
	center += 2 * Vector3f(-centerDisplacement.x, centerDisplacement.y, 0.f);

	IG::WindowDim = window.getDimensions();

	if (IG::SAVE)
	{
		set.saveFrame(window.graphics, IG::PATH, IG::QUALITY, IG::ADAPT);
		IG::SAVE = false;
	}

}

void Mandelbrot::doFrame()
{
	eventManager();

	window.graphics.updatePerspective({ 0.f, 0.f, -1.f }, center, scale);

	//	Update objects

	window.setTitle("Mandelbrot  -  " + std::to_string(int(std::round(window.getFramerate()))) + "fps");

	//	Rendering

	set.Draw(window.graphics);

	//	ImGui crap

	IG_Mandelbrot::render();

	//	Push the frame to the scriin

	window.graphics.pushFrame();
}
