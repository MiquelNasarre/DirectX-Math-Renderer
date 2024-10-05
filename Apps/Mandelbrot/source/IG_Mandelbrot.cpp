#include "IG_Mandelbrot.h"
#include "imgui/imgui.h"
#include "Mandelbrot.h"

#include <string>

IG_Mandelbrot::QY IG_Mandelbrot::QUALITY = IG_Mandelbrot::QUALITY_CURRENT;
int IG_Mandelbrot::customX = 0;
int IG_Mandelbrot::customY = 0;

void IG_Mandelbrot::render()
{
	newFrame();

	if (IG::MENU_OPEN && ImGui::Begin("Menu"))
	{
		ImGui::SetWindowPos(ImVec2(2, 2), ImGuiCond_Once);
		ImGui::SetWindowCollapsed(true, ImGuiCond_Once);
		ImGui::SetWindowSize(ImVec2(315, 80), ImGuiCond_Once);

		if (IG::SAVE_MENU_OPEN)
			ImGui::BeginDisabled();
		if (ImGui::Button("Save Image"))
			IG::SAVE_MENU_OPEN = true;

		else if (IG::SAVE_MENU_OPEN)
			ImGui::EndDisabled();


	}
	if (IG::MENU_OPEN)
		ImGui::End();

	if (IG::SAVE_MENU_OPEN)
		saveMenu();

	drawFrame();
}

void IG_Mandelbrot::saveMenu()
{
	if (ImGui::Begin("Save Menu", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
	{
		ImVec2 MenuSize(350, 180);
		ImGui::SetWindowSize(MenuSize, ImGuiCond_Once);
		ImGui::SetWindowPos(ImVec2(IG::WindowDim.x / 2 - MenuSize.x / 2, IG::WindowDim.y / 2 - MenuSize.y / 2));

		ImGui::SetNextItemWidth(265);
		ImGui::InputText("File path", (char*)IG::PATH, 200);

		ImGui::Spacing();
		ImGui::Spacing();

		std::string name;

		switch (QUALITY)
		{
		case IG_Mandelbrot::QUALITY_CURRENT:
			IG::QUALITY = IG::WindowDim;
			name = std::to_string(IG::QUALITY.x) + "x" + std::to_string(IG::QUALITY.y) + " px (Current Dimensions)";
			break;

		case IG_Mandelbrot::QUALITY_720x480:
			IG::QUALITY = { 720,480 };
			name = std::to_string(IG::QUALITY.x) + "x" + std::to_string(IG::QUALITY.y) + " px";
			break;

		case IG_Mandelbrot::QUALITY_1280x720:
			IG::QUALITY = { 1280,720 };
			name = std::to_string(IG::QUALITY.x) + "x" + std::to_string(IG::QUALITY.y) + " px";
			break;

		case IG_Mandelbrot::QUALITY_1920x1080:
			IG::QUALITY = { 1920,1080 };
			name = std::to_string(IG::QUALITY.x) + "x" + std::to_string(IG::QUALITY.y) + " px";
			break;

		case IG_Mandelbrot::QUALITY_3840x2160:
			IG::QUALITY = { 3840,2160 };
			name = std::to_string(IG::QUALITY.x) + "x" + std::to_string(IG::QUALITY.y) + " px";
			break;

		case IG_Mandelbrot::QUALITY_7680x4320:
			IG::QUALITY = { 7680,4320 };
			name = std::to_string(IG::QUALITY.x) + "x" + std::to_string(IG::QUALITY.y) + " px";
			break;

		case IG_Mandelbrot::QUALITY_CUSTOM:
			name = std::to_string(IG::QUALITY.x) + "x" + std::to_string(IG::QUALITY.y) + " px (Custom Dimensions)";
			break;

		default:
			break;
		}

		ImGui::SetNextItemWidth(265);
		if (ImGui::BeginCombo("Quality", name.c_str()))
		{

			if (ImGui::Selectable("Current Dimensions"))
				QUALITY = QUALITY_CURRENT;
			if (ImGui::Selectable("720x480px"))
				QUALITY = QUALITY_720x480;
			if (ImGui::Selectable("1280x720px"))
				QUALITY = QUALITY_1280x720;
			if (ImGui::Selectable("1920x1080px"))
				QUALITY = QUALITY_1920x1080;
			if (ImGui::Selectable("3840x2160px"))
				QUALITY = QUALITY_3840x2160;
			if (ImGui::Selectable("7680x4320px"))
				QUALITY = QUALITY_7680x4320;
			if (ImGui::Selectable("Custom Dimensions"))
				QUALITY = QUALITY_CUSTOM;


			ImGui::EndCombo();
		}
		if (QUALITY != QUALITY_CUSTOM)
			ImGui::BeginDisabled();
		ImGui::SetNextItemWidth(265);
		ImGui::InputInt2("Custom", (int*)&IG::QUALITY);
		if (QUALITY != QUALITY_CUSTOM)
			ImGui::EndDisabled();

		ImGui::Spacing();
		ImGui::Spacing();

		if (QUALITY == QUALITY_CURRENT)
		{
			name = "##";
			ImGui::BeginDisabled();
		}
		else if (IG::ADAPT)
			name = "Resize window";
		else
			name = "Strecht window";

		ImGui::SetNextItemWidth(265);
		if (ImGui::BeginCombo("Missmatch", name.c_str()))
		{
			if (ImGui::Selectable("Adapt window dimensions to fit the selected dimensions"))
				IG::ADAPT = true;
			if (ImGui::Selectable("Strecht image width to adapt to selected dimensions"))
				IG::ADAPT = false;
			ImGui::EndCombo();
		}
		if (QUALITY == QUALITY_CURRENT)
			ImGui::EndDisabled();

		ImGui::SetCursorPos(ImVec2(10, 150));
		if (ImGui::Button("Save", ImVec2(160, 21)))
		{
			IG::SAVE = true;
			IG::SAVE_MENU_OPEN = false;
		}
		ImGui::SetCursorPos(ImVec2(180, 150));
		if (ImGui::Button("Cancel", ImVec2(160, 21)))
			IG::SAVE_MENU_OPEN = false;

	}
	ImGui::End();
}
