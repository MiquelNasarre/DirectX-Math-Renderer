#pragma once
#include "Header.h"

/* GRAPHICS OBJECT CLASS
-------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------
This header contains the Graphics object and its functions. All windows contain
their own graphics object which controls the GPU pointers regarding that window.

Also due to the nature of the API the class also contains the POV of the window,
including a direction of view and a center. All drawables have acces to this buffer
and the shader are built accordingly. The buffer can be modified via class functions.
-------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------
*/

// Class to manage the global device shared by all windows and 
// graphics instances along the process.
class GlobalDevice
{
	friend class Bindable;
	friend class Graphics;
	friend class iGManager;
private:
	// Helper to delete the global device at the end of the process.
	static GlobalDevice helper;
	GlobalDevice() {}
	~GlobalDevice();

	static void* globalDeviceData; // Stores the device data masked as void*
public:
	// Different GPU preferences following the IDXGIFactory6::EnumAdapterByGpuPreference
	// GPU distinction layout. 
	enum GPU_PREFERENCE
	{
		GPU_HIGH_PERFORMANCE,
		GPU_MINIMUM_POWER,
		GPU_UNSPECIFIED,
	};
	// Sets the global devica according to the GPU preference, must be set before
	// creating any window instance, otherwise it will be ignored.
	// If none is set it will automatically be created by the first window creation.
	static void set_global_device(GPU_PREFERENCE preference = GPU_HIGH_PERFORMANCE);
protected:
	// Internal function that returns the ID3D11Device* masked as a void*.
	static void* get_device_ptr();
	// Internal function that returns the ID3D11DeviceContext* masked as void*
	static void* get_context_ptr();
};

// Contains the graphics information of the window, storing the GPU access pointers
// as well as the POV of the windows, draw calls are funneled through this object.
class Graphics
{
	friend class Drawable;
	friend class Window;
	friend class MSGHandlePipeline;

private:
	// Destroys the class data and frees the pointers to the graphics instance.
	~Graphics();

	// Initializes the class data and calls the creation of the graphics instance.
	// Initializes all the necessary GPU data to be able to render the graphics objects.
	Graphics(void* hWnd);

	// No copies of a graphics instance are allowed.
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;

	// Clears the depth buffer so that all objects painted are moved to the back.
	void clearDepthBuffer();

	// Calls to draw the objects as indexed in the index count.
	void drawIndexed(unsigned IndexCount);

	// Sets the window dimensions to the ones specified by the vector.
	void setWindowDimensions(const Vector2i Dim);

public:
	// Swaps the current frame and shows the new frame to the window.
	void pushFrame();

	// Clears the buffer with the specified color.
	void clearBuffer(Color color);

	// Simple conversion from a pixel position on screen to a (-1.0,1.0)x(-1.0,1.0) c R^2 position.
	Vector2f PixeltoR2(const Vector2i MousePos);

	// Updates the perspective on the window, by changing the observer direction, 
	// the center of the POV and the scale of the object looked at.
	void updatePerspective(Vector3f obs, Vector3f center, float scale);

	// Returns the current window dimensions as an interger vector.
	Vector2i getWindowDimensions() const;

	// Returns the current observer direction vector.
	Vector3f getObserver() const;

	// Returns the current Center POV.
	Vector3f getCenter() const;

	// Returns the current scals.
	float getScale() const;

private:
	void* GraphicsData = nullptr; // Stores the graphics object internal data.

	struct {
		_float4matrix perspective = {};
		_float4vector traslation = {};
	}cbuff;

	Vector2i WindowDim;
	Vector3f Observer = { 0.f , -1.f , 0.f };
	Vector3f Center = { 0.f , 0.f , 0.f };
	float Scale = 250.f;
};
