#include "Mandelbrot.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "IG_Mandelbrot.h"

#include <string>

//  Public

Mandelbrot::Mandelbrot()
	: window(1080, 720, "Mandelbrot"),

	set(window.graphics)
{
	window.setFramerateLimit(60);
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

	if (Keyboard::isKeyPressed('W'))
		center.y += 5.f / scale;
	if (Keyboard::isKeyPressed('S'))
		center.y -= 5.f / scale;
	if (Keyboard::isKeyPressed('A'))
		center.x -= 5.f / scale;
	if (Keyboard::isKeyPressed('D'))
		center.x += 5.f / scale;

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
