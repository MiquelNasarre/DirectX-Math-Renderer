#pragma once
#include "Graphics.h"

/* WINDOW OBJECT CLASS
-------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------
This header contains the Window object and its functions. To facilitate window
creation and management this class handles all the Win32 API and gives access to 
a set of simple functions to control the application window.

For App creation this API comes with a set of default Drawables and a Graphics 
object contained inside the window, for an example on how to use the library you
can check the template app.

For further information check the github page:
https://github.com/MiquelNasarre/DirectX-Math-Renderer.git
-------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------
*/

// The window class contains all the necessary functions to create and use a window.
// Since the library is built for 3D apps it also contains a graphics object and 
// default drawables to choose from to create any graphics App from scratch.
class Window
{
	// Class that handles the user updates to the window.
	friend class MSGHandlePipeline;
	friend class iGManager;
public:
	// Returns a reference to the window internal Graphics object.
	Graphics& graphics();

	// Loops throgh the messages, pushes them to the queue and translates them.
	bool processEvents();

	// Closes the window.
	void close();

public:
	// --- CONSTRUCTOR / DESTRUCTOR ---

	// Creates the window and its associated Graphics object with the
	// specified dimensions, title, icon and theme.
	Window(Vector2i Dim, const char* Title, const char* IconFilename = "", bool darkTheme = true);

	// Handles the proper deletion of the window data after its closing.
	~Window();

	// --- GETTERS / SETTERS ---

	// Set the title of the window, allows for formatted strings.
	void setTitle(const char* name, ...);

	// Sets the icon of the window via its filename (Has to be a .ico file).
	void setIcon(const char* filename);

	// Sets the dimensions of the window to the ones specified.
	void setDimensions(Vector2i Dim);

	// Sets the position of the window to the one specified.
	void setPosition(Vector2i Pos);

	// Sets the maximum framerate of the widow to the one specified.
	void setFramerateLimit(int fps);

	// Toggles the dark theme of the window on or off as specified.
	void setDarkTheme(bool DARK_THEME);

	// Toggles the window on or off of full screen as specified.
	void setFullScreen(bool FULL_SCREEN);

	// Returns a string pointer to the title of the window.
	const char* getTitle() const;

	// Returns the dimensions vector of the window.
	Vector2i getDimensions() const;

	// Returns the position vector of the window.
	Vector2i getPosition() const;

	// Returs the current framerate of the window.
	float getFramerate() const;

private:
	// --- INTERNALS ---

	// Returns the HWND of the window.
	void* getWindowHandle();

	// Waits for a certain amount of time to keep the window
	// running stable at the desired framerate.
	void handleFramerate();

	// Contains the internal data used by the window.
	void* WindowData = nullptr;

	// No Window copies are allowed
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
};