#pragma once
#include "Exception/Exception.h"

/* DEFAULT EXCEPTION CLASS
-------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------
This header contains the default exception class thrown by the
library when no specific exception is being thrown.

Contains the line and file and a description of the excetion that
can be entered as a single string or as a list of strings.
For user created exceptions please use this one.
-------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------
*/

#define INFO_EXCEPT(info)	InfoException(__LINE__, __FILE__, (info))

// Basic Exception class, stores the given information and adds it
// to the whatBuffer when the what() function is called.
class InfoException : public Exception
{
public:
	// Single message constructor, the message is stored in the info.
	InfoException(int line, const char* file, const char* msg) noexcept
		: Exception(line, file)
	{
		unsigned c = 0u;
		info = new char[2048];

		// Add intro to information.
		const char* intro = "\n[Description]\n";
		unsigned i = 0u;
		while (intro[i] && c < 2047)
			info[c++] = intro[i++];

		// join all info messages with newlines into single string.
		i = 0u;
		while (msg[i] && c < 2047)
			info[c++] = msg[i++];

		// Add intro to origin string.
		const char* origin_intro = "\n[Error Info]\n";
		i = 0u;
		while (origin_intro[i] && c < 2047)
			info[c++] = origin_intro[i++];

		// Add origin location.
		const char* origin = GetOriginString();
		i = 0u;
		while (origin[i] && c < 2047)
			info[c++] = origin[i++];

		// Add final EOS
		info[c] = '\0';
	}

	// Multiple messages constructor, the messages are stored in the info.
	InfoException(int line, const char* file, const char** infoMsgs = nullptr) noexcept
		:Exception(line, file)
	{
		unsigned c = 0u;
		info = new char[2048];

		// Add intro to information.
		const char* intro = "\n[Description]\n";
		unsigned i = 0u;
		while (intro[i] && c < 2047)
			info[c++] = intro[i++];

		// join all info messages with newlines into single string.
		i = 0u;
		while (infoMsgs[i])
		{
			unsigned j = 0u;
			while (infoMsgs[i][j] && c < 2047)
				info[c++] = infoMsgs[i][j++];

			if(infoMsgs[++i] && c < 2047)
				info[c++]= '\n';
		}

		// Add intro to origin string.
		const char* origin_intro = "\n[Error Info]\n";
		i = 0u;
		while (origin_intro[i] && c < 2047)
			info[c++] = origin_intro[i++];

		// Add origin location.
		const char* origin = GetOriginString();
		i = 0u;
		while (origin[i] && c < 2047)
			info[c++] = origin[i++];

		// Add final EOS
		info[c] = '\0';
	}

	// Deletes the string pointer.
	~InfoException()
	{
		if (info)
			delete[] info;
	}

	// Override method, prints the stored information and the 
	// position the exception was thrown at.
	const char* what() const noexcept override
	{
		return info;
	}

	// Info Exception type override.
	const char* GetType() const noexcept override
	{
		return "Graphics Info Exception";
	}

private:
	// Exception information storage.
	char* info = nullptr;
};
