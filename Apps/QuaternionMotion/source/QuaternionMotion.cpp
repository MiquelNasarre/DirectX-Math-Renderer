#include "QuaternionMotion.h"
#include "IG_QuaternionMotion.h"

#include "Keyboard.h"
#include "Mouse.h"

#include <cmath>

QuaternionMotion::QuaternionMotion()
	: desc{ "QuaternionMotion", WINDOW_DESC::WINDOW_MODE_NORMAL, { 640, 480 }  },
	window(&desc)
{
	window.setFramerateLimit(60);
#ifdef _INCLUDE_IMGUI
	imGui.bind(window);
#endif
	IG_DATA::LIGHTS[0].is_on = true;
	IG_DATA::LIGHTS[0].color = Color(255, 51, 51, 255);
	IG_DATA::LIGHTS[0].intensities = { 60.f,10.f };
	IG_DATA::LIGHTS[0].position = { 0.f,8.f,8.f };
	IG_DATA::LIGHTS[1].is_on = true;
	IG_DATA::LIGHTS[1].color = Color(0, 255, 0, 255);
	IG_DATA::LIGHTS[1].intensities = { 60.f,10.f };
	IG_DATA::LIGHTS[1].position = { 0.f,-8.f,8.f };
	IG_DATA::LIGHTS[2].is_on = true;
	IG_DATA::LIGHTS[2].color = Color(127, 0, 255, 255);
	IG_DATA::LIGHTS[2].intensities = { 60.f,10.f };
	IG_DATA::LIGHTS[2].position = { -8.f,0.f,-8.f };
	IG_DATA::LIGHTS[3].is_on = true;
	IG_DATA::LIGHTS[3].color = Color(255, 255, 0, 255);
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

	Color colors[36] = {};

	for (Color& c : colors)
		c = Color(rand() % 256, rand() % 256, rand() % 256, 128);

	Image grass_tex("resources/grass_block.bmp");
	Image cube_map("resources/cube_park.bmp");

	Vector2i texture_coord[36] =
	{
		{ 16, 00 }, { 16, 16 }, { 32, 16 },
		{ 32, 16 }, { 32, 00 }, { 16, 00 },

		{ 32, 00 }, { 32, 16 }, { 48, 16 },
		{ 48, 16 }, { 48, 00 }, { 32, 00 },

		{ 00, 00 }, { 16, 00 }, { 00, 16 },
		{ 00, 16 }, { 16, 16 }, { 16, 00 },

		{ 00, 00 }, { 16, 00 }, { 00, 16 },
		{ 00, 16 }, { 16, 16 }, { 16, 00 },

		{ 00, 00 }, { 16, 00 }, { 00, 16 },
		{ 16, 00 }, { 00, 16 }, { 16, 16 },

		{ 00, 00 }, { 16, 00 }, { 00, 16 },
		{ 16, 00 }, { 00, 16 }, { 16, 16 },
	};

	SURFACE_DESC surfDesc0 = {};
	surfDesc0.type = SURFACE_DESC::IMPLICIT_SURFACE;
	surfDesc0.implicit_func = [](float x, float y, float z) { return 2 * x * x + 2 * y * y - z * z - 1.f; };
	surfDesc0.wire_frame_topology = true;

	shape_0.initialize(&surfDesc0);

	POLIHEDRON_DESC desc = {};
	desc.coloring = POLIHEDRON_DESC::TEXTURED_COLORING;
	desc.pixelated_texture = true;
	desc.texture_image = &grass_tex;
	desc.texture_coordinates_list = texture_coord;
	desc.color_list = colors;
	desc.triangle_count = 12;
	desc.triangle_list = triangles;
	desc.enable_iluminated = false;
	desc.vertex_list = vertexs;

	shape_1.initialize(&desc);

	POLIHEDRON_DESC desc0 = {};
	desc0.global_color = Color(255, 255, 255, 128);
	desc0.triangle_count = 12;
	desc0.triangle_list = triangles0;
	desc0.vertex_list = vertexs0;
	desc0.enable_transparency = true;

	shape_2.initialize(&desc0);

	BACKGROUND_DESC backDesc = {};
	backDesc.override_buffers = true;
	backDesc.image = &cube_map;
	backDesc.pixelated_texture = false;
	backDesc.texture_updates = false;
	backDesc.type = BACKGROUND_DESC::DYNAMIC_BACKGROUND;

	back.initialize(&backDesc);

	CURVE_DESC curveDesc = {};
	curveDesc.coloring = CURVE_DESC::FUNCTION_COLORING;
	curveDesc.curve_function = [](float t)-> Vector3f { return { cosf(10 * t), sinf(10 * t), t }; };
	curveDesc.color_function = [](float t) { return Color((unsigned char)((cosf(10 * t) + 1) * 128), (unsigned char)((sinf(10 * t) + 1) * 128), (unsigned char)((t + 1) * 128)); };

	curve.initialize(&curveDesc);

	LIGHT_DESC lightDesc = {};
	lightDesc.color = Color::Blue;
	lightDesc.intensity = 2.f;
	lightDesc.position = { 2.f, 0.f, 0.f };

	light.initialize(&lightDesc);

	SURFACE_DESC surfDesc = {};
	surfDesc.type = SURFACE_DESC::PARAMETRIC_SURFACE;
	surfDesc.parametric_func = &KleinBottle;
	surfDesc.range_u = { 0.f, MATH_PI };
	surfDesc.range_v = { 0.f,2.f * MATH_PI };
	surfDesc.num_u = 50u;
	surfDesc.num_v = 50u;
	surfDesc.wire_frame_topology = true;

	shape_3.initialize(&surfDesc);

	window.graphics().enableOITransparency();
}

