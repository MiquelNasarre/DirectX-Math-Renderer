#include "Window.h"
#include "Exception/_exWindow.h"

#include "Timer.h"
#include "Graphics.h"
#include "iGManager.h"
#include "Keyboard.h"
#include "Mouse.h"

#include <windowsx.h>
#include <dwmapi.h>
#include <queue>
#include <string>
#include <cstdarg>

/*
--------------------------------------------------------------------------------------------
 Window Internal Data struct
--------------------------------------------------------------------------------------------
*/

// Custom message that signals a window close button pressed.
#define WM_APP_WINDOW_CLOSE		(WM_APP + 1)

// The maximum amount of messages to be stored on queue.
#define MAX_MSG		128 

using std::queue;
using std::string;

// This structure contains the internal data allocated by every window.
struct WindowInternals
{
	Graphics* graphics = nullptr;	// Graphics object of the window.
	iGManager* imGui = nullptr;		// Pointer to the current iGManager.
	static inline Timer timer;		// Timer object to keep track of the framerate.

	Vector2i Dimensions = {};	// Dimensions of the window.
	Vector2i Position = {};		// Position of the window.
	string Name = {};			// Name of the window.
	HWND hWnd = nullptr;		// Handle to the window.

	static inline bool noFrameUpdate = false;	// Schedules next frame time to be skipped.
	static inline float frame = 0.f;			// Stores the time of the last frame push.
	static inline float Frametime = 0.f;		// Stores the specified time for each frame.
};

/*
--------------------------------------------------------------------------------------------
 MSG Pipeline Static Class
--------------------------------------------------------------------------------------------
*/

// Static class that handles the process messages and sets custom procedures.
class MSGHandlePipeline
{
public:
	// Custom procedure for window message handling, Stores mouse and keyboard events and
	// handles other specific window events. Other events are sent to the defaul procedure.
	static LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, WindowInternals* _data) noexcept
	{
		WindowInternals& data = *_data;

		//	This function handles all the messages send by the computer to the application

		// Priority cases I want to always take care of

		switch (msg)
		{
		case WM_CLOSE:
		{
			// Get the Window* for this HWND (you already do this in your thunk)
			Window* pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
			if (!pWnd)
				return DefWindowProc(hWnd, msg, wParam, lParam);

			// Send a custom message to the thread's message queue
			PostThreadMessage(GetCurrentThreadId(), WM_APP_WINDOW_CLOSE, (WPARAM)pWnd->getID(), 0);
			return 0;
		}

		case WM_SIZE:
			data.Dimensions.x = LOWORD(lParam);
			data.Dimensions.y = HIWORD(lParam);
			data.graphics->setWindowDimensions(data.Dimensions);
			break;

		case WM_MOVE:
			data.noFrameUpdate = true;
			data.Position.x = LOWORD(lParam);
			data.Position.y = HIWORD(lParam);
			break;

		case WM_KILLFOCUS:
			Keyboard::clearKeyStates();
			Mouse::resetWheel();
			Mouse::clearBuffer();
			break;
		}

		// Let ImGui handle the rest if he has focus

		if (iGManager::WndProcHandler(hWnd, msg, (unsigned int)wParam, (unsigned int)lParam))
			return DefWindowProc(hWnd, msg, wParam, lParam);

		// Other lesser cases

		switch (msg)
		{
		case WM_CHAR:
			Keyboard::pushChar((char)wParam);
			break;

		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			if (Keyboard::getAutorepeat() || !Keyboard::isKeyPressed((unsigned char)wParam))
				Keyboard::pushEvent(Keyboard::event::Pressed, (unsigned char)wParam);
			Keyboard::setKeyPressed((unsigned char)wParam);
			break;

		case WM_SYSKEYUP:
		case WM_KEYUP:
			Keyboard::setKeyReleased((unsigned char)wParam);
			Keyboard::pushEvent(Keyboard::event::Released, (unsigned char)wParam);
			break;

		case WM_LBUTTONDOWN:
			Mouse::pushEvent(Mouse::event::Pressed, Mouse::Left);
			Mouse::setButtonPressed(Mouse::Left);
			SetCapture(hWnd);
			break;

		case WM_LBUTTONUP:
			Mouse::pushEvent(Mouse::event::Released, Mouse::Left);
			Mouse::setButtonReleased(Mouse::Left);
			ReleaseCapture();
			break;

		case WM_RBUTTONDOWN:
			Mouse::pushEvent(Mouse::event::Pressed, Mouse::Right);
			Mouse::setButtonPressed(Mouse::Right);
			break;

		case WM_RBUTTONUP:
			Mouse::pushEvent(Mouse::event::Released, Mouse::Right);
			Mouse::setButtonReleased(Mouse::Right);
			break;

		case WM_MBUTTONDOWN:
			Mouse::pushEvent(Mouse::event::Pressed, Mouse::Middle);
			Mouse::setButtonPressed(Mouse::Middle);
			break;

		case WM_MBUTTONUP:
			Mouse::pushEvent(Mouse::event::Released, Mouse::Middle);
			Mouse::setButtonReleased(Mouse::Middle);
			break;

		case WM_MOUSEMOVE:
			Mouse::setPosition(Vector2i(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
			Mouse::setScPosition(Vector2i(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)) + data.Position);
			Mouse::pushEvent(Mouse::event::Moved, Mouse::None);
			break;

		case WM_MOUSEWHEEL:
			Mouse::increaseWheel(GET_WHEEL_DELTA_WPARAM(wParam));
			Mouse::pushEvent(Mouse::event::Wheel, Mouse::None);
			break;
		}

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	// Trampoline used to call the custom message pipeline to the specific window data.
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		// retrieve ptr to window class
		Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if (!pWnd || !pWnd->WindowData)
			return DefWindowProc(hWnd, msg, wParam, lParam);
		// forward message to window class handler
		return HandleMsg(hWnd, msg, wParam, lParam, (WindowInternals*)pWnd->WindowData);
	}

	// Setup to create the trampoline method. Creates a virtual window that points to the 
	// actual window and is accessible by the trampoline via its handle.
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		// use create parameter passed in from CreateWindow() to store window class pointer at WinAPI side
		if (msg == WM_NCCREATE)
		{
			// extract ptr to window class from creation data
			const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
			Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
			// set WinAPI-managed user data to store ptr to window class
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
			// set message proc to normal (non-setup) handler now that setup is finished
			SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&HandleMsgThunk));
			// forward message to window class handler
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
		// if we get a message before the WM_NCCREATE message, handle with default handler
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
};

