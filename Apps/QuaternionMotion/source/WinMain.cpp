#include "default_helpers.h"

void __stdcall WinMain()
{
	SURFACE_DESC desc = {};
	desc.explicit_func = [](float x, float y) { return cosf(10.f * (x * x + y * y)) / 5.f; };
	Surface surf(&desc);

	defaultWindow window({ 640,320 });
	window.pushDrawable(&surf);

	while (Window::processEvents() != window.getID())
	{
		defaultEventManager(window.data);
		surf.updateRotation(window.data.rot_free);
		window.scale = window.data.scale;
		window.drawFrame();
	}
}