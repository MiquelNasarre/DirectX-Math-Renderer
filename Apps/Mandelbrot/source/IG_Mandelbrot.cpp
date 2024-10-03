#include "IG_Mandelbrot.h"
#include "imgui/imgui.h"
#include "Mandelbrot.h"

void IG_Mandelbrot::render()
{
	newFrame();

	if (ImGui::Begin("Camera View"))
	{
		ImGui::SetWindowPos(ImVec2(2, 2), ImGuiCond_Once);
		ImGui::SetWindowCollapsed(true, ImGuiCond_Once);
		ImGui::SetWindowSize(ImVec2(315, 80), ImGuiCond_Once);

	}
	ImGui::End();

	drawFrame();
}