/*
--------------------------------------------------------------------------------------------
 Instance creation Helper class
--------------------------------------------------------------------------------------------
*/

// Global window class that creates the handle to the instance
// and assings a name and icon to the process.
class WindowClass
{
public:
	// Returns the name of the window class.
	static const LPCSTR GetName() noexcept
	{
		return wndClassName;
	}
	// Returns the handle to the instance.
	static HINSTANCE GetInstance() noexcept
	{
		return wndClass.hInst;
	}

private:
	// Private constructor creates the instance.
	WindowClass() noexcept
		:hInst{ GetModuleHandle(nullptr) }
	{
		//	Register Window class

		WNDCLASSEXA wc = { 0 };
		wc.cbSize = sizeof(wc);
		wc.style = CS_OWNDC;
		wc.lpfnWndProc = MSGHandlePipeline::HandleMsgSetup;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = GetInstance();
		wc.hIcon = static_cast<HICON>(LoadImageA(0, (RESOURCES_DIR + std::string("Icon.ico")).c_str(), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE | LR_SHARED));
		//wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = nullptr;
		wc.lpszMenuName = nullptr;
		wc.lpszClassName = GetName();
		wc.hIconSm = nullptr;
		RegisterClassExA(&wc);
	}
	// Private destructor deletes the instance.
	~WindowClass()
	{
		UnregisterClassA(wndClassName, GetInstance());
	}

	WindowClass(const WindowClass&) = delete;
	WindowClass& operator=(const WindowClass&) = delete;
	static constexpr LPCSTR wndClassName = "DirectX Window";
	static WindowClass wndClass;
	HINSTANCE hInst;
};

WindowClass WindowClass::wndClass;

/*
--------------------------------------------------------------------------------------------
 Window Class Functions
--------------------------------------------------------------------------------------------
*/

// Returns a reference to the window internal Graphics object.

Graphics& Window::graphics()
{
	WindowInternals& data = *((WindowInternals*)WindowData);

	return *data.graphics;
}

// Loops throgh the messages, pushes them to the queue and translates them.
// It reuturns 0 unless a window close button is pressed, in that case it 
// returns the window ID of that specific window.

