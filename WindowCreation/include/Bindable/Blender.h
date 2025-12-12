#pragma once
#include "Bindable.h"

/* BLENDER BINDABLE CLASS
-------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------
The blend state is the last step when drawing to a render target, and it decides how 
the output color of the pixel is combined with the existent color in the buffer
according to the colors distances and alpha value.
-------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------
*/

// Enumerator that defines the blending mode used by the blender.
enum BLEND_MODE
{
	BLEND_MODE_OPAQUE,			// Opaque surfaces
	BLEND_MODE_ALPHA,			// Standard alpha blending
	BLEND_MODE_ADDITIVE,		// Additive blending
	BLEND_MODE_OIT_WEIGHTED,	// Order Independent Transparency
};

// Blender bindable, decides how the colors are blended in the screen by the current drawable.
class Blender : public Bindable
{
	// Needs to acces the blending mode.
	friend class Drawable;
public:
	// Takes the blending mode as an input and creates the blending state accordingly.
	Blender(BLEND_MODE mode);

	// Releases the GPU pointer and deletes the data.
	~Blender() override;

	// Binds the Blender to the global context.
	void Bind() override;

private:
	// To be called by the draw call to check for OIT.
	BLEND_MODE getMode();

	// Pointer to the internal Blender data.
	void* BindableData = nullptr;
};