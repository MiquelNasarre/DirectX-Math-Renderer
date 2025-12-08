#include "Exception/Exception.h"
#include <string>

// Default constructor, Only accessable to inheritance.
// Stores the code line and file where the exception was thrown.

Exception::Exception(int line, const char* _file) noexcept
	: line(line)
{
	file = new char[512];

	int i = -1;
	while (_file[++i]) file[i] = _file[i];
	file[i] = '\0';
}

// Virtual function that returns the description of the exception.
// Each exception type can override this default method.

const char* Exception::what() const noexcept
{
	auto message = new std::string(GetOriginString());
	whatBuffer = (char*)message->c_str();
	return whatBuffer;
}

// Returns the line of the code file the exception was thrown.

int Exception::GetLine() const noexcept
{
	return line;
}

// Returns the file path of the file where the exception was thrown.

const char* Exception::GetFile() const noexcept
{
	return file;
}

// Returns the original code string where the exception was thrown.

const char* Exception::GetOriginString() const noexcept
{
	auto string = new std::string("[File] " + std::string(file) + "\n[Line] " + std::to_string(line));
	return string->c_str();
}

// Creates a default message box using Win32 with the exception data.
#include "WinHeader.h"

void Exception::PopMessageBox() const noexcept
{
	MessageBoxA(nullptr, what(), GetType(), MB_OK | MB_ICONEXCLAMATION);
}