int QuaternionMotion::Run()
{
	while (!window.processEvents())
		doFrame();
	return 0;
}

void QuaternionMotion::eventManager()
{
	//	Calculate observer vector

	Vector3f up = { 0.f,1.f,0.f };

	Quaternion rotUp = (up.normal().y > -0.99999f) ? Quaternion{ 1.f + up.y, -up.z, 0.f, up.x } : Quaternion{ 0.f, 0.f, 0.f, 1.f };

	Quaternion rotTheta = { cosf(IG_DATA::THETA / 2.f), 0.f, sinf(IG_DATA::THETA / 2.f), 0.f };
	Quaternion rotPhi   = { cosf(  IG_DATA::PHI / 2.f),-sinf(  IG_DATA::PHI / 2.f), 0.f, 0.f };

	observer = (rotPhi * rotTheta * rotUp).normal();

	// returning motion

	if (Keyboard::isKeyPressed('R'))
		magneticReturn();

	if (Keyboard::isKeyPressed('T'))
		strictReturn();

	if (Keyboard::isKeyPressed('S'))
		rotation = 1.f;


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

	//	Light updates
	
	int l = IG_DATA::UPDATE_LIGHT;
	if (l == -2) {
		for (int i = 0; i < 8; i++) 
		{
			shape_0.updateLight(i, IG_DATA::LIGHTS[i].intensities, IG_DATA::LIGHTS[i].color, IG_DATA::LIGHTS[i].position);
			shape_1.updateLight(i, IG_DATA::LIGHTS[i].intensities, IG_DATA::LIGHTS[i].color, IG_DATA::LIGHTS[i].position);
			shape_2.updateLight(i, IG_DATA::LIGHTS[i].intensities, IG_DATA::LIGHTS[i].color, IG_DATA::LIGHTS[i].position);
			shape_3.updateLight(i, IG_DATA::LIGHTS[i].intensities, IG_DATA::LIGHTS[i].color, IG_DATA::LIGHTS[i].position);
		}
		IG_DATA::UPDATE_LIGHT = -1;
	}
	if (l > -1) {
		shape_0.updateLight(l, IG_DATA::LIGHTS[l].intensities, IG_DATA::LIGHTS[l].color, IG_DATA::LIGHTS[l].position);
		shape_1.updateLight(l, IG_DATA::LIGHTS[l].intensities, IG_DATA::LIGHTS[l].color, IG_DATA::LIGHTS[l].position);
		shape_2.updateLight(l, IG_DATA::LIGHTS[l].intensities, IG_DATA::LIGHTS[l].color, IG_DATA::LIGHTS[l].position);
		shape_3.updateLight(l, IG_DATA::LIGHTS[l].intensities, IG_DATA::LIGHTS[l].color, IG_DATA::LIGHTS[l].position);
		IG_DATA::UPDATE_LIGHT = -1;
	}
}

void QuaternionMotion::doFrame()
{
	eventManager();

	//	Update objects
	
	window.graphics().setRenderTarget();

	window.graphics().updatePerspective(observer, center, scale);

	shape_0.updateRotation(rotation, true);
	shape_1.updateRotation(rotation, true);
	shape_2.updateRotation(rotation, true);
	shape_3.updateRotation(rotation, true);
	back.updateRotation(rotation, true);
	curve.updateRotation(rotation, true);

	window.setTitle("%s  -  %u fps", shape_1.getRotation().str(), (unsigned)(window.getFramerate() + 0.5f));

	//	Rendering

	switch (IG_DATA::FIGURE)
	{
	case SQUARE:
		window.graphics().clearTransparencyBuffers();
		back.Draw();
		shape_1.Draw();
		break;
	case WEIRD_SHAPE:
		window.graphics().clearBuffer(Color::Black);
		shape_0.Draw();
		curve.Draw();
		light.Draw();
		break;
	case OCTAHEDRON:	
		shape_2.updateDistortion(Matrix(250.f / window.graphics().getScale()));
		
		window.graphics().clearBuffer(Color::Black);
		shape_2.Draw();
		light.Draw();
		break;

	case KLEIN:
		window.graphics().clearBuffer(Color::Black);
		shape_3.Draw();
		break;
	default:
		break;
	}

	//	Push the frame to the scriin

	window.graphics().pushFrame();
}

