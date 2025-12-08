#pragma once
#include "Bindable.h"

/* DRAWABLE OBJECT BASE CLASS
-------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------
This header contains the base class for all drawable objects.
Minimal class includes a virtual draw function optionally to be
overwritten. Some internal functions and internal data to store 
the bindables.

See drawable object source files for reference on how to implement
different drawable classes.
-------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------
*/

// Drawable base class, contains minimal functions to be 
// used by any drawable object.
class Drawable
{
protected:
	// Constructor, allocates space to store the bindables.
	Drawable();

public:
	// Destructor, deletes allovated storage space.
	virtual ~Drawable();

	// Copies of drawable objects are not allowed.
	Drawable(Drawable&&) = delete;
	Drawable& operator=(Drawable&&) = delete;

	// Virtual drawing function, checks that the object is initialized
	// and calls the internal draw function.
	virtual void Draw(Window& _w);

protected:
	// Boolean to make sure the drawable has been initialized.
	bool isInit = false;

	// Internal draw function, to be called by ahy overwriting of the
	// main Draw() function, iterates through the bindable objects
	// list and once all are bind, issues a draw call to the window.
	void _draw(Window& _w) const;

	// Adds a new bindable to the bindable list of the object. For proper
	// memory management the bindable sent to this function must be allocated
	// using new(), and the deletion must be left to the drawable management.
	Bindable* AddBind(Bindable* bind);

	// Changes an existing bindable from the bindable list of the object. For proper
	// memory management the bindable sent to this function must be allocated
	// using new(), and the deletion must be left to the drawable management.
	Bindable* changeBind(Bindable* bind, unsigned N);

private:
	void* DrawableData; // Stores the internal data of the drawable.
};