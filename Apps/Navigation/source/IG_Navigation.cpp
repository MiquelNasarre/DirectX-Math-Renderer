#include "IG_Navigation.h"
#include "Navigation.h"
#include "imgui/imgui.h"

int IG_Navigation::waypointSelected = -1;
int IG_Navigation::routeSelected = -1;

void IG_Navigation::renderWaypointSettings(int& w)
{
	if (ImGui::Begin("Waypoint settings"))
	{
		ImGui::Text("Name");
		ImGui::InputText("##", IG_DATA::POINTS[w].name, 20);


		ImGui::Spacing();
		ImGui::Text("Coordinates");

		float lat = float(IG_DATA::POINTS[w].pos.latitude.angleDeg);
		if (ImGui::SliderFloat("latitude", &lat, -90.f, 90.f, "%.1f"))
			IG_DATA::POINTS[w].pos.latitude.angleDeg = double(lat);

		float Long = float(IG_DATA::POINTS[w].pos.Longitude.angleDeg);
		if (ImGui::SliderFloat("Longitude", &Long, -180.f, 180.f, "%.1f"))
			IG_DATA::POINTS[w].pos.Longitude.angleDeg = double(Long);

		ImGui::Spacing();
		ImGui::Text("Radius");

		float rad = IG_DATA::POINTS[w].rad;
		if (ImGui::SliderFloat(" ", &rad, 0.f, 20.f, "%.1f")) {
			IG_DATA::POINTS[w].rad = rad;
			IG_DATA::POINTS[w].point->updateRadius(IG_DATA::window->graphics, rad);
		}


		ImGui::Spacing();
		ImGui::Text("Color");

		if (ImGui::ColorPicker4("##", IG_DATA::POINTS[w].color))
			IG_DATA::POINTS[w].point->updateColor(IG_DATA::window->graphics, Color(IG_DATA::POINTS[w].color));

		if (ImGui::Button("Delete"))
		{
			IG_DATA::POINTS.erase(IG_DATA::POINTS.begin() + w);
			w = -1;
		}

		if (ImGui::Button("Cancel"))
			w = -1;

		if (ImGui::Button("Apply"))
			w = -1;

	}
	ImGui::End();
}

void IG_Navigation::renderRouteSettings(int& r)
{
}

void IG_Navigation::render()
{
	newFrame();

	//	imgui stuff

	if (ImGui::Begin("ToolBox", NULL, ImGuiWindowFlags_MenuBar))
	{
		ImGui::SetWindowPos(ImVec2(2, 2), ImGuiCond_Once);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("WayPoints"))
			{
				for (UINT i = 0; i<IG_DATA::POINTS.size();i++)
				{
					if (ImGui::MenuItem(IG_DATA::POINTS[i].name))
						waypointSelected = i;
				}
				if (ImGui::MenuItem("New"))
				{
					waypointSelected = (int)IG_DATA::POINTS.size();
					IG_DATA::POINTS.push_back(Navigation::Waypoint(IG_DATA::window->graphics, Coordinate(IG_DATA::PHI, IG_DATA::THETA + pi / 2, RADIANS)));
				}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Routes"))
			{

				ImGui::EndMenu();
			}
			
			ImGui::EndMenuBar();
		}

		ImGui::Spacing();
		ImGui::Text("Earth & Moon movement:");

		ImGui::SliderAngle("Theta", &IG_DATA::THETA);
		ImGui::SliderAngle("Phi", &IG_DATA::PHI, -90.f, 90.f);

		if (IG_DATA::PHI > 3.140f / 2.f)
			IG_DATA::PHI = 3.140f / 2.f;
		if (IG_DATA::PHI < -3.140f / 2.f)
			IG_DATA::PHI = -3.140f / 2.f;
	}
	ImGui::End();

	if (waypointSelected != -1)
		renderWaypointSettings(waypointSelected);

	if (routeSelected =! -1)
		renderRouteSettings(routeSelected);

	//	...

	drawFrame();
}
