#pragma once
#include "Graphics.h"

/* WINDOW OBJECT CLASS
-------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------
To create a window in your desktop just create a Window object and you have it!
To close the window just delete the object and it is gone. No headaches, and no
non-sense. To know if a window close button has been pressed or close() has been
called the processEvents() will return the ID of the window that wants to close.

The window class deals with all the Win32 API in the background, and allows for a
flexible window design, with multiple functions to customize the window to your 
liking, check out the class to learn all its functionalities.

The library also supports multiple window settings. For proper opening and closing
I suggest holding pointers to windows and a counter, and deleting the windows when
the global process events returns their ID.

For App creation this API comes with a set of default Drawables and a Graphics 
object contained inside the window, for an example on how to use the library you
can check the demo apps or the default helpers header.

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
	friend class Graphics;
	friend class iGManager;
private:
	static inline unsigned next_id = 1u;	// Stores the next ID assigned to a window
	const unsigned w_id;					// Stores the window ID.
public:
	// Returns a reference to the window internal Graphics object.
	Graphics& graphics();

	// Function that may be called every frame during the App runtime. It manages the
	// message pipeline, pushing events to the Mouse and Keboard for user interaction,
	// sending them to ImGui if required, and doing important window internal updates.
	// It also manages the framerate, waiting the correct amount of time to keep it 
	// stable. 
	// The function returns 0 during normal runtime, but if a window close button has
	// been pressed or its close function has been called it will return the ID of the
	// corresponding window, so that the App can safely delete the object if it pleases.
	static unsigned processEvents();

	// Equivalent to pressing the close button on the window. Sends a message to the
	// handler and process events will catch it and return this window ID so that the
	// user can close it if he pleases. The only way of closing a window properly is 
	// through the desctructor.
	void close();

public:
	// --- CONSTRUCTOR / DESTRUCTOR ---

	// Creates the window and its associated Graphics object with the
	// specified dimensions, title, icon and theme.
	Window(Vector2i Dim, const char* Title);

	// Handles the proper deletion of the window data after its closing.
	~Window();

	// --- GETTERS / SETTERS ---

	// Returs the window ID. 
	// The one posted by process events when close button pressed.
	unsigned getID() const;

	// Checks whether the window has focus.
	bool hasFocus() const;

	// Set the title of the window, allows for formatted strings.
	void setTitle(const char* name, ...);

	// Sets the icon of the window via its filename (Has to be a .ico file).
	void setIcon(const char* filename);

	// Sets the dimensions of the window to the ones specified.
	void setDimensions(Vector2i Dim);

	// Sets the position of the window to the one specified.
	void setPosition(Vector2i Pos);

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

	// Sets the maximum framerate of the process to the one specified.
	// This framerate is controlled by the process events, after every
	// processing run if the time since last frame is too short it will 
	// wait to keep the framerate stable.
	static void setFramerateLimit(int fps);

	// Returs the current framerate of the process.
	static float getFramerate();
private:
	// --- INTERNALS ---

	// Returns the HWND of the window.
	void* getWindowHandle();

	// Waits for a certain amount of time to keep the window
	// running stable at the desired framerate.
	static void handleFramerate();
#ifdef _INCLUDE_IMGUI
	// Returns adress to the pointer to the iGManager bound to the window.
	// This is to be accessed by the iGManager and set the data accordingly.
	void** imGuiPtrAdress();
#endif
	// Contains the internal data used by the window.
	void* WindowData = nullptr;

	// No Window copies are allowed
	Window(Window&&) = delete;
	Window& operator=(Window&&) = delete;
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
};