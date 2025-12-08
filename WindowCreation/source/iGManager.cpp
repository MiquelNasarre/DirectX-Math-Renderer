#include "iGManager.h"

#include "WinHeader.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

// Declares the use of the ImGui message procedure.
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Keeps track of how many instances of the class are currently active.
unsigned int iGManager::contextCount = 0u;

/*
--------------------------------------------------------------------------------------------
 iGManager Function Definitions
--------------------------------------------------------------------------------------------
*/

// Constructor, initializes the ImGui context for the specific window and 
// initializes ImGui WIN32/DX11 in general if called for the first time.
// Important to call ImGui is to be used in the application.

iGManager::iGManager(Window& _w)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	Context = ImGui::GetCurrentContext();
	contextCount++;

	if (contextCount == 1)
	{
		ImGui_ImplWin32_Init(_w.getWindowHandle());
		ImGui_ImplDX11_Init((ID3D11Device*)GlobalDevice::get_device_ptr(), (ID3D11DeviceContext*)GlobalDevice::get_context_ptr());
	}
}

// If it is the last class instance shuts down ImGui WIN32/DX11.

iGManager::~iGManager()
{
	contextCount--;
	if (!contextCount)
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
	}

}

// Function to be called at the beggining of an ImGui render function.
// Calls new frame on Win32 and DX11 on the imGui API.

void iGManager::newFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

// Function to be called at the end of an ImGui render function.
// Calls the rendering method of DX11 on the imGui API.

void iGManager::drawFrame()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

// Called by the MSGHandlePipeline lets ImGui handle the message flow
// if imGui is active and currently has focus on the window.

bool iGManager::WndProcHandler(void* hWnd, unsigned int msg, unsigned int wParam, unsigned int lParam)
{
	if (!contextCount)
		return false;
	ImGui_ImplWin32_WndProcHandler((HWND)hWnd, msg, wParam, lParam);
	const auto& imio = ImGui::GetIO();
	if (imio.WantCaptureKeyboard || imio.WantCaptureMouse)
		return true;
	return false;
}

