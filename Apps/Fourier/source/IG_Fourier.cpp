#include "IG_Fourier.h"
#include "Fourier.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include <string>

bool			IG_Fourier::loadMenuOpen = false;
bool			IG_Fourier::saveMenuOpen = false;
bool			IG_Fourier::addName = false;
int				IG_Fourier::iMenu = -1;
bool			IG_Fourier::colorPickerOpen = false;
char**			IG_Fourier::interpolationsNames = NULL;
char**			IG_Fourier::figureNames = NULL;
char**			IG_Fourier::plotsNames = NULL;
int*			IG_Fourier::figureSizes = NULL;
int				IG_Fourier::light = -1;

static _float4color	harmonicsTexture = { -1.f,0.f,0.f,0.f };
static IG::lightsource savestate;

//	Private

void IG_Fourier::saveLightState(int id)
{
	savestate = IG::LIGHTS[id];
}

void IG_Fourier::loadLightState(int id)
{
	IG::LIGHTS[id] = savestate;
}

void IG_Fourier::doLightEditor(int& id)
{
	if (ImGui::Begin(" Light editor", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
	{
		ImGui::SetWindowSize(ImVec2(340, 430));

		ImGui::Text("Color:");
		if (ImGui::ColorPicker4("", (float*)&IG::LIGHTS[id].color))
			IG::UPDATE_LIGHT = id;
		ImGui::Spacing();
		ImGui::Text("Intensities:");
		if (ImGui::SliderFloat2("", (float*)&IG::LIGHTS[id].intensities, 0.f, 100000.f, "%.3f", ImGuiSliderFlags_Logarithmic))
			IG::UPDATE_LIGHT = id;
		ImGui::Spacing();
		ImGui::Text("Position:");
		if (ImGui::SliderFloat3(" ", (float*)&IG::LIGHTS[id].position, -1000.f, 1000.f, "%.3f", ImGuiSliderFlags_Logarithmic))
			IG::UPDATE_LIGHT = id;

		ImGui::SetCursorPos(ImVec2(245, 235));
		if (ImGui::Button("Cancel", ImVec2(80, 45))) {
			loadLightState(id);
			IG::UPDATE_LIGHT = id;
			id = -1;
		}
		ImGui::SetCursorPos(ImVec2(245, 290));
		if (ImGui::Button("Delete", ImVec2(80, 45))) {
			IG::LIGHTS[id].is_on = false;
			IG::LIGHTS[id].position = { 0.f,0.f,0.f,0.f };
			IG::LIGHTS[id].intensities = { 0.f,0.f,0.f,0.f };
			IG::LIGHTS[id].color = { 0.f,0.f,0.f,0.f };
			IG::UPDATE_LIGHT = id;
			id = -1;
		}
		ImGui::SetCursorPos(ImVec2(245, 345));
		if (ImGui::Button("Apply", ImVec2(80, 45)))
			id = -1;

	}
	ImGui::End();
}

void IG_Fourier::loadMenu()
{
	if (ImGui::Begin("Load Menu", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar))
	{
		ImGui::SetWindowPos(ImVec2(IG::WindowDim.x / 2.f - 100.f, IG::WindowDim.y / 2.f - 52.f));

		if(IG::FIGURE_FILE)
			ImGui::SetWindowSize(ImVec2(200, 127));

		else
			ImGui::SetWindowSize(ImVec2(200, 81));


		static bool change = false;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Load Menu", false))
				ImGui::EndMenu();

			const char* label;
			if (IG::FIGURE_FILE)
				label = "Figure";
			else
				label = "Coefficients";

			if (ImGui::BeginMenu(label))
			{
				if (ImGui::MenuItem("Figure"))
				{
					IG::FIGURE_FILE = true;
					change = true;
				}
				if (ImGui::MenuItem("Coefficients"))
				{
					IG::FIGURE_FILE = false;
					change = true;
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		static const char* existance;
		static bool exists = false;
		
		if (ImGui::InputText(" File", IG::FILENAME, 100, ImGuiInputTextFlags_AutoSelectAll) || change)
		{
			change = false;
			FILE* file;
			if (IG::FIGURE_FILE)
				file = fopen((FIGURES_DIR + std::string(IG::FILENAME) + ".dat").c_str(), "r");
			else
				file = fopen((COEFFICIENTS_DIR + std::string(IG::FILENAME) + ".dat").c_str(), "r");

			exists = file;
			if (file)
				fclose(file);
		}

		if (IG::FIGURE_FILE)
		{
			static int L = 20;
			ImGui::InputInt(" Max L", &L);
			if (L < 0)
				L = 0;
			if (L > MAX_L)
				L = MAX_L;
			IG::MAXL = (unsigned int)L;

			static int T = 0;
			ImGui::InputInt(" Depth", &T);
			if (T < 0)
				T = 0;
			if (T > MAX_T_DIV)
				T = MAX_T_DIV;
			IG::TDEPTH = T;
		}
		ImGui::Spacing();
		if (!exists)
			ImGui::BeginDisabled();
		if (ImGui::Button("Load", ImVec2(89, 19)) || (ImGui::IsKeyPressed(ImGuiKey_Enter) && exists))
		{
			if (IG::FIGURE_FILE)
			{
				for (unsigned int i = 0; i < IG::NFIG; i++)
				{
					if (std::string(figureNames[i]) == IG::FILENAME && figureSizes[i]!= -1)
					{
						if (figureSizes[i] == IG::MAXL + 100 * IG::TDEPTH)
						{
							IG::VIEW1 = i;
							for(unsigned int j = 0; j<IG::PAIRS_SIZE;j++)
							{
								if (IG::PAIRS[j].x == i)
								{
									IG::VIEW2 = IG::PAIRS[j].y;
									IG::DOUBLE_VIEW = true;
								}
							}
							IG::UPDATE_CURVES = true;
							IG::UPDATE_LIGHT = -2;
							loadMenuOpen = false;
							ImGui::End();
							return;
						}

						for (unsigned int j = 0; j < IG::PAIRS_SIZE; j++)
						{
							if (IG::PAIRS[j].x == i)
							{
								IG::COPY = IG::PAIRS[j].y;
								IG::ALREADY_EXISTS = true;
							}
						}
					}
				}

				add1to(IG::PAIRS, IG::PAIRS_SIZE);
				IG::PAIRS[IG::PAIRS_SIZE] = { int(IG::NFIG), IG::ALREADY_EXISTS ? IG::COPY : -int(IG::NPLOT) - 2 };
				IG::PAIRS_SIZE++;

				add1to(figureNames, IG::NFIG);
				figureNames[IG::NFIG] = (char*)calloc(100, sizeof(char));
				for (unsigned int i = 0; i < 100; i++)
				{
					char c = IG::FILENAME[i];
					if (c <= 90 && c >= 65)
						c += 32;
					figureNames[IG::NFIG][i] = c;
					if (!c)
						break;
				}

				if (!IG::ALREADY_EXISTS)
				{
					add1to(plotsNames, IG::NPLOT);
					plotsNames[IG::NPLOT] = (char*)calloc(100, sizeof(char));
					for (unsigned int i = 0; i < 100; i++)
					{
						char c = IG::FILENAME[i];
						if (c <= 90 && c >= 65)
							c += 32;
						plotsNames[IG::NPLOT][i] = c;
						if (!c)
							break;
					}
				}

				add1to(figureSizes, IG::NFIG);
				figureSizes[IG::NFIG] = IG::MAXL + 100 * IG::TDEPTH;

				IG::CALCULATE_FIGURE = true;
				loadMenuOpen = false;
			}
			
			else
			{
				for (unsigned int i = 0; i < IG::NFIG; i++)
				{
					if (std::string(figureNames[i]) == IG::FILENAME && figureSizes[i] == -1)
					{
						IG::VIEW1 = i;
						IG::DOUBLE_VIEW = false;
						IG::UPDATE_CURVES = true;
						IG::UPDATE_LIGHT = -2;
						loadMenuOpen = false;
						ImGui::End();
						return;
					}
				}

				add1to(figureNames, IG::NFIG);
				figureNames[IG::NFIG] = (char*)calloc(100, sizeof(char));
				for (unsigned int i = 0; i < 100; i++)
				{
					char c = IG::FILENAME[i];
					if (c <= 90 && c >= 65)
						c += 32;
					figureNames[IG::NFIG][i] = c;
					if (!c)
						break;
				}

				add1to(figureSizes, IG::NFIG);
				figureSizes[IG::NFIG] = -1;

				IG::CALCULATE_FIGURE = true;
				loadMenuOpen = false;

			}

		}
		if (!exists)
			ImGui::EndDisabled();

		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 95.f, ImGui::GetCursorPosY() - 23.f));
		if (ImGui::Button("Cancel", ImVec2(89, 19)))
			loadMenuOpen = false;

	}
	ImGui::End();

}

void IG_Fourier::saveMenu()
{
	if(ImGui::Begin("Save Menu", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
	{
		ImGui::SetWindowPos(ImVec2(IG::WindowDim.x / 2.f - 100.f, IG::WindowDim.y / 2.f - 52.f));
		ImGui::SetWindowSize(ImVec2(200, 82));

		if (addName)
		{
			if (IG::SAVE_NAME)
				free(IG::SAVE_NAME);

			IG::SAVE_NAME = (char*)calloc(110, sizeof(char));

			if (IG::VIEW1 >= int(IG::NFIG))
			{
				int i = -1;
				while (interpolationsNames[IG::VIEW1 - IG::NFIG][++i])
					IG::SAVE_NAME[i] = interpolationsNames[IG::VIEW1 - IG::NFIG][i];
			}
			else
			{
				int i = -1;
				while (figureNames[IG::VIEW1][++i])
					IG::SAVE_NAME[i] = figureNames[IG::VIEW1][i];
				IG::SAVE_NAME[i] = '_';
				IG::SAVE_NAME[i + 1] = std::to_string(figureSizes[IG::VIEW1] / 100).c_str()[0];
				IG::SAVE_NAME[i + 2] = '_';
				IG::SAVE_NAME[i + 3] = std::to_string(figureSizes[IG::VIEW1] % 100).c_str()[0];
				IG::SAVE_NAME[i + 4] = std::to_string(figureSizes[IG::VIEW1] % 100).c_str()[1];
			}
			addName = false;
		}

		ImGui::InputText(" Name", IG::SAVE_NAME, 100, ImGuiInputTextFlags_AutoSelectAll);

		ImGui::Spacing();
		if (!IG::SAVE_NAME[0])
			ImGui::BeginDisabled();
		if (ImGui::Button("Save", ImVec2(89, 19)) || (ImGui::IsKeyPressed(ImGuiKey_Enter) && IG::SAVE_NAME[0]))
		{
			IG::SAVE = true;
			saveMenuOpen = false;
		}
		if (!IG::SAVE_NAME[0])
			ImGui::EndDisabled();

		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 95.f, ImGui::GetCursorPosY() - 23.f));
		if (ImGui::Button("Cancel", ImVec2(89, 19)))
			saveMenuOpen = false;
	}
	ImGui::End();
}

void IG_Fourier::colorPicker()
{
	static _float4color color = Color::Red.getColor4();

	if (ImGui::Begin(" Light editor", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
	{
		ImGui::SetWindowSize(ImVec2(350, 380));

		ImGui::Text("Color:");
		if (ImGui::ColorPicker4("", (float*)&color))

		ImGui::SetCursorPos(ImVec2(8, 321));
		if (ImGui::Button("Cancel", ImVec2(110, 45))) 
		{
			color = IG::TEXTURE;
			colorPickerOpen = false;
		}
		ImGui::SetCursorPos(ImVec2(125, 321));

		if (ImGui::Button("Apply", ImVec2(110, 45)))
		{
			colorPickerOpen = false;
			IG::UPDATE_TEXTURE = true;
			IG::TEXTURE = color;
			if (IG::VIEW1 == -1)
				harmonicsTexture = IG::TEXTURE;
		}
	}
	ImGui::End();
}

void IG_Fourier::errorsWindow()
{
	if (ImGui::Begin("L2-error computations", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar))
	{
		ImGui::SetWindowSize(ImVec2(250, float(IG::PAIRS_SIZE * 17 + 80)));
		ImGui::SetWindowCollapsed(false, ImGuiCond_Once);

		if (ImGui::BeginMenuBar())
		{
			ImGui::BeginDisabled();

			ImGui::BeginMenu("Name ");
			ImGui::BeginMenu("Depth");
			ImGui::BeginMenu("Max L");
			ImGui::BeginMenu("     Error");

			ImGui::EndDisabled();
			ImGui::EndMenuBar();
		}

		bool finished = true;
		for (unsigned int i = 0; i < IG::PAIRS_SIZE; i++)
		{
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 5.f, ImGui::GetCursorPosY()));
			ImGui::Text(figureNames[IG::PAIRS[i].x]);
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 80.f, ImGui::GetCursorPosY() - 17.f));
			ImGui::Text(std::to_string(figureSizes[IG::PAIRS[i].x] / 100).c_str());
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 125.f, ImGui::GetCursorPosY() - 17.f));
			ImGui::Text(std::to_string(figureSizes[IG::PAIRS[i].x] % 100).c_str());

			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 158.f, ImGui::GetCursorPosY() - 17.f));
			if (IG::COMPUTED_ERRORS[i])
				ImGui::Text("%.8f", IG::COMPUTED_ERRORS[i]);
			else
			{
				ImGui::Text("Computing..");
				finished = false;
			}
		}

		ImGui::Spacing();
		if (ImGui::Button(finished ? "Done" : "Cancel", ImVec2(234, 19)))
			IG::ERROR_WINDOW = false;
	}
	ImGui::End();
}

