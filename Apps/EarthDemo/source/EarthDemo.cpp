#include "EarthDemo.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "IG_EarthDemo.h"

#include <string>

float IG_DATA::EARTH_THETA = 0.f;
float IG_DATA::EARTH_PHI = 0.f;
float IG_DATA::SPEED = 0.f;
float IG_DATA::MOON_SPEED = 1.f;
float IG_DATA::MOON_POS = 0.f;
float IG_DATA::THETA = MATH_PI / 2.f;
float IG_DATA::PHI = 0.f;
float IG_DATA::FOV = 1.f;

int	  IG_DATA::UPDATE_LIGHT = -1;
Vector2i IG_DATA::UPDATE_TEXTURE = Vector2i(-1, -1);

IG_DATA::lightsource* IG_DATA::LIGHTS = (IG_DATA::lightsource*)calloc(sizeof(IG_DATA::lightsource), 8);

EarthDemo::EarthDemo()
	: window({ 640, 480 }, "Hello World", "", true),

	TexEarth(Image("Resources/EarthDemo/earthTextures/default.jpg")),
	TexNEarth			("Resources/EarthDemo/earthTextures/defaultNight.jpg"	),
	TexEarthInverted	(window.graphics,	"Resources/EarthDemo/earthTextures/inverted.jpg"		),
	TexNEarthInverted	(window.graphics,	"Resources/EarthDemo/earthTextures/invertedNight.jpg"	),
	TexEarthChalked		(window.graphics,	"Resources/EarthDemo/earthTextures/chalked.jpg"			),
	TexNEarthChalked	(window.graphics,	"Resources/EarthDemo/earthTextures/chalkedNight.jpg"	),
	TexMoon				(window.graphics,	"Resources/EarthDemo/moonTextures/default.jpg"			),
	TexMoonInverted		(window.graphics,	"Resources/EarthDemo/moonTextures/inverted.jpg"			),
	TexMoonChalked		(window.graphics,	"Resources/EarthDemo/moonTextures/chalked.jpg"			),
	TexBack				("Resources/EarthDemo/nightSky/highperformance.jpg"		),
	TexBackInverted		("Resources/EarthDemo/nightSky/inverted.jpg"				),
	TexBackEarth		("Resources/EarthDemo/earthTextures/projected.jpg"		),
	TexBackMoon			("Resources/EarthDemo/moonTextures/projected.jpg"		),

	Earth	(SURFACE_SHAPE(_EXPLICIT_SPHERICAL,EarthRadius), SURFACE_COLORING(TexEarth, TexNEarth).ptr()),
	Moon	(SURFACE_SHAPE(_EXPLICIT_SPHERICAL, MoonRadius), SURFACE_COLORING(TexMoon).ptr()),
	back	(TexBack, true, PT_AZIMUTHAL)
{

	IG_DATA::LIGHTS[0].is_on = true;
	IG_DATA::LIGHTS[0].color = Color::White.getColor4();
	IG_DATA::LIGHTS[0].intensities = { 32000.f,5000.f };
	IG_DATA::LIGHTS[0].position = { 160.f,0.f,60.f };

	window.setFramerateLimit(60);
	timer.reset();
}

int EarthDemo::Run()
{
	while (window.processEvents())
		doFrame();
	return 0;
}

