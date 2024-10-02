#include "QuaternionMotion.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "IG_QuaternionMotion.h"
#include <string>

#define DO(call) shape_0.call;shape_1.call;shape_2.call;shape_3.call

QuaternionMotion::QuaternionMotion()
	: window(640, 480, "QuaternionMotion", "", true)
{
	window.setFramerateLimit(60);

	IG_DATA::LIGHTS[0].is_on = true;
	IG_DATA::LIGHTS[0].color = Color(255, 51, 51, 255).getColor4();
	IG_DATA::LIGHTS[0].intensities = { 60.f,10.f };
	IG_DATA::LIGHTS[0].position = { 0.f,8.f,8.f };
	IG_DATA::LIGHTS[1].is_on = true;
	IG_DATA::LIGHTS[1].color = Color(0, 255, 0, 255).getColor4();
	IG_DATA::LIGHTS[1].intensities = { 60.f,10.f };
	IG_DATA::LIGHTS[1].position = { 0.f,-8.f,8.f };
	IG_DATA::LIGHTS[2].is_on = true;
	IG_DATA::LIGHTS[2].color = Color(127, 0, 255, 255).getColor4();
	IG_DATA::LIGHTS[2].intensities = { 60.f,10.f };
	IG_DATA::LIGHTS[2].position = { -8.f,0.f,-8.f };
	IG_DATA::LIGHTS[3].is_on = true;
	IG_DATA::LIGHTS[3].color = Color(255, 255, 0, 255).getColor4();
	IG_DATA::LIGHTS[3].intensities = { 60.f,10.f };
	IG_DATA::LIGHTS[3].position = { 8.f,0.f,8.f };

	Vector3f vertexs[8] = {
	Vector3f( 1.f, 1.f, 1.f),
	Vector3f(-1.f, 1.f, 1.f),
	Vector3f(-1.f,-1.f, 1.f),
	Vector3f( 1.f,-1.f, 1.f),
	Vector3f( 1.f, 1.f,-1.f),
	Vector3f(-1.f, 1.f,-1.f),
	Vector3f(-1.f,-1.f,-1.f),
	Vector3f( 1.f,-1.f,-1.f),
	};

	Vector3i triangles[12] = {
		Vector3i(0, 1, 2),
		Vector3i(2, 3, 0),
		Vector3i(4, 5, 6),
		Vector3i(6, 7, 4),
		Vector3i(0, 1, 4),
		Vector3i(4, 5, 1),
		Vector3i(2, 3, 6),
		Vector3i(6, 7, 3),
		Vector3i(0, 3, 4),
		Vector3i(3, 4, 7),
		Vector3i(1, 2, 5),
		Vector3i(2, 5, 6),
	};

	Color colors[12] = {
		Color(255,255,255, 200),
		Color(255,255,255, 200),
		Color(255,255,255, 200),
		Color(255,255,255, 200),
		Color(255,255,255, 200),
		Color(255,255,255, 200),
		Color(255,255,255, 200),
		Color(255,255,255, 200),
		Color(255,255,255, 200),
		Color(255,255,255, 200),
		Color(255,255,255, 200),
		Color(255,255,255, 200),
	};

	Vector3f vertexs0[8] = {
		Vector3f(0.f, 0.f, 1.f),
		Vector3f(cosf(2 * 3.14159f * 0 / 6) ,-sinf(2 * 3.14159f * 0 / 6), 0.f),
		Vector3f(cosf(2 * 3.14159f * 1 / 6) ,-sinf(2 * 3.14159f * 1 / 6), 0.f),
		Vector3f(cosf(2 * 3.14159f * 2 / 6) ,-sinf(2 * 3.14159f * 2 / 6), 0.f),
		Vector3f(cosf(2 * 3.14159f * 3 / 6) ,-sinf(2 * 3.14159f * 3 / 6), 0.f),
		Vector3f(cosf(2 * 3.14159f * 4 / 6) ,-sinf(2 * 3.14159f * 4 / 6), 0.f),
		Vector3f(cosf(2 * 3.14159f * 5 / 6) ,-sinf(2 * 3.14159f * 5 / 6), 0.f),
		Vector3f(0.f, 0.f,-1.f),
	};

	Vector3i triangles0[12] = {
		Vector3i(0, 1, 2),
		Vector3i(0, 2, 3),
		Vector3i(0, 3, 4),
		Vector3i(0, 4, 5),
		Vector3i(0, 5, 6),
		Vector3i(0, 6, 1),
		Vector3i(7, 1, 2),
		Vector3i(7, 2, 3),
		Vector3i(7, 3, 4),
		Vector3i(7, 4, 5),
		Vector3i(7, 5, 6),
		Vector3i(7, 6, 1),
	};

	SURFACE_SHAPE ss0 = { _EXPLICIT_SPHERICAL, exampleRadius };
	ss0.numU = 200u;
	ss0.numV = 200u;

	SURFACE_COLORING sc = {};

	sc.color = Color(255, 255, 255, 127);
	sc.transparency = true;

	SURFACE_SHAPE ss1 = { _PARAMETRIC, KleinBottle, false, Vector2f(0, 0), Vector2f(pi, 2 * pi) };
	ss1.numU = 200u;
	ss1.numV = 200u;
	
	shape_0.create(window.graphics, ss0, &sc);
	shape_1.create(window.graphics, vertexs, triangles, 12, colors);
	shape_2.create(window.graphics, vertexs0, triangles0, 12, colors);
	shape_3.create(window.graphics, ss1, &sc);
}

