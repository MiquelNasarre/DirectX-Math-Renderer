#pragma once
#include "Window.h"

/* BINDABLE OBJECT BASE CLASS
-------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------
This header contains the base class for all bindable objects.
Minimal class includes a virtual binding function to be defined
by each bindable and a default virtual deleter.

See bindable object source files for reference on how to implement 
different bindable classes.
-------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------
*/

// Bindable base class, contains the minimal functions expeted by 
// any bindable object to have.
class Bindable
{
public:
	// Bind function to be defined by each bindable object.
	virtual void Bind() = 0;

	// Deleter function optionally overwritten by inherited class.
	virtual ~Bindable() = default;

protected:
	// Helper function that returns the pointer to the global device.
	static void* device() { return GlobalDevice::get_device_ptr(); }

	// Helper function that returns the pointer to the global context.
	static void* context() { return GlobalDevice::get_context_ptr(); }
};