void EarthDemo::eventManager()
{
	//	Keyboard and Mouse events

	scale *= powf(1.1f, Mouse::getWheel() / 120.f);

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

	if (Mouse::isButtonPressed(Mouse::Left) && !dragging) {
		dragging = true;
		initialDrag = Mouse::getPosition();
		initialDragAngles = { IG_DATA::EARTH_THETA,IG_DATA::EARTH_PHI };
	}
	if (dragging && !Mouse::isButtonPressed(Mouse::Left))
		dragging = false;

	if (dragging) {
		Vector2i movement = Mouse::getPosition() - initialDrag;
		float temp = initialDragAngles.x - 2.f * (float)movement.x / scale;
		IG_DATA::SPEED = -10.f * (temp - IG_DATA::EARTH_THETA);
		IG_DATA::EARTH_THETA = temp;
		IG_DATA::EARTH_PHI = initialDragAngles.y + 2.f * (float)movement.y / scale;
		if (IG_DATA::EARTH_PHI > pi / 2.f) {
			IG_DATA::EARTH_PHI = pi / 2.f - 0.0001f;
			initialDrag.y = Mouse::getPosition().y - int((pi / 2.f - initialDragAngles.y) * scale / 2.f);
		}
		if (IG_DATA::EARTH_PHI < -pi / 2.f) {
			IG_DATA::EARTH_PHI = -pi / 2.f + 0.0001f;
			initialDrag.y = Mouse::getPosition().y - int((-pi / 2.f - initialDragAngles.y) * scale / 2.f);
		}
	}
	else {
		if (IG_DATA::SPEED > 1.f) {
			IG_DATA::SPEED *= 0.99f;
			if (IG_DATA::SPEED < 1.f)
				IG_DATA::SPEED = 1.f;
		}
		if (IG_DATA::SPEED < -1.f) {
			IG_DATA::SPEED *= 0.99f;
			if (IG_DATA::SPEED > -1.f)
				IG_DATA::SPEED = -1.f;
		}

		IG_DATA::EARTH_THETA += -IG_DATA::SPEED / 10.f;
	}

	IG_DATA::MOON_POS += IG_DATA::MOON_SPEED / 300.f;

	//	Calculate observer vector

	observer = {
			-cosf(IG_DATA::PHI) * cosf(IG_DATA::THETA) ,
			-cosf(IG_DATA::PHI) * sinf(IG_DATA::THETA) ,
			-sinf(IG_DATA::PHI)
	};

	//	Set lights & textures

	int l = IG_DATA::UPDATE_LIGHT;
	if (l == -2) {
		for (int i = 0; i < 8; i++) {
			Earth.updateLight(window.graphics, i, IG_DATA::LIGHTS[i].intensities, IG_DATA::LIGHTS[i].color, IG_DATA::LIGHTS[i].position);
			Moon.updateLight(window.graphics, i, IG_DATA::LIGHTS[i].intensities, IG_DATA::LIGHTS[i].color, IG_DATA::LIGHTS[i].position);
		}
	}
	if (l > -1) {
		Earth.updateLight(window.graphics, l, IG_DATA::LIGHTS[l].intensities, IG_DATA::LIGHTS[l].color, IG_DATA::LIGHTS[l].position);
		Moon.updateLight(window.graphics, l, IG_DATA::LIGHTS[l].intensities, IG_DATA::LIGHTS[l].color, IG_DATA::LIGHTS[l].position);
		IG_DATA::UPDATE_LIGHT = -1;
	}
	Vector2i t = IG_DATA::UPDATE_TEXTURE;
	if (t.x == 0) {
		if (t.y == 0)
			Earth.updateTextures(window.graphics, TexEarth, TexNEarth);
		if (t.y == 1)
			Earth.updateTextures(window.graphics, TexEarthInverted, TexNEarthInverted);
		if (t.y == 2)
			Earth.updateTextures(window.graphics, TexEarthChalked, TexNEarthChalked);
		if (t.y == 3)
			Earth.updateTextures(window.graphics, TexMoon, TexMoon);
		if (t.y == 4)
			Earth.updateTextures(window.graphics, TexEarth, TexEarth);
		if (t.y == 5)
			Earth.updateTextures(window.graphics, TexNEarth, TexNEarth);
		t.x = -1;
	}
	if (t.x == 1) {
		if (t.y == 0)
			Moon.updateTextures(window.graphics, TexMoon, TexMoon);
		if (t.y == 1)
			Moon.updateTextures(window.graphics, TexMoonInverted, TexMoonInverted);
		if (t.y == 2)
			Moon.updateTextures(window.graphics, TexMoonChalked, TexMoonChalked);
		if (t.y == 3)
			Moon.updateTextures(window.graphics, TexEarth, TexNEarth);
		t.x = -1;
	}
	if (t.x == 2) {
		if (t.y == 0)
			back.updateTexture(window.graphics, TexBack);
		if (t.y == 1)
			back.updateTexture(window.graphics, TexBackInverted);
		if (t.y == 2)
			back.updateTexture(window.graphics, TexBackEarth);
		if (t.y == 3)
			back.updateTexture(window.graphics, TexBackMoon);
		t.x = -1;
	}
	if (t.x == 3) {
		Earth.updateTextures(window.graphics, TexEarth, TexNEarth);
		Moon.updateTextures(window.graphics, TexMoon, TexMoon);
		back.updateTexture(window.graphics, TexBack);
		t.x = -1;
	}
}

void EarthDemo::doFrame()
{
	eventManager();

	//	Update objects

	window.graphics().updatePerspective(observer, center, scale);
	back.updateObserver(observer);
	back.updateWideness(IG_DATA::FOV, (Vector2f)window.getDimensions());

	Earth.updateRotation(rotationQuaternion(Vector3f(1.f, 0.f, 0.f), -IG_DATA::EARTH_THETA) * rotationQuaternion(Vector3f(0.f,0.f,1.f), -IG_DATA::EARTH_PHI));
	//Moon.updateRotation(0.f, 0.f, IG_DATA::MOON_POS);
	Moon.updatePosition(Vector3f(10.f * cosf(IG_DATA::MOON_POS), 10.f * sinf(IG_DATA::MOON_POS), 0.f));

	//window.setTitle("Hello World  -  " + std::to_string(int(std::round(window.getFramerate()))) + "fps");

	//	Rendering

	window.graphics().clearBuffer(Color::Black);
	back.Draw();

	Earth.Draw();
	Moon.Draw();

	//	ImGui crap

	IG_EarthDemo::render();

	//	Push the frame to the scriin

	window.graphics().pushFrame();
}

//	Surface functions

float EarthRadius(float, float)
{
	return 1.f;
}

float MoonRadius(float, float)
{
	return 0.3f;
}
