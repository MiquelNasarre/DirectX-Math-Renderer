#pragma once
#include "Window.h"

/* IMGUI BASE CLASS MANAGER
-------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------
Given the difficulty of developing User Interface classes this library encourages 
the use of ImGui for the user interface of the developed applications.

To use ImGui inside your applications you just need to create an inherited iGManager
class and a constructor that feeds the widow to the base constructor.

Then you can create a render function that starts with newFrame and end with drawFrame
using the imGui provided functions and your ImGui interface will be drawn to the window.

For more information on how to develop ImGui interfaces please visit:
ImGui GitHub Repositiory: https://github.com/ocornut/imgui.git
-------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------
*/

// ImGui Interface base class, contains the basic building blocks to connect the ImGui
// API to this library, any app that wishes to use ImGui needs to inherit this class and 
// call the constructor on the desired window.
class iGManager 
{
	// Needs access to the internals to feed messages to ImGui if used.
	friend class MSGHandlePipeline;

protected:
	// Constructor, initializes the ImGui context for the specific window and 
	// initializes ImGui WIN32/DX11 in general if called for the first time.
	// Important to call ImGui is to be used in the application.
	iGManager(Window& _w);

	// If it is the last class instance shuts down ImGui WIN32/DX11.
	~iGManager();

	// Function to be called at the beggining of an ImGui render function.
	// Calls new frame on Win32 and DX11 on the imGui API.
	static void newFrame();

	// Function to be called at the end of an ImGui render function.
	// Calls the rendering method of DX11 on the imGui API.
	static void drawFrame();

private:
	// Keeps track of how many instances of the class are currently active.
	static unsigned int contextCount;

	// Stores the pointer to the ImGui context of the specific window of the instance.
	void* Context = nullptr;

	// Called by the MSGHandlePipeline lets ImGui handle the message flow
	// if imGui is active and currently has focus on the window.
	static bool WndProcHandler(void* hWnd, unsigned int msg, unsigned int wParam, unsigned int lParam);
};