//	Movement functions

void QuaternionMotion::magneticReturn()
{
	constexpr float drag = 0.05f, e = 1e-8f, force = 0.04f;

	Quaternion attraction = (shape_0.getRotation().inv() + 1.f / force);
	
	rotation = (attraction * rotation).normal() + drag;
}

void QuaternionMotion::strictReturn()
{
	Quaternion rot = shape_0.getRotation().inv();
	
	rotation = (rot.r > 0.f) ? rot + 16.f : -rot + 16.f;
}

void QuaternionMotion::drag_rigid_plane()
{
	Vector3f obs = -(observer * QUAT_K * observer.inv()).getVector();
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
	Vector3f obs = -(observer * QUAT_K * observer.inv()).getVector();
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
	Vector3f obs = -(observer * QUAT_K * observer.inv()).getVector();
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
	Vector2i dim = window.getDimensions() / 2;

	Vector3f p0 = { (lastPos.x - dim.x) / scale, -(lastPos.y - dim.y) / scale, -1.f };

	lastPos = Mouse::getPosition();
	Vector3f p1 = { (lastPos.x - dim.x) / scale, -(lastPos.y - dim.y) / scale, -1.f };

	p0 = (observer.inv() * Quaternion(p0) * observer).getVector().normal();
	p1 = (observer.inv() * Quaternion(p1) * observer).getVector().normal();

	Quaternion rot = (Quaternion(p1 * p0) + 1.f + (p0 ^ p1)).normal();

	Quaternion wheel_spin = rotationQuaternion(p1, Mouse::getWheel() / 18000.f);
	Quaternion momentum = fabsf(rotation.r) < 1.f - 1e-6f ? (rotation + (1.f - fabsf(rotation.getVector().normal() ^ p1))).normal() : Quaternion(1.f);

	rotation = wheel_spin * rot * momentum;
}

void QuaternionMotion::drag_magnetic_mouse()
{
	Vector2i dim = window.getDimensions() / 2;

	lastPos = Mouse::getPosition();
	Vector3f p1 = { (lastPos.x - dim.x) / scale, -(lastPos.y - dim.y) / scale, -1.f };

	p1 = (observer * Quaternion(p1) * observer.inv()).getVector().normal();

	Quaternion rot = shape_0.getRotation();
	Vector3f polePos = (rot * QUAT_K * rot.inv()).getVector();

	constexpr float weakness = 32.f, drag = 0.05f;
	rotation = ((Quaternion(p1 * polePos) + (polePos ^ p1) + weakness) * (rotation + drag)).normal();

	Mouse::getWheel();
}

//	Functions

float exampleRadius(float x, float y, float z)
{
	if (z == 1.f || z == -1.f)
		return z * 0.65f;

	float costheta = z;
	float sintheta = sqrtf(1 - z * z);
	float cosphi = x / sintheta;
	float sinphi = y / sintheta;

	return 1.f + (sintheta * sintheta + cosphi * sinphi) * sinf(5 * costheta) * cosf(3 * cosphi) * cosphi / 2.f;
}

Vector3f KleinBottle(float u, float v)
{
	float c_u = cosf(u);
	float s_u = sinf(u);
	float c_v = cosf(v);
	float s_v = sinf(v);

	float c_u3 = c_u * c_u * c_u;
	float c_u4 = c_u3 * c_u;

	float x = -2.f / 15.f * c_u * (3 * c_v - 30 * s_u + 90 * c_u4 * s_u - 60 * c_u3 * c_u3 * s_u + 5 * c_u * c_v * s_u);
	float y = -1.f / 15.f * s_u * (3 * c_v - 3 * c_u * c_u * c_v - 48 * c_u4 * c_v + 48 * c_u3 * c_u3 * c_v - 60 * s_u + 5 * c_u * c_v * s_u - 5 * c_u3 * c_v * s_u - 80 * c_u3 * c_u4 * c_v * s_u) - 2;
	float z = 2.f / 15.f * (3 + 5 * c_u * s_u) * s_v;
	return { x, y, z };
}