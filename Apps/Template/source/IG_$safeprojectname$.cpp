#include "IG_$safeprojectname$.h"
#include "imgui/imgui.h"
#include "$safeprojectname$.h"

void IG_$safeprojectname$::render()
{
	newFrame();

	if (ImGui::Begin("Camera View"))
	{
		ImGui::SetWindowPos(ImVec2(2, 2), ImGuiCond_Once);
		ImGui::SetWindowCollapsed(true, ImGuiCond_Once);
		ImGui::SetWindowSize(ImVec2(315, 80), ImGuiCond_Once);

		ImGui::SliderAngle("Theta", &IG::THETA);
		ImGui::SliderAngle("Phi", &IG::PHI, -90, 90);
	}
	ImGui::End();

	drawFrame();
}
