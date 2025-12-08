#pragma once

/* EXCEPTION BASE CLASS
-------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------
This header contains the base class for all exception types defined
across the library, for proper exception management you can wrapp your
entire app call in try/catch and catch this exception type.

The what() and GetType() functions are virtual and will describe the 
specifics of the thrown exception.
-------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------
*/

// Exception Base class layout, the what() and GetType() function
// contain the relevant information from the specific exception.
class Exception
{
protected:
	// Default constructor, Only accessable to inheritance.
	// Stores the code line and file where the exception was thrown.
	Exception(int line, const char* file) noexcept;

public:
	// Virtual function that returns the description of the exception.
	// Each exception type can override this default method.
	virtual const char* what() const noexcept;

	// Returns the exception type, to be overwritten by inheritance.
	virtual const char* GetType() const noexcept = 0;

	// Returns the line of the code file the exception was thrown.
	int GetLine() const noexcept;

	// Returns the file path of the file where the exception was thrown.
	const char* GetFile() const noexcept;

	// Returns the original code string where the exception was thrown.
	const char* GetOriginString() const noexcept;

	// Creates a default message box using Win32 with the exception data.
	void PopMessageBox() const noexcept;
private:
	int line;	// Stores the line where the exception was thrown.
	char* file;	// Stores the file where the exception was thrown.

protected:
	// Pointer to the what buffer to be used by the inheritance.
	mutable char* whatBuffer;
};
