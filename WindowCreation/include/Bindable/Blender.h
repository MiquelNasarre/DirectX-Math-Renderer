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
typedef enum BLEND_MODE
{
	BLEND_MODE_OPAQUE = 0,			// Opaque surfaces
	BLEND_MODE_ALPHA = 1,			// Standard alpha blending
	BLEND_MODE_ADDITIVE = 2,		// Additive blending
	BLEND_MODE_OIT_WEIGHTED = 3,	// Order Independent Transparency
};

class Blender : public Bindable
{
public:
	// Takes the blending mode as an input and creates the blending state accordingly.
	Blender(BLEND_MODE mode);

	// Releases the GPU pointer and deletes the data.
	~Blender() override;

	// Binds the Blender to the global context.
	void Bind() override;

private:
	// Pointer to the internal Blender data.
	void* BindableData = nullptr;
};