int QuaternionMotion::Run()
{
	while (window.processEvents())
		doFrame();
	return 0;
}

void QuaternionMotion::eventManager()
{
	// returning motion

	if (Keyboard::isKeyPressed('R'))
		magneticReturn();

	if (Keyboard::isKeyPressed('T'))
		strictReturn();

	if (Keyboard::isKeyPressed('S'))
		dangle = 0.f;


	if (Mouse::isButtonPressed(Mouse::Left) && !dragging)
	{
		dragging = true;
		lastPos = Mouse::getPosition();
	}

	if (dragging && !Mouse::isButtonPressed(Mouse::Left))
		dragging = false;

	if (dragging)
	{
		switch (IG_DATA::TYPE)
		{
		case RIGID_PLANE:
			drag_rigid_plane();
			break;
		case RIGID_SPACE:
			drag_rigid_space();
			break;
		case DYNAMIC_PLANE:
			drag_dynamic_plane();
			break;
		case DYNAMIC_SPACE:
			drag_dynamic_space();
			break;
		case MAGNETIC_MOUSE:
			drag_magnetic_mouse();
			break;
		default:
			break;
		}
	}
	else
		scale *= powf(1.1f, Mouse::getWheel() / 120.f);

	//	Calculate observer vector

	observer = {
			-cosf(IG_DATA::PHI) * cosf(IG_DATA::THETA) ,
			-cosf(IG_DATA::PHI) * sinf(IG_DATA::THETA) ,
			-sinf(IG_DATA::PHI)
	};

	//	Light updates
	
	int l = IG_DATA::UPDATE_LIGHT;
	if (l == -2) {
		for (int i = 0; i < 8; i++) {
			DO(updateLight(window.graphics, i, IG_DATA::LIGHTS[i].intensities, IG_DATA::LIGHTS[i].color, IG_DATA::LIGHTS[i].position));
		}
		IG_DATA::UPDATE_LIGHT = -1;
	}
	if (l > -1) {
		DO(updateLight(window.graphics, l, IG_DATA::LIGHTS[l].intensities, IG_DATA::LIGHTS[l].color, IG_DATA::LIGHTS[l].position));
		IG_DATA::UPDATE_LIGHT = -1;
	}
}

