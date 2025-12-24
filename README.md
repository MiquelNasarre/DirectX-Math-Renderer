# Chaotic

Library under development for 3D rendering apps in C++. Bloat free, minimal dependencies, minimal code.

No deployments yet. Still need to clean a lot of libraries and create a unified header chaotic.h and 
chaotic.lib, and add some additional features. It will be ready soon.

Right now the only way of using this library is cloning the entire VisualStudio solution and building it 
yourself. Then create a new project and add the Chaotic `include/` directory to its path.

## Usage

Demo is still not available, but basic usage allows for minimal lines of code to create very complex views. 
Including triangle meshes, math surfaces, curves, backgrounds, lights, and point and line meshes. And allows 
for textures, illumination and different coloring and rendering settings.

For a basic example, the following main file would create a window and plot an interactive surface given by the 
specified function, moveable with the mouse and mouse wheel. 

``` cpp
#include "chaotic_defaults.h"

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
```

This is just the most basic example, your only limit is imagination with this library, full deployment will be out soon.

## Requirements

- [Visual Studio](https://visualstudio.com) for Windows
- OS Windows 7 or newer

## License

Chaotic is released under the MIT License.
See the LICENSE file for details.

- Dear ImGui: Dear ImGui is licensed under the MIT License.
