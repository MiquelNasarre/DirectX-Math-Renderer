#include "IG_QuaternionMotion.h"
#include "QuaternionMotion.h"
#include "imgui/imgui.h"
#include <string>

DRAG_TYPE	IG_DATA::TYPE			= DYNAMIC_SPACE;
SHAPE		IG_DATA::FIGURE			= SQUARE;
int			IG_DATA::UPDATE_LIGHT	= -1;
float		IG_DATA::THETA			= MATH_PI / 2.f;
float		IG_DATA::PHI			= 0.f;

IG_DATA::lightsource* IG_DATA::LIGHTS = (IG_DATA::lightsource*)calloc(sizeof(IG_DATA::lightsource), 8);

static IG_DATA::lightsource savestate;

void IG_QuaternionMotion::saveLightState(int id)
{
	savestate = IG_DATA::LIGHTS[id];
}

void IG_QuaternionMotion::loadLightState(int id)
{
	IG_DATA::LIGHTS[id] = savestate;
}

void IG_QuaternionMotion::doLightEditor(int& id)
{
	if (ImGui::Begin(" Light editor", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
	{
		ImGui::Text("Color:");
		if (ImGui::ColorPicker4("", (float*)&IG_DATA::LIGHTS[id].color))
			IG_DATA::UPDATE_LIGHT = id;
		ImGui::Spacing();
		ImGui::Text("Intensities:");
		if (ImGui::SliderFloat2("", (float*)&IG_DATA::LIGHTS[id].intensities, 0.f, 1000.f, "%.3f", 4))
			IG_DATA::UPDATE_LIGHT = id;
		ImGui::Spacing();
		ImGui::Text("Position:");
		if (ImGui::SliderFloat3(" ", (float*)&IG_DATA::LIGHTS[id].position, -100.f, 100.f, "%.3f", 4))
			IG_DATA::UPDATE_LIGHT = id;

		ImGui::SetCursorPos(ImVec2(290, 285));
		if (ImGui::Button("Cancel", ImVec2(80, 45))) {
			loadLightState(id);
			IG_DATA::UPDATE_LIGHT = id;
			id = -1;
		}
		ImGui::SetCursorPos(ImVec2(290, 340));
		if (ImGui::Button("Delete", ImVec2(80, 45))) {
			IG_DATA::LIGHTS[id].is_on = false;
			IG_DATA::LIGHTS[id].position = {};
			IG_DATA::LIGHTS[id].intensities = {};
			IG_DATA::LIGHTS[id].color = {};
			IG_DATA::UPDATE_LIGHT = id;
			id = -1;
		}
		ImGui::SetCursorPos(ImVec2(290, 395));
		if (ImGui::Button("Apply", ImVec2(80, 45)))
			id = -1;

	}
	ImGui::End();
}

void IG_QuaternionMotion::render()
{
	static int light = -1;

	newFrame();

	if (ImGui::Begin("Settings", NULL, ImGuiWindowFlags_MenuBar))
	{
		ImGui::SetWindowPos(ImVec2(2, 2), ImGuiCond_Once);
		ImGui::SetWindowCollapsed(true, ImGuiCond_Once);
		ImGui::SetWindowSize(ImVec2(315, 120), ImGuiCond_Once);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Motion"))
			{
				if (ImGui::MenuItem("Plain rigid"))
					IG_DATA::TYPE = RIGID_PLANE;

				if (ImGui::MenuItem("Space rigid"))
					IG_DATA::TYPE = RIGID_SPACE;

				if (ImGui::MenuItem("Plain dynamic"))
					IG_DATA::TYPE = DYNAMIC_PLANE;

				if (ImGui::MenuItem("Space dynamic"))
					IG_DATA::TYPE = DYNAMIC_SPACE;

				if (ImGui::MenuItem("Magnetic mouse"))
					IG_DATA::TYPE = MAGNETIC_MOUSE;

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Shape"))
			{
				if (ImGui::MenuItem("Square"))
					IG_DATA::FIGURE = SQUARE;

				if (ImGui::MenuItem("Weird shape"))
					IG_DATA::FIGURE = WEIRD_SHAPE;

				if (ImGui::MenuItem("Octahedron"))
					IG_DATA::FIGURE = OCTAHEDRON;

				if (ImGui::MenuItem("Klein Bottle"))
					IG_DATA::FIGURE = KLEIN;

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Lights"))
			{
				for (int i = 0; i < 8; i++) {
					if (IG_DATA::LIGHTS[i].is_on && ImGui::MenuItem(std::string("Light " + std::to_string(i)).c_str()))
					{
						light = i;
						saveLightState(i);
					}
				}
				ImGui::Spacing();

				if (ImGui::MenuItem("New"))
				{
					for (int i = 0; i < 8; i++) {
						if (!IG_DATA::LIGHTS[i].is_on) {
							IG_DATA::LIGHTS[i].is_on = true;
							light = i;
							break;
						}
					}
				}

				if (ImGui::MenuItem("Reset"))
				{
					free(IG_DATA::LIGHTS);
					IG_DATA::LIGHTS = (IG_DATA::lightsource*)calloc(8, sizeof(IG_DATA::lightsource));
					IG_DATA::LIGHTS[0].is_on = true;
					IG_DATA::LIGHTS[0].color = Color(255,51,51,255);
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

					IG_DATA::UPDATE_LIGHT = -2;
					light = -1;
				}

				if (ImGui::MenuItem("Clear"))
				{
					free(IG_DATA::LIGHTS);
					IG_DATA::LIGHTS = (IG_DATA::lightsource*)calloc(8, sizeof(IG_DATA::lightsource));
					IG_DATA::UPDATE_LIGHT = -2;
					light = -1;
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::Spacing();
		ImGui::Text("Observer/Camera position:");

		ImGui::SliderAngle("Theta", &IG_DATA::THETA);
		ImGui::SliderAngle("Phi", &IG_DATA::PHI, -90.f, 90.f);

		if (IG_DATA::PHI > 3.140f / 2.f)
			IG_DATA::PHI = 3.140f / 2.f;
		if (IG_DATA::PHI < -3.140f / 2.f)
			IG_DATA::PHI = -3.140f / 2.f;

	}
	ImGui::End();

	if (light > -1)
		doLightEditor(light);

	drawFrame();
}
