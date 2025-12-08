#include "Drawable.h"
#include "Bindable/IndexBuffer.h"
#include "Exception/_exDefault.h"

#include <memory>
#include <vector>
#include <typeinfo>

// Struct containing the bindable data of a given drawable object.
struct DrawableInternals
{
	// The index buffer is stored here for easier use.
	const IndexBuffer* pIndexBuffer = nullptr;
	// All other bindables are found in this vector.
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
}

// Destructor, deletes allovated storage space

Drawable::~Drawable()
{
	DrawableInternals& data = *((DrawableInternals*)DrawableData);

	delete (DrawableInternals*)DrawableData;
}

// Copies of drawable objects are not allowed.

void Drawable::Draw(Window& _w)
{
	if (!isInit)
		throw INFO_EXCEPT("You cannot issue a draw call if the drawable has not been initialized");

	_draw(_w);
}

// Internal draw function, to be called by ahy overwriting of the
// main Draw() function, iterates through the bindable objects
// list and once all are bind, issues a draw call to the window.

void Drawable::_draw(Window& _w) const
{
	DrawableInternals& data = *((DrawableInternals*)DrawableData);

	for (auto& b : data.binds)
		b->Bind();

	_w.graphics().drawIndexed(data.pIndexBuffer->GetCount());
}

// Adds a new bindable to the bindable list of the object. For proper
// memory management the bindable sent to this function must be allocated
// using new(), and the deletion must be left to the drawable management.

Bindable* Drawable::AddBind(Bindable* bind)
{
	DrawableInternals& data = *((DrawableInternals*)DrawableData);

	// If the bindable is an index buffer store thr raw pointer in the data
	if(typeid(*bind) == typeid(IndexBuffer))
		data.pIndexBuffer = (IndexBuffer*)bind;

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

	// If we’re replacing an IndexBuffer, and this one was the active one, reset pointer
	if (data.pIndexBuffer == dynamic_cast<IndexBuffer*>(data.binds[N]))
		data.pIndexBuffer = nullptr;

	// unique_ptr assignment automatically deletes the old object
	delete data.binds[N];
	data.binds[N] = bind;

	// If new bind is an IndexBuffer, update pointer
	if (auto* ib = dynamic_cast<IndexBuffer*>(data.binds[N]))
		data.pIndexBuffer = ib;

	return data.binds[N];
}
