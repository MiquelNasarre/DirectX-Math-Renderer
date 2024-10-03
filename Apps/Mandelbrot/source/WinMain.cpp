#include "Mandelbrot.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	try {

		return Mandelbrot().Run();

	}
	catch (const ExceptionClass& exc) {
		MessageBoxA(nullptr, exc.what(), exc.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& exc) {
		MessageBoxA(nullptr, exc.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...) {
		MessageBoxA(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}