unsigned Window::processEvents()
{
	//	Message Pump
	MSG msg;

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_APP_WINDOW_CLOSE)
			return (unsigned)msg.wParam; // Close Window ID

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	handleFramerate();
	return 0;
}

// Closes the window.

void Window::close()
{
	PostQuitMessage(0);
}

// Creates the window and its associated Graphics object with the
// specified dimensions, title, icon and theme.

Window::Window(Vector2i Dim, const char* Title, const char* IconFilename, bool darkTheme): w_id { next_id++ }
{
	//	Calculate window size based on desired client region size

	RECT wr;
	wr.left = 100;
	wr.right = Dim.x + wr.left;
	wr.top = 100;
	wr.bottom = Dim.y + wr.top;
	if (!AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU | WS_OVERLAPPEDWINDOW, FALSE))
		throw WND_LAST_EXCEPT();

	//	Create Window & get hWnd

	HWND hWnd = CreateWindowExA(
		NULL,
		WindowClass::GetName(), 
		NULL,
		WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		wr.right - wr.left, 
		wr.bottom - wr.top,
		nullptr, 
		nullptr, 
		WindowClass::GetInstance(),
		this
	);

	//	Check for error

	if (!hWnd)
		throw WND_LAST_EXCEPT();

	// Create internal data
	WindowData = new WindowInternals;
	WindowInternals& data = *((WindowInternals*)WindowData);

	data.graphics	= new Graphics(hWnd);
	data.hWnd		= hWnd;
	data.Dimensions = Dim;

	//	Set title & icon & theme

	setTitle(Title);
	if (IconFilename[0])
		setIcon(IconFilename);
	if (darkTheme)
		setDarkTheme(TRUE);

	//	Create graphics object

	data.graphics->setWindowDimensions(data.Dimensions);
	
}

// Handles the proper deletion of the window data after its closing.

Window::~Window()
{
	WindowInternals& data = *((WindowInternals*)WindowData);

	// If an imgui instance is bound to the window unbind it.
	if (data.imGui)
		data.imGui->unbind();

	delete data.graphics;

	DestroyWindow(data.hWnd);

	delete &data;
}

// Returs the window ID. 
// The one posted by process events when cluse button pressed.

unsigned Window::getID() const
{
	return w_id;
}

// Checks whether the window has focus.

bool Window::hasFocus() const
{
	WindowInternals& data = *((WindowInternals*)WindowData);
	return (GetForegroundWindow() == data.hWnd);
}

// Set the title of the window, allows for formatted strings.

void Window::setTitle(const char* fmt_name, ...)
{
	va_list ap;

	// Unwrap the format
	char name[512];

	va_start(ap, fmt_name);
	if (vsnprintf(name, 512, fmt_name, ap) < 0) return;
	va_end(ap);

	WindowInternals& data = *((WindowInternals*)WindowData);

	data.Name = name;

	if (!IsWindow(data.hWnd))
		return;
	if (!SetWindowTextA(data.hWnd, name))
		throw WND_LAST_EXCEPT();
}

// Sets the icon of the window via its filename (Has to be a .ico file).

