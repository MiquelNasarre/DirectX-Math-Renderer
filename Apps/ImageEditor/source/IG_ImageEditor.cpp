#include "IG_ImageEditor.h"
#include "ImageEditor.h"
#include "imgui/imgui.h"

void IG_ImageEditor::render()
{
	newFrame();

	//	imgui stuff

	if (ImGui::Begin("View"))
	{
		const char* items[3] = { "Background", "Sphere", "Weird" };
		ImGui::ListBox("##", (int*)&IG_DATA::SHOW, items, 3);
	}
	ImGui::End();

	//	...

	drawFrame();
}
