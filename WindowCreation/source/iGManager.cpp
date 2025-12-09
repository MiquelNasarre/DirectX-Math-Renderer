#include "iGManager.h"
#include "Exception/_exDefault.h"

#include "WinHeader.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

#include <unordered_map>

// Declares the use of the ImGui message procedure.
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Map each HWND to its iGManager instance
namespace {
	std::unordered_map<HWND, iGManager*> g_imguiInstances;
}

/*
--------------------------------------------------------------------------------------------
 iGManager Function Definitions
--------------------------------------------------------------------------------------------
*/

// Constructor, initializes the ImGui context for the specific window and 
// initializes ImGui WIN32/DX11 in general if called for the first time.
// Important to call ImGui is to be used in the application.

iGManager::iGManager(Window& _w)
	: iGManager()
{
	bind(_w);
}

// Constructor, initializes ImGui DX11 for the specific instance. Does not 
// bind user interface to any window. Bind must be called for interaction.

iGManager::iGManager()
{
	IMGUI_CHECKVERSION();

	// If the global device are still not created create them.
	GlobalDevice::set_global_device();

	// Create a unique context for this window
	Context = ImGui::CreateContext();
	ImGui::SetCurrentContext((ImGuiContext*)Context);
	ImGui::StyleColorsDark();

	ImGui_ImplDX11_Init(
		(ID3D11Device*)GlobalDevice::get_device_ptr(),
		(ID3D11DeviceContext*)GlobalDevice::get_context_ptr()
	);
}

// If it is the last class instance shuts down ImGui WIN32/DX11.

iGManager::~iGManager()
{
	HWND hWnd = (HWND)pWindow->getWindowHandle();
	g_imguiInstances.erase(hWnd);

	// Destroy this ImGui context
	ImGui::SetCurrentContext((ImGuiContext*)Context);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext((ImGuiContext*)Context);
}

// Function to be called at the beggining of an ImGui render function.
// Calls new frame on Win32 and DX11 on the imGui API.

void iGManager::newFrame()
{
	ImGui::SetCurrentContext((ImGuiContext*)Context);
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

// Function to be called at the end of an ImGui render function.
// Calls the rendering method of DX11 on the imGui API.

void iGManager::drawFrame()
{
	ImGui::SetCurrentContext((ImGuiContext*)Context);
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

// Binds the objects user interaction to the specified window.

void iGManager::bind(Window& _w)
{
	ImGui::SetCurrentContext((ImGuiContext*)Context);

	// If a window is bind already delete that bind.
	if (pWindow)
	{
		g_imguiInstances.erase((HWND)pWindow->getWindowHandle());
		ImGui_ImplWin32_Shutdown();
	}

	// If another iGManager is bound to this window throw exception.
	auto it = g_imguiInstances.find((HWND)_w.getWindowHandle());
	if (it != g_imguiInstances.end())
		throw INFO_EXCEPT("You cannot have multiple ImGui context bound to the same window at the same time.");

	ImGui_ImplWin32_Init(_w.getWindowHandle());

	// Register this instance for its HWND
	g_imguiInstances[(HWND)_w.getWindowHandle()] = this;

	pWindow = &_w;
}

// Called by the MSGHandlePipeline lets ImGui handle the message flow
// if imGui is active and currently has focus on the window.

bool iGManager::WndProcHandler(void* hWnd, unsigned int msg, unsigned int wParam, unsigned int lParam)
{
	HWND hwnd = (HWND)hWnd;

	auto it = g_imguiInstances.find(hwnd);
	if (it == g_imguiInstances.end())
		return false;

	iGManager* mgr = it->second;
	if (!mgr->Context)
		return false;

	// Switch to the correct ImGui context for this window
	ImGui::SetCurrentContext((ImGuiContext*)mgr->Context);

	ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
	const auto& imio = ImGui::GetIO();
	if (imio.WantCaptureKeyboard || imio.WantCaptureMouse)
		return true;

	return false;
}

