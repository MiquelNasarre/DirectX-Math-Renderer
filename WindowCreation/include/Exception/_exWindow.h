#pragma once
#include "Exception/Exception.h"
#include "dxerr.h"

#include <string>

/* WIN32 EXCEPTION CLASS
-------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------
This header contains the windows exception class, to be used when calling Win32
API functions that can throw errors but are not part of the DXGI API functions.

The class takes an HRESULT as an input and decodes it using the Win32 FormatMessage
functions. By the macros default it retrieves the last error by calling GetLastError().
-------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------
*/

// Macro to retrieve the last error code amd create a Window Exception with it.
#define WND_LAST_EXCEPT()	WindowException( __LINE__,__FILE__,GetLastError() )

// Win32 Exception class, when an API function call returns something unexpected
// the HRESULT from that function is retrieved and used to translate the error code.
// This error code and string can be accessed via the what function.
class WindowException : public Exception
{
public:
	// Constructor initializes the public Exception and stores the HRESULT.
	WindowException(int line, const char* file, HRESULT hr) noexcept
		: Exception(line, file), hr(hr)
	{
	}

	// Returns a string containing the error code, the code description,
	// obtained via TranslateErrorCode(hr), and the exception origin.
	const char* what() const noexcept override
	{
		auto message = new std::string(
			"[Error Code] " + std::to_string(hr) + "\n" +
			"[Description] " + TranslateErrorCode(hr) + "\n" +
			GetOriginString()
		);
		whatBuffer = (char*)message->c_str();
		return whatBuffer;
	}

	// Win32 Exception type override.
	const char* GetType() const noexcept override
	{
		return "Win32 Exception";
	}

private:
	// Retrieves the error string from a given Win32 failed HRESULT 
	// using the FormatMessage method and returns the string.
	static const char* TranslateErrorCode(HRESULT hr) noexcept
	{
		char* pMsgBuf = nullptr;
		// windows will allocate memory for err string and make our pointer point to it
		DWORD nMsgLen = FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
		);
		// 0 string length returned indicates a failure
		if (nMsgLen == 0)
			return "Unidentified error code";

		// return error string from windows-allocated buffer string
		return pMsgBuf;
	}

	// Stores the FAILED HRESULT
	HRESULT hr;
};

