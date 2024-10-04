#pragma once

#include "Exception/Exception.h"
#include "Timer.h"
#include "Graphics.h"
#include "iGManager.h"

class Window
{
public:
	class Exception : public ExceptionClass
	{
	public:
		Exception(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept;
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
	private:
		HRESULT hr;
	};

private:
	class WindowClass
	{
	public:
		static const LPCSTR GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr LPCSTR wndClassName = "Learn DirectX Window";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};

private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

	Vector2i Dimensions;
	Vector2i Position;
	std::string Name;
	HWND hWnd;

	MSG msg;
	BOOL gResult;
	static constexpr unsigned int maxMessages = 60u;
	std::queue<MSG> msgQueue;
	void pushMessage();

	void handleFramerate();
	bool noFrameUpdate = false;
	float frame = 0;
	float Frametime = 0;
	Timer timer;
	iGManager imGui;

public:

	Window(int width, int height, const char* Title, const char* IconFilename = "", bool darkTheme = true, GPU_PREFERENCE Preference = GPU_HIGH_PERFORMANCE);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	//	Client Functions

	Graphics graphics;

	void setTitle(std::string name);
	void setIcon(std::string filename);
	void setDimensions(int width, int height);
	void setDimensions(Vector2i Dim);
	void setPosition(int X, int Y);
	void setPosition(Vector2i Pos);
	void setFramerateLimit(int fps);
	void setDarkTheme(BOOL DARK_THEME);
	void setFullScreen(BOOL FULL_SCREEN);

	std::string getTitle();
	Vector2i getDimensions();
	Vector2i getPosition();
	float getFramerate();
	float getFrameTime();
	bool popMessage(MSG& clientMsg);
	void* getImGuiContext();

	bool processEvents();
	void close();
};