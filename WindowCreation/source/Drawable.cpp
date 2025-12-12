#include "Drawable.h"
#include "Bindable/IndexBuffer.h"
#include "Bindable/Blender.h"
#include "Exception/_exDefault.h"

#include <memory>
#include <vector>
#include <typeinfo>

// Struct containing the bindable data of a given drawable object.
struct DrawableInternals
{
	// All bindables are found in this vector.
	std::vector<Bindable*> binds = {};

	~DrawableInternals()
	{
		for (auto& b : binds)
			delete b;
	}
};

/*
--------------------------------------------------------------------------------------------
 Drawable class Functions
--------------------------------------------------------------------------------------------
*/

// Constructor, allocates space to store the bindables.

Drawable::Drawable()
{
	DrawableData = new DrawableInternals;

	// If a global device has not been created yet create it.
	GlobalDevice::set_global_device();
}

// Destructor, deletes allovated storage space

Drawable::~Drawable()
{
	DrawableInternals& data = *((DrawableInternals*)DrawableData);

	delete (DrawableInternals*)DrawableData;
}

// Copies of drawable objects are not allowed.

void Drawable::Draw()
{
	if (!isInit)
		throw INFO_EXCEPT("You cannot issue a draw call if the drawable has not been initialized");

	_draw();
}

// Internal draw function, to be called by any overwriting of the
// main Draw() function, iterates through the bindable objects
// list and once all are bind, issues a draw call to the window.

void Drawable::_draw() const
{
	DrawableInternals& data = *((DrawableInternals*)DrawableData);

	unsigned indexCount = 0u;
	bool isOIT = false;

	for (auto& bind : data.binds)
	{
		// Look for the Blender in case it requires OIT.
		if (typeid(*bind) == typeid(Blender) && ((Blender*)bind)->getMode() == BLEND_MODE_OIT_WEIGHTED)
			isOIT = true;

		// Look for the IndexBuffer and store the index count.
		if (typeid(*bind) == typeid(IndexBuffer))
			indexCount = ((IndexBuffer*)bind)->getCount();

		// Bind all bindables.
		bind->Bind();
	}
	// Tell Graphics to draw.
	Graphics::drawIndexed(indexCount, isOIT);
}

// Adds a new bindable to the bindable list of the object. For proper
// memory management the bindable sent to this function must be allocated
// using new(), and the deletion must be left to the drawable management.

Bindable* Drawable::AddBind(Bindable* bind)
{
	DrawableInternals& data = *((DrawableInternals*)DrawableData);

	// Push the new bindable to the vector.
	data.binds.push_back(bind);

	// Return the pointer to the new bindable.
	return bind;
}

// Changes an existing bindable from the bindable list of the object. For proper
// memory management the bindable sent to this function must be allocated
// using new(), and the deletion must be left to the drawable management.

Bindable* Drawable::changeBind(Bindable* bind, unsigned N)
{
	DrawableInternals& data = *((DrawableInternals*)DrawableData);

	// delete old object, assign new object.
	delete data.binds[N];
	data.binds[N] = bind;

	return data.binds[N];
}
