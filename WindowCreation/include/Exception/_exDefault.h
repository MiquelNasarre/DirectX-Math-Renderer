#pragma once
#include "Exception/Exception.h"
#include <string>

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
		: Exception(line, file), info{ msg }
	{
	}

	// Multiple messages constructor, the messages are stored in the info.
	InfoException(int line, const char* file, const char** infoMsgs = nullptr) noexcept
		:Exception(line, file)
	{
		// join all info messages with newlines into single string

		if (!infoMsgs)
			return;

		unsigned int i = 0;
		while (infoMsgs[i])
		{
			info += infoMsgs[i];
			info.push_back('\n');
			i++;
		}

		// remove final newline if exists

		if (!info.empty())
			info.pop_back();
	}

	// Override method, prints the stored information and the 
	// position the exception was thrown at.
	const char* what() const noexcept override
	{
		auto message = new std::string(std::string("\n[Error Info]\n") + info + "\n[Exception Thrown At]\n" + GetOriginString());
		whatBuffer = (char*)message->c_str();
		return whatBuffer;
	}

	// Info Exception type override.
	const char* GetType() const noexcept override
	{
		return "Graphics Info Exception";
	}

private:
	// Exception information storage.
	std::string info;
};
