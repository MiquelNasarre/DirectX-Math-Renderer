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
	GlobalDevice() = default;
	~GlobalDevice();

	static inline void* globalDeviceData = nullptr; // Stores the device data masked as void*
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

private:
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
	Graphics(Graphics&&) = delete;
	Graphics& operator=(Graphics&&) = delete;
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;

	// Clears the depth buffer so that all objects painted are moved to the back.
	void clearDepthBuffer();

	// Calls to draw the objects as indexed in the index count.
	static void drawIndexed(unsigned IndexCount, bool isOIT);

	// Sets the window dimensions to the ones specified by the vector.
	void setWindowDimensions(const Vector2i Dim);

	// Returns the current window dimensions.
	Vector2i getWindowDimensions() const;

public:
	// Before issuing any draw calls to the window, for multiple window settings 
	// this function has to be called to bind the window as the render target.
	void setRenderTarget();

	// Swaps the current frame and shows the new frame to the window.
	void pushFrame();

	// Clears the buffer with the specified color.
	void clearBuffer(Color color);

	// Simple conversion from a pixel position on screen to a (-1.0,1.0)x(-1.0,1.0) c R^2 position.
	Vector2f PixeltoR2(const Vector2i MousePos);

	// To draw transparent objects this setting needs to be toggled on, it causes extra 
	// conputation due to other buffers being used for rendering, so only turn on if needed.
	// It uses the McGuire/Bavoli OIT approach. For more information you can check the 
	// original paper at: https://jcgt.org/published/0002/02/09/
	void enableOITransparency();

	// Deletes the extra buffers and disables the extra steps when pushing frames.
	void disableOITransparency();

	// Returns whether OITransparency is enabled on this Graphics object.
	bool isOITransparencyEnabled() const;

	// Updates the perspective on the window, by changing the observer direction, 
	// the center of the POV and the scale of the object looked at.
	void updatePerspective(Quaternion obs, Vector3f center, float scale);

	// Returns the current observer quaternion.
	Quaternion getObserver() const;

	// Returns the current Center POV.
	Vector3f getCenter() const;

	// Returns the current scals.
	float getScale() const;

private:
	void* GraphicsData = nullptr; // Stores the graphics object internal data.

	static inline Graphics* currentRenderTarget = nullptr; // Stores the pointer to the current graphics target.

	struct // Constant buffer of the current graphics perspective, accessable to all vertex shaders.
	{
		Quaternion observer = 1.f;	// Stores the current observer direction.
		_float4vector center = {};	// Stores the current center of the POV.
		_float4vector scaling = {};	// Scaling values for the shader
	}cbuff;		

	Vector2i WindowDim;			// Stores the current window dimensions.
	float Scale = 250.f;		// Stores the current view scale.
};