void QuaternionMotion::doFrame()
{
	eventManager();

	//	Update objects

	window.graphics.updatePerspective(observer, center, scale);

	DO(updateRotation(window.graphics, axis, dangle, true));

	const char* rot = shape_0.getRotation().str();
	window.setTitle(std::string(rot) + "  -  " + std::to_string(int(std::round(window.getFramerate()))) + "fps");
	free((void*)rot);

	//	Rendering

	window.graphics.clearBuffer(Color::Black);

	switch (IG_DATA::FIGURE)
	{
	case SQUARE:
		shape_1.Draw(window.graphics);
		break;
	case WEIRD_SHAPE:
		shape_0.Draw(window.graphics);
		break;
	case OCTAHEDRON:
		shape_2.Draw(window.graphics);
		break;
	case KLEIN:
		shape_3.Draw(window.graphics);
	default:
		break;
	}
	

	//	ImGui crap

	IG_QuaternionMotion::render();

	//	Push the frame to the scriin

	window.graphics.pushFrame();
}

//	Movement functions

void QuaternionMotion::magneticReturn()
{
	Quaternion rot = shape_0.getRotation();
	if (rot.r < 0)rot = -rot;
	constexpr float pull = 0.03f;

	float angle = 2 * acosf(rot.r);
	Vector3f axisRot = rot.getVector();

	Quaternion newRot;
	if (angle < pull)
		newRot = rotationQuaternion(axisRot, -pull / 40.f) * rotationQuaternion(axis, dangle);
	else
		newRot = rotationQuaternion(axisRot, -angle / 40.f) * rotationQuaternion(axis, dangle);

	axis = newRot.getVector();
	dangle = 2 * acos(newRot.r);

	dangle *= 0.95f;

	if (fabs(angle) < 0.01f && fabs(dangle) < 0.005f)
	{
		shape_0.updateRotation(window.graphics, Vector3f(), 0.f);
		shape_1.updateRotation(window.graphics, Vector3f(), 0.f);
		shape_2.updateRotation(window.graphics, Vector3f(), 0.f);
		returning = false;
		dangle = 0.f;
	}
}

void QuaternionMotion::strictReturn()
{
	dangle = 0;

	Quaternion rot = shape_0.getRotation();
	if (rot.r < 0)rot = -rot;

	float angle = 2 * acosf(rot.r);
	Vector3f axisRot = rot.getVector();

	axis = axisRot;
	dangle = -powf(angle, 0.65f) / 40.f;

	if (fabs(angle) < 0.01f)
	{
		shape_0.updateRotation(window.graphics, Vector3f(), 0.f);
		shape_1.updateRotation(window.graphics, Vector3f(), 0.f);
		shape_2.updateRotation(window.graphics, Vector3f(), 0.f);
		strict = false;
		dangle = 0.f;
	}
}

void QuaternionMotion::drag_rigid_plane()
{
	Vector3f obs = window.graphics.getObserver();
	Vector3f ex = -(obs * Vector3f(0.f, 0.f, 1.f)).normalize();
	Vector3f ey = ex * obs;

	Vector2i movement = Mouse::getPosition() - lastPos;
	lastPos = Mouse::getPosition();

	if (!movement)
	{
		if (axis != obs)
		{
			axis = obs;
			dangle = 0.f;
		}
		else
			dangle += Mouse::getWheel() / 18000.f;
	}
	else
	{
		axis = movement.y * ex - movement.x * ey;
		dangle = movement.abs() / scale;
	}
}

void QuaternionMotion::drag_rigid_space()
{
	Vector3f obs = window.graphics.getObserver();
	Vector3f ex = -(obs * Vector3f(0.f, 0.f, 1.f)).normalize();
	Vector3f ey = ex * obs;

	Vector3f lastMouse = (-obs + (ex * (lastPos.x - window.getDimensions().x / 2) + ey * (lastPos.y - window.getDimensions().y / 2)) / scale).normalize();
	lastPos = Mouse::getPosition();
	Vector3f newMouse = (-obs + (ex * (lastPos.x - window.getDimensions().x / 2) + ey * (lastPos.y - window.getDimensions().y / 2)) / scale).normalize();

	if (lastMouse == newMouse)
	{
		if (axis != newMouse)
		{
			axis = newMouse;
			dangle = 0.f;
		}
		else
			dangle += Mouse::getWheel() / 18000.f;
	}
	else
	{
		axis = lastMouse * newMouse;
		dangle = -acosf(lastMouse ^ newMouse);
	}
}

