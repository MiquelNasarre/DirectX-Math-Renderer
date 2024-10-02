#include "IG_EarthDemo.h"

#include "imgui/imgui.h"
#include "EarthDemo.h"

#include <string>

static IG_DATA::lightsource savestate;

void IG_EarthDemo::saveLightState(int id)
{
	savestate = IG_DATA::LIGHTS[id];
}

void IG_EarthDemo::loadLightState(int id)
{
	IG_DATA::LIGHTS[id] = savestate;
}

void IG_EarthDemo::doLightEditor(int& id)
{
	if (ImGui::Begin(" Light editor", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
	{
		ImGui::Text("Color:");
		if (ImGui::ColorPicker4("", (float*)&IG_DATA::LIGHTS[id].color))
			IG_DATA::UPDATE_LIGHT = id;
		ImGui::Spacing();
		ImGui::Text("Intensities:");
		if (ImGui::SliderFloat2("", (float*)&IG_DATA::LIGHTS[id].intensities, 0.f, 100000.f, "%.3f", 4))
			IG_DATA::UPDATE_LIGHT = id;
		ImGui::Spacing();
		ImGui::Text("Position:");
		if (ImGui::SliderFloat3(" ", (float*)&IG_DATA::LIGHTS[id].position, -1000.f, 1000.f, "%.3f", 4))
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
			IG_DATA::LIGHTS[id].position = { 0.f,0.f,0.f,0.f };
			IG_DATA::LIGHTS[id].intensities = { 0.f,0.f,0.f,0.f };
			IG_DATA::LIGHTS[id].color = { 0.f,0.f,0.f,0.f };
			IG_DATA::UPDATE_LIGHT = id;
			id = -1;
		}
		ImGui::SetCursorPos(ImVec2(290, 395));
		if (ImGui::Button("Apply", ImVec2(80, 45)))
			id = -1;

	}
	ImGui::End();
}

void IG_EarthDemo::render()
{
	static int light = -1;

	newFrame();

	//	Add all imGui functionalities here

	if (ImGui::Begin("Settings", NULL, ImGuiWindowFlags_MenuBar)) {

	ImGui::SetWindowCollapsed(true, ImGuiCond_Once);
	ImGui::SetWindowSize(ImVec2(616, 258), ImGuiCond_Once);
	ImGui::SetWindowPos(ImVec2(2, 2), ImGuiCond_Once);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Textures"))
			{
				if (ImGui::BeginMenu("Earth"))
				{
					if (ImGui::MenuItem("Default"))
						IG_DATA::UPDATE_TEXTURE = Vector2i(0, 0);
					if (ImGui::MenuItem("Inverted"))
						IG_DATA::UPDATE_TEXTURE = Vector2i(0, 1);
					if (ImGui::MenuItem("Chalked"))
						IG_DATA::UPDATE_TEXTURE = Vector2i(0, 2);
					if (ImGui::MenuItem("Moon"))
						IG_DATA::UPDATE_TEXTURE = Vector2i(0, 3);
					if (ImGui::MenuItem("No night"))
						IG_DATA::UPDATE_TEXTURE = Vector2i(0, 4);
					if (ImGui::MenuItem("Only night"))
						IG_DATA::UPDATE_TEXTURE = Vector2i(0, 5);

					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Moon"))
				{
					if (ImGui::MenuItem("Default"))
						IG_DATA::UPDATE_TEXTURE = Vector2i(1, 0);
					if (ImGui::MenuItem("Inverted"))
						IG_DATA::UPDATE_TEXTURE = Vector2i(1, 1);
					if (ImGui::MenuItem("Chalked"))
						IG_DATA::UPDATE_TEXTURE = Vector2i(1, 2);
					if (ImGui::MenuItem("Earth"))
						IG_DATA::UPDATE_TEXTURE = Vector2i(1, 3);

					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Background"))
				{
					if (ImGui::MenuItem("Default"))
						IG_DATA::UPDATE_TEXTURE = Vector2i(2, 0);
					if (ImGui::MenuItem("Inverted"))
						IG_DATA::UPDATE_TEXTURE = Vector2i(2, 1);
					if (ImGui::MenuItem("Earth"))
						IG_DATA::UPDATE_TEXTURE = Vector2i(2, 2);
					if (ImGui::MenuItem("Moon"))
						IG_DATA::UPDATE_TEXTURE = Vector2i(2, 3);

					ImGui::EndMenu();
				}
				ImGui::Spacing();
				if (ImGui::MenuItem("Reset"))
					IG_DATA::UPDATE_TEXTURE = Vector2i(3, 0);

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
					IG_DATA::LIGHTS[0].color = Color::White.getColor4();
					IG_DATA::LIGHTS[0].intensities = { 32000.f,5000.f };
					IG_DATA::LIGHTS[0].position = { 160.f,0.f,60.f };
					IG_DATA::UPDATE_LIGHT = -2;
					light = -1;
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::Spacing();
		ImGui::Text("Earth & Moon movement:");

		ImGui::SliderFloat("Speed", &IG_DATA::SPEED, -1.5f, 1.5f, "%0.3f");
		ImGui::SliderAngle("Rotation", &IG_DATA::EARTH_THETA);
		ImGui::SliderAngle("Pitch", &IG_DATA::EARTH_PHI, -90.f, 90.f);
		ImGui::SliderFloat("Moon Speed", &IG_DATA::MOON_SPEED, -4.f, 4.f, "%0.3f");

		ImGui::Spacing();
		ImGui::Text("Observer/Camera position:");

		ImGui::SliderAngle("Theta", &IG_DATA::THETA);
		ImGui::SliderAngle("Phi", &IG_DATA::PHI, -90.f, 90.f);
		ImGui::SliderFloat("Background View", &IG_DATA::FOV, 0.2f, 2.f);


		if (IG_DATA::EARTH_THETA > 2.f * 3.141593f)
			IG_DATA::EARTH_THETA -= 4.f * 3.141593f;
		if (IG_DATA::EARTH_THETA < -2.f * 3.141593f)
			IG_DATA::EARTH_THETA += 4.f * 3.141593f;

		if (IG_DATA::PHI > 3.140f / 2.f)
			IG_DATA::PHI = 3.140f / 2.f;
		if (IG_DATA::PHI < -3.140f / 2.f)
			IG_DATA::PHI = -3.140f / 2.f;
	}
	ImGui::End();

	if (light > -1)
		doLightEditor(light);

	//	....

	drawFrame();
}