#include "$safeprojectname$.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "IG_$safeprojectname$.h"

#include <string>

float IG::THETA = pi / 2.f;
float IG::PHI	 = 0.f;

//  Private

void $safeprojectname$::drag_motion()
{
	Vector3f obs = window.graphics.getObserver();
	Vector3f ex = -(obs * Vector3f(0.f, 0.f, 1.f)).normalize();
	Vector3f ey = ex * obs;

	Vector3f lastMouse = (-obs + (ex * (lastPos.x - window.getDimensions().x / 2) + ey * (lastPos.y - window.getDimensions().y / 2)) / scale).normalize();
	lastPos = Mouse::getPosition();
	Vector3f newMouse = (-obs + (ex * (lastPos.x - window.getDimensions().x / 2) + ey * (lastPos.y - window.getDimensions().y / 2)) / scale).normalize();

	Vector3f axis0 = lastMouse * newMouse;
	if (!axis0) axis0 = newMouse;
	axis0.normalize();
	float dangle0 = -acosf(lastMouse ^ newMouse);
	if ((lastMouse ^ newMouse) > 1.f)dangle0 = 0.f;
	if (lastMouse == newMouse) dangle0 = 0.f;

	constexpr float s = 1.f / 1.5f;

	Quaternion newRot = rotationQuaternion(newMouse, Mouse::getWheel() / 18000.f) * rotationQuaternion(axis0, dangle0) * rotationQuaternion(axis, (dangle * (1 - s + fabs(axis ^ newMouse) * s)));

	dangle = 2 * acosf(newRot.r);
	axis = newRot.getVector();
	if (!axis)axis = newMouse;
	axis.normalize();
}

//  Public

$safeprojectname$::$safeprojectname$()
	: window(640, 480, "$safeprojectname$"),

	example(window.graphics, SURFACE_SHAPE(_EXPLICIT_SPHERICAL, exampleRadius))
{
	window.setFramerateLimit(60);
}

int $safeprojectname$::Run()
{
	while (window.processEvents())
		doFrame();
	return 0;
}

void $safeprojectname$::eventManager()
{
	//  Keyboard events

	if (Keyboard::isKeyPressed('W'))
		center.y += 0.02f;
	if (Keyboard::isKeyPressed('S'))
		center.y -= 0.02f;
	if (Keyboard::isKeyPressed('A'))
		center.x -= 0.02f;
	if (Keyboard::isKeyPressed('D'))
		center.x += 0.02f;
	if (Keyboard::isKeyPressed('R'))
		center.z += 0.02f;
	if (Keyboard::isKeyPressed('F'))
		center.z -= 0.02f;

	//	Mouse events

	if (Mouse::isButtonPressed(Mouse::Left) && !dragging)
	{
		dragging = true;
		lastPos = Mouse::getPosition();
	}
	if (dragging && !Mouse::isButtonPressed(Mouse::Left))
		dragging = false;

	if (dragging)
		drag_motion();
	else
		scale *= powf(1.1f, Mouse::getWheel() / 120.f);

	//  Update observer

	observer = { -cosf(IG::PHI) * cosf(IG::THETA), -cosf(IG::PHI) * sinf(IG::THETA), -sinf(IG::PHI) };
}

void $safeprojectname$::doFrame()
{
	eventManager();

	//	Update objects

	window.graphics.updatePerspective(observer, center, scale);

	example.updateRotation(window.graphics, axis, dangle, true);

	window.setTitle("$safeprojectname$  -  " + std::to_string(int(std::round(window.getFramerate()))) + "fps");

	//	Rendering

	window.graphics.clearBuffer(Color::Black);

	example.Draw(window.graphics);

	//	ImGui crap

	IG_$safeprojectname$::render();

	//	Push the frame to the scriin

	window.graphics.pushFrame();
}

//	Functions

float exampleRadius(float theta, float phi)
{
	return 1.f + (sinf(theta) * sinf(theta) * cosf(phi) + cosf(phi) * cosf(phi) * sinf(phi)) * sinf(5 * theta) * cosf(3 * phi) / 2.f;
}