void IG_Fourier::interpolationEditor()
{
	int m = iMenu;
	unsigned int s = IG::I_DATA_SIZE[m];
	if (ImGui::Begin("Interpolation Editor", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar))
	{
		if(ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Add figure"))
			{
				for (unsigned int i = 0; i < IG::NFIG; i++)
				{
					if (ImGui::MenuItem(figureNames[i], figureSizes[i] != -1 ? (std::to_string(figureSizes[i] / 100) + " " + std::to_string(figureSizes[i] % 100)).c_str() : ""))
					{
						IG::ADD_FIGURE = i;
						add1to(IG::I_DATA[m], s);
						IG::I_DATA[m][s] = i;
						IG::I_DATA_SIZE[m]++;
					}
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::SetWindowSize(ImVec2(200, 190), ImGuiCond_Once);

		if(ImGui::BeginListBox("##",ImVec2(180, 90)))
		{
			for (unsigned int i = 0; i < s; i++)
			{
				if (ImGui::Selectable(IG::I_DATA[m][i] != -1 ? figureNames[IG::I_DATA[m][i]] : "**************"))
				{
					IG::DELETE_FIGURE = i;
					for (unsigned int j = i; j < s - 1; j++)
						IG::I_DATA[m][j] = IG::I_DATA[m][j + 1];
					IG::I_DATA_SIZE[m]--;
				}
			}
			ImGui::EndListBox();
		}

		ImGui::InputText(" Name", interpolationsNames[m], 100);

		if (!interpolationsNames[m][0])
			ImGui::BeginDisabled();
		if (ImGui::Button("Save", ImVec2(180, 19)) || (interpolationsNames[m][0] && ImGui::IsKeyPressed(ImGuiKey_Enter)))
			iMenu = -1;
		else if (!interpolationsNames[m][0])
			ImGui::EndDisabled();

	}
	ImGui::End();
}

void IG_Fourier::mainMenu()
{
	if (ImGui::Begin("Menu", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove))
	{
		if (ImGui::BeginMenuBar())
		{
			if (iMenu != -1)
				ImGui::BeginDisabled();
			if (ImGui::BeginMenu("Figure"))
			{
				if (ImGui::MenuItem("Spherical Harmonics"))
				{
					IG::VIEW1 = -1;
					IG::UPDATE_CURVES = true;
					IG::DOUBLE_VIEW = false;
				}

				if (IG::NFIG)
				{
					ImGui::MenuItem("Fourier Series", "", false, false);

					for (unsigned int i = 0; i < IG::NFIG; i++)
					{
						if (ImGui::MenuItem(figureNames[i], figureSizes[i] == -1 ? "" : (std::to_string(figureSizes[i] / 100) + " " + std::to_string(figureSizes[i] % 100)).c_str()))
						{ 
							IG::VIEW1 = i;
							IG::UPDATE_CURVES = true;
							IG::DOUBLE_VIEW = false;
						}
					}
				}

				if (IG::NPLOT)
				{
					ImGui::MenuItem("Data Plots", "", false, false);

					for (unsigned int i = 0; i < IG::NPLOT; i++)
					{
						if (ImGui::MenuItem(plotsNames[i]))
						{
							IG::VIEW1 = -int(i) - 2;
							IG::DOUBLE_VIEW = false;
						}

					}
				}

				if (IG::NINT)
				{
					ImGui::MenuItem("Interpolations", "", false, false);

					for (unsigned int i = 0; i < IG::NINT; i++)
					{
						if (ImGui::MenuItem(interpolationsNames[i]))
						{
							IG::VIEW1 = IG::NFIG + i;
							IG::DOUBLE_VIEW = false;
						}
					}
				}

				ImGui::EndMenu();
			}
			else if (iMenu != -1)
				ImGui::EndDisabled();

			if (ImGui::BeginMenu("Advanced"))
			{
				static bool datasetStarted = false;

				if (datasetStarted)
				{
					unsigned int d = FourierSurface::depthDataset();
					if (d < MAX_L)
						ImGui::MenuItem("Generating", (std::to_string(d) + "/" + std::to_string(MAX_L)).c_str(), false, false);
					else
						ImGui::MenuItem("Generated", (std::to_string(d) + "/" + std::to_string(MAX_L)).c_str(), false, false);
				}

				ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);
				if (!datasetStarted && ImGui::MenuItem("Generate Dataset"))
				{
					datasetStarted = true;
					FourierSurface::generateDataSet();
				}
				ImGui::PopItemFlag();

				if (IG::LOADING || !IG::PAIRS_SIZE)
					ImGui::BeginDisabled();
				if (ImGui::MenuItem("Compute L2-errors"))
				{
					IG::COMPUTE_ERROR = true;
					IG::ERROR_WINDOW = true;
				}
				if (IG::LOADING || !IG::PAIRS_SIZE)
					ImGui::EndDisabled();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Lights"))
			{
				for (int i = 0; i < 8; i++) {
					if (IG::LIGHTS[i].is_on && ImGui::MenuItem(std::string("Light " + std::to_string(i)).c_str()))
					{
						light = i;
						saveLightState(i);
					}
				}
				ImGui::Spacing();

				if (ImGui::MenuItem("New"))
				{
					for (int i = 0; i < 8; i++) {
						if (!IG::LIGHTS[i].is_on) {
							saveLightState(i);
							IG::LIGHTS[i].is_on = true;
							light = i;
							break;
						}
					}
				}

				if (ImGui::MenuItem("Reset"))
				{
					free(IG::LIGHTS);
					IG::LIGHTS = (IG::lightsource*)calloc(8, sizeof(IG::lightsource));
					IG::LIGHTS[0].is_on = true;
					IG::LIGHTS[0].color = { 1.f, 1.f, 1.f, 1.f };
					IG::LIGHTS[0].intensities = { 600.f, 320.f, 0.f, 0.f };
					IG::LIGHTS[0].position = { 30.f, 10.f, 20.f, 0.f };
					IG::UPDATE_LIGHT = -2;
					light = -1;
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Textures"))
			{
				if (ImGui::MenuItem("Default"))
				{
					IG::UPDATE_TEXTURE = true;
					IG::TEXTURE = { -1.f,0.f,0.f,0.f };
				}
				if (ImGui::MenuItem("Pick Color"))
					colorPickerOpen = true;
				if (ImGui::MenuItem("Randomized"))
				{
					IG::UPDATE_TEXTURE = true;
					IG::TEXTURE = { 0.f,-1.f,0.f,0.f };
				}
				if (IG::VIEW1 == -1)
					harmonicsTexture = IG::TEXTURE;

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::SetWindowPos(ImVec2(2, 2), ImGuiCond_Once);
		ImGui::SetWindowCollapsed(true, ImGuiCond_Once);
		ImGui::SetWindowSize(ImVec2(330, 120), ImGuiCond_Once);

		if (IG::VIEW1 == -1)
		{
			if (ImGui::SliderInt(" L value", &IG::L, 0, MAX_L))
			{
				IG::UPDATE = true;
				IG::UPDATE_TEXTURE = true;
				IG::TEXTURE = harmonicsTexture;
			}

			if (ImGui::SliderInt(" M value", &IG::M, -IG::L, IG::L))
			{
				IG::UPDATE = true;
				IG::UPDATE_TEXTURE = true;
				IG::TEXTURE = harmonicsTexture;
			}

			if (IG::M > IG::L) IG::M = IG::L;
			if (-IG::M > IG::L) IG::M = -IG::L;
		}

		else
		{
			if (IG::VIEW1 >= int(IG::NFIG))
			{
				if (IG::I_DATA_SIZE[IG::VIEW1 - IG::NFIG] < 2)
					ImGui::BeginDisabled();
				if (ImGui::Button(IG::PLAY ? "S" : "P", ImVec2(28, 19)))
				{
					if (IG::PLAY)
						IG::PLAY = false;
					else
						IG::PLAY = true;
				}

				ImGui::SetCursorPos(ImVec2(40, 46));
				if (ImGui::Button("<", ImVec2(15, 19)))
					IG::PLAY_SPEED /= 1.1f;

				ImGui::SetCursorPos(ImVec2(56, 46));
				if (ImGui::Button(">", ImVec2(15, 19)))
					IG::PLAY_SPEED *= 1.1f;

				ImGui::SetCursorPos(ImVec2(75, 46));
				if (ImGui::SliderFloat("T", &IG::TVALUE, 0.f, float(IG::I_DATA_SIZE[IG::VIEW1 - IG::NFIG] - 1.f)) && IG::CURVES)
					IG::UPDATE_CURVES = true;
				if (IG::I_DATA_SIZE[IG::VIEW1 - IG::NFIG] < 2)
					ImGui::EndDisabled();

			}
			else
			{
				int m = iMenu;
				if (IG::NFIG < 2 || m != -1 || IG::VIEW1 < 0)
					ImGui::BeginDisabled();
				if (ImGui::Button("Create Interpolation", ImVec2(295, 19)))
				{
					IG::ADD_FIGURE = IG::VIEW1;

					add1to(IG::I_DATA, IG::NINT);
					IG::I_DATA[IG::NINT] = (int*)calloc(1, sizeof(int));
					IG::I_DATA[IG::NINT][0] = IG::VIEW1;

					add1to(IG::I_DATA_SIZE, IG::NINT);
					IG::I_DATA_SIZE[IG::NINT] = 1u;

					add1to(interpolationsNames, IG::NINT);
					interpolationsNames[IG::NINT] = (char*)calloc(100, sizeof(char));

					IG::I_ADD = true;
					iMenu = IG::NINT;
				}
				else if (IG::NFIG < 2 || m != -1 || IG::VIEW1 < 0)
					ImGui::EndDisabled();
			}

			if (IG::VIEW1 < 0 || iMenu != -1)
				ImGui::BeginDisabled();
			if (ImGui::Button("Save Figure", ImVec2(95, 19)))
			{
				IG::DOUBLE_VIEW = false;
				IG::PLAY = false;
				iMenu = -1;
				saveMenuOpen = true;
				addName = true;
			}
			if (IG::VIEW1 < 0 || iMenu != -1)
				ImGui::EndDisabled();

			ImGui::SetCursorPos(ImVec2(108, 69));
			if (IG::VIEW1 >= int(IG::NFIG))
			{
				if (IG::LOADING)
					ImGui::BeginDisabled();
				if (ImGui::Button("Editor", ImVec2(95, 19)))
					iMenu = IG::VIEW1 - IG::NFIG;
				if (IG::LOADING)
					ImGui::EndDisabled();
			}
			else if (IG::DOUBLE_VIEW)
			{
				if (ImGui::Button("Single View", ImVec2(95, 19)))
					IG::DOUBLE_VIEW = false;
			}
			else
			{
				bool haspair = false;
				for (unsigned int i = 0; i < IG::PAIRS_SIZE; i++)
				{
					if (IG::VIEW1 == IG::PAIRS[i].x || IG::VIEW1 == IG::PAIRS[i].y)
					{
						if (ImGui::Button("Double View", ImVec2(95, 19)))
						{
							IG::DOUBLE_VIEW = true;
							IG::VIEW2 = IG::VIEW1 == IG::PAIRS[i].x ? IG::PAIRS[i].y : IG::PAIRS[i].x;
						}
						haspair = true;
					}
				}
				if (!haspair)
				{
					ImGui::BeginDisabled();
					ImGui::Button("Double View", ImVec2(95, 19));
					ImGui::EndDisabled();
				}
			}

			ImGui::SetCursorPos(ImVec2(208, 69));
			if (ImGui::Button("Delete", ImVec2(95, 19)))
			{
				IG::DELETE_VIEW = true;

				if (IG::VIEW1 >= int(IG::NFIG))
				{
					free(interpolationsNames[IG::VIEW1 - IG::NFIG]);
					free(IG::I_DATA[IG::VIEW1 - IG::NFIG]);

					for (unsigned int i = IG::VIEW1 - IG::NFIG; i < IG::NINT - 1; i++)
					{
						IG::I_DATA[i] = IG::I_DATA[i + 1];
						IG::I_DATA_SIZE[i] = IG::I_DATA_SIZE[i + 1];
						interpolationsNames[i] = interpolationsNames[i + 1];
					}
					iMenu = -1;

					if (IG::NINT == 1)
					{
						free(IG::I_DATA);
						free(IG::I_DATA_SIZE);
						free(interpolationsNames);
						interpolationsNames = NULL;
						IG::I_DATA = NULL;
						IG::I_DATA_SIZE = NULL;

					}
				}
				else if (IG::VIEW1 > -1)
				{
					free(figureNames[IG::VIEW1]);
					for (unsigned int i = IG::VIEW1; i < IG::NFIG - 1; i++)
						figureNames[i] = figureNames[i + 1];

					for (unsigned int i = 0; i < IG::PAIRS_SIZE; i++)
					{
						if (IG::PAIRS[i].x == IG::VIEW1)
						{
							for (unsigned int j = i; j < IG::PAIRS_SIZE - 1; j++)
								IG::PAIRS[j] = IG::PAIRS[j + 1];
							IG::PAIRS_SIZE--;
							i--;
						}
					}
					for (unsigned int i = 0; i < IG::PAIRS_SIZE; i++)
					{
						if (IG::PAIRS[i].x > IG::VIEW1)
							IG::PAIRS[i].x--;
					}
					if (!IG::PAIRS_SIZE)
					{
						free(IG::PAIRS);
						IG::PAIRS = NULL;
					}
					for (unsigned int i = 0; i < IG::NINT; i++)
					{
						for (unsigned int j = 0; j < IG::I_DATA_SIZE[i]; j++)
							IG::I_DATA[i][j] = -1;
					}
					if (IG::NFIG == 1u)
					{
						free(figureNames);
						free(figureSizes);
						figureSizes = NULL;
						figureNames = NULL;
					}
				}
				else
				{
					unsigned int p = -IG::VIEW1 - 2;
					free(plotsNames[p]);
					for (unsigned int i = p; i < IG::NPLOT - 1; i++)
						plotsNames[i] = plotsNames[i + 1];

					for (unsigned int i = 0; i < IG::PAIRS_SIZE; i++)
					{
						if (IG::PAIRS[i].y == IG::VIEW1)
						{
							for (unsigned int j = i; j < IG::PAIRS_SIZE - 1; j++)
								IG::PAIRS[j] = IG::PAIRS[j + 1];
							IG::PAIRS_SIZE--;
							i--;
						}
					}
					for (unsigned int i = 0; i < IG::PAIRS_SIZE; i++)
					{
						if (IG::PAIRS[i].y < IG::VIEW1)
							IG::PAIRS[i].y++;
					}
					if (!IG::PAIRS_SIZE)
					{
						free(IG::PAIRS);
						IG::PAIRS = NULL;
					}
					if (IG::NPLOT == 1u)
					{
						free(plotsNames);
						plotsNames = NULL;
					}
				}
			}
		}

		if (iMenu != -1)
			ImGui::BeginDisabled();
		if (!IG::LOADING && ImGui::Button("Load Figure", ImVec2(95, 19)))
			loadMenuOpen = true;
		if (iMenu != -1)
			ImGui::EndDisabled();

		static int counter = 0;
		static std::string loadText;
		if (IG::LOADING)
		{
			counter++;
			counter %= 120;
			if (counter < 30)
				loadText = "Loading.";
			else if (counter < 60)
				loadText = "Loading..";
			else if (counter < 90)
				loadText = "Loading...";
			else
				loadText = "Loading....";
			ImGui::BeginDisabled();
			ImGui::Button(loadText.c_str(), ImVec2(95, 19));
			ImGui::EndDisabled();
		}

		ImGui::SetCursorPos(ImVec2(108, 92));
		if (!IG::DOUBLE_VIEW && IG::VIEW1 < -1)
		{
			ImGui::BeginDisabled();
			IG::CURVES = false;
			ImGui::SetWindowSize(ImVec2(330, 120));
		}
		if (IG::CURVES)
		{
			if (ImGui::Button("Hide Curves", ImVec2(95, 19)))
			{
				IG::CURVES = false;
				ImGui::SetWindowSize(ImVec2(330, 120));
			}

			if (ImGui::SliderFloat("Phi", &IG::phi, 0.f, 2 * pi)) IG::UPDATE_CURVES = true;
			if (ImGui::SliderFloat("Theta", &IG::theta, 0.f, pi)) IG::UPDATE_CURVES = true;
		}
		else if (ImGui::Button("Show Curves", ImVec2(95, 19)))
		{
			ImGui::SetWindowSize(ImVec2(330, 168));
			IG::CURVES = true;
			IG::UPDATE_CURVES = true;
		}
		if (!IG::DOUBLE_VIEW && IG::VIEW1 < -1)
			ImGui::EndDisabled();

		ImGui::SetCursorPos(ImVec2(208, 92));
		if (ImGui::Button("Coefficients", ImVec2(95, 19)))
		{
			if (IG::COEF_VIEW)
				IG::COEF_VIEW = false;
			else
				IG::COEF_VIEW = true;
		}
	}
	ImGui::End();
}

//	Public

void IG_Fourier::render()
{

	if (!IG::MENU)
		return;

	newFrame();

	if (saveMenuOpen)
	{
		saveMenu();
		drawFrame();
		return;
	}

	if (IG::ERROR_WINDOW)
	{
		errorsWindow();
		drawFrame();
		return;
	}

	mainMenu();

	if (iMenu > -1)
		interpolationEditor();

	if (loadMenuOpen)
		loadMenu();

	if (light > -1)
		doLightEditor(light);

	if (colorPickerOpen)
		colorPicker();

	drawFrame();
}