void Window::setIcon(const char* filename)
{
	WindowInternals& data = *((WindowInternals*)WindowData);

	HANDLE hIcon = LoadImageA(0, filename, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE | LR_SHARED);
	if (hIcon) {
		//	Change both icons to the same icon handle
		SendMessage(data.hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
		SendMessage(data.hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

		//	This will ensure that the application icon gets changed too
		SendMessage(GetWindow(data.hWnd, GW_OWNER), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
		SendMessage(GetWindow(data.hWnd, GW_OWNER), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	}
	else throw WND_LAST_EXCEPT();
}

// Sets the dimensions of the window to the ones specified.

void Window::setDimensions(Vector2i Dim)
{
	int width = Dim.x;
	int height = Dim.y;
	
	WindowInternals& data = *((WindowInternals*)WindowData);

	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	if (!AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_MAXIMIZEBOX | WS_OVERLAPPEDWINDOW, FALSE))
		throw WND_LAST_EXCEPT();

	if (!SetWindowPos(data.hWnd, data.hWnd, 0, 0, wr.right - wr.left, wr.bottom - wr.top, SWP_NOMOVE | SWP_NOZORDER))
		throw WND_LAST_EXCEPT();
}

// Sets the position of the window to the one specified.

void Window::setPosition(Vector2i Pos)
{
	int X = Pos.x, Y = Pos.y;

	WindowInternals& data = *((WindowInternals*)WindowData);

	int eX = -8;
	int eY = -31;
	X += eX;
	Y += eY;

	if (!SetWindowPos(data.hWnd, data.hWnd, X, Y, 0, 0, SWP_NOSIZE | SWP_NOZORDER))
		throw WND_LAST_EXCEPT();
}

// Sets the maximum framerate of the widow to the one specified.

void Window::setFramerateLimit(int fps)
{
	WindowInternals::Frametime = 1.f / float(fps);
	WindowInternals::timer.setMax(fps);
}

// Toggles the dark theme of the window on or off as specified.

void Window::setDarkTheme(bool DARK_THEME)
{
	WindowInternals& data = *((WindowInternals*)WindowData);

	if (FAILED(DwmSetWindowAttribute(data.hWnd, DWMWINDOWATTRIBUTE::DWMWA_USE_IMMERSIVE_DARK_MODE, &DARK_THEME, sizeof(BOOL))))
		throw WND_LAST_EXCEPT();

	// Window dimensions wiggle to force and udate.
	setDimensions(data.Dimensions - Vector2i(1, 1));
	setDimensions(data.Dimensions + Vector2i(1, 1));
}

// Toggles the window on or off of full screen as specified.

void Window::setFullScreen(bool FULL_SCREEN)
{
	WindowInternals& data = *((WindowInternals*)WindowData);

	static WINDOWPLACEMENT g_wpPrev;

	DWORD dwStyle = GetWindowLong(data.hWnd, GWL_STYLE);
	if (dwStyle & WS_OVERLAPPEDWINDOW && FULL_SCREEN) {
		MONITORINFO mi = { sizeof(mi) };
		if (GetWindowPlacement(data.hWnd, &g_wpPrev) && GetMonitorInfo(MonitorFromWindow(data.hWnd,MONITOR_DEFAULTTOPRIMARY), &mi))
		{
			SetWindowLongPtr(data.hWnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
			SetWindowPos(data.hWnd, HWND_TOP,
				mi.rcMonitor.left, 
				mi.rcMonitor.top,
				mi.rcMonitor.right - mi.rcMonitor.left,
				mi.rcMonitor.bottom - mi.rcMonitor.top,
				SWP_NOOWNERZORDER | SWP_FRAMECHANGED
			);
		}
	}
	else if (!FULL_SCREEN)
	{
		SetWindowLongPtr(data.hWnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(data.hWnd, &g_wpPrev);
	}
}

// Returns a string pointer to the title of the window.

const char* Window::getTitle() const
{
	WindowInternals& data = *((WindowInternals*)WindowData);

	return data.Name.c_str();
}

// Returns the dimensions vector of the window.

Vector2i Window::getDimensions() const
{
	WindowInternals& data = *((WindowInternals*)WindowData);

	return data.Dimensions;
}

// Returns the position vector of the window.

Vector2i Window::getPosition() const
{
	WindowInternals& data = *((WindowInternals*)WindowData);

	return data.Position;
}

// Returs the current framerate of the window.

float Window::getFramerate()
{
	return 1.f / WindowInternals::timer.average();
}

// Returns the HWND of the window.

void* Window::getWindowHandle()
{
	WindowInternals& data = *((WindowInternals*)WindowData);

	return data.hWnd;
}

// Waits for a certain amount of time to keep the window
// running stable at the desired framerate.

void Window::handleFramerate()
{
	if (WindowInternals::noFrameUpdate)
	{
		WindowInternals::noFrameUpdate = false;
		WindowInternals::frame = WindowInternals::timer.skip();
		return;
	}

	if (WindowInternals::timer.check() < WindowInternals::Frametime)
		Timer::sleep_for(unsigned long(1000 * (WindowInternals::Frametime - WindowInternals::timer.check())));

	WindowInternals::frame = WindowInternals::timer.mark();
}

// Returns adress to the pointer to the iGManager bound to the window.
// This is to be accessed by the iGManager and set the data accordingly.

void** Window::imGuiPtrAdress()
{
	WindowInternals& data = *((WindowInternals*)WindowData);

	return (void**)&data.imGui;
}