void QuaternionMotion::drag_dynamic_plane()
{
	Vector3f obs = window.graphics.getObserver();
	Vector3f ex = -(obs * Vector3f(0.f, 0.f, 1.f)).normalize();
	Vector3f ey = ex * obs;

	Vector2i movement = Mouse::getPosition() - lastPos;
	lastPos = Mouse::getPosition();

	Vector3f desiredAxis;
	float desiredAngle;

	if (!movement)
	{
		desiredAxis = obs;
		desiredAngle = Mouse::getWheel() / 18000.f;
	}
	else
	{
		desiredAxis = (movement.y * ex - movement.x * ey).normalize();
		desiredAngle = movement.abs() / scale;
	}

	Vector3f preAxis = axis;
	axis = (axis * dangle + desiredAxis * desiredAngle);
	if (axis)axis.normalize();
	else axis = desiredAxis;
	dangle *= axis ^ preAxis;
	dangle += (desiredAxis ^ axis) * (desiredAngle - dangle) / 20.f;

	if (dangle > 0.2f)dangle = 0.2f;
	if (dangle < -0.2f)dangle = -0.2f;
}

void QuaternionMotion::drag_dynamic_space()
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

void QuaternionMotion::drag_magnetic_mouse()
{
	Vector3f obs = window.graphics.getObserver();
	Vector3f ex = -(obs * Vector3f(0.f, 0.f, 1.f)).normalize();
	Vector3f ey = ex * obs;

	lastPos = Mouse::getPosition();
	Vector3f newMouse = (-obs + (ex * (lastPos.x - window.getDimensions().x / 2) + ey * (lastPos.y - window.getDimensions().y / 2)) / scale).normalize();

	Quaternion rot = shape_0.getRotation();
	Vector3f polePos = (rot * Quaternion(Vector3f(0.f, 1.f, 0.f)) * Quaternion(rot.r, -rot.i, -rot.j, -rot.k)).getVector();

	Vector3f axis0 = newMouse * polePos;
	if (!axis0) axis0 = newMouse;
	axis0.normalize();

	constexpr float max = 0.02f;
	float dangle0 = acosf(polePos ^ newMouse);
	if ((polePos ^ newMouse) > 1.f)dangle0 = 0.f;
	if (polePos == newMouse) dangle0 = 0.f;
	dangle0 /= 40.f;
	if (dangle0 > max)
		dangle0 = max;
	else if (dangle0 < -max)
		dangle0 = -max;

	dangle *= 0.97f;

	Quaternion newRot = rotationQuaternion(axis0, dangle0) * rotationQuaternion(axis, dangle);

	dangle = 2 * acosf(newRot.r);
	if (newRot.r > 1)
		dangle = 0.f;
	axis = newRot.getVector();
	if (!axis)axis = newMouse;
	axis.normalize();

	Mouse::getWheel();
}

//	Functions

float exampleRadius(float theta, float phi)
{
	return 1.f + (sinf(theta) * sinf(theta) + cosf(phi) * sinf(phi)) * sinf(5 * theta) * cosf(3 * phi) * cosf(phi) / 2.f;
}

Vector3f KleinBottle(float u, float v)
{
	float c_u = cosf(u);
	float s_u = sinf(u);
	float c_v = cosf(v);
	float s_v = sinf(v);

	float x = -2.f / 15.f * c_u * (3 * c_v - 30 * s_u + 90 * powf(c_u, 4.f) * s_u - 60 * powf(c_u, 6.f) * s_u + 5 * c_u * c_v * s_u);
	float y = -1.f / 15.f * s_u * (3 * c_v - 3 * c_u * c_u * c_v - 48 * powf(c_u, 4.f) * c_v + 48 * powf(c_u, 6.f) * c_v - 60 * s_u + 5 * c_u * c_v * s_u - 5 * powf(c_u, 3) * c_v * s_u - 80 * powf(c_u, 7.f) * c_v * s_u) - 1;
	float z = 2.f / 15.f * (3 + 5 * c_u * s_u) * s_v;
	return Vector3f(x, y, z);
}