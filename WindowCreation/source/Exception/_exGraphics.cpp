#include "Exception/_exGraphics.h"
#include "WinHeader.h"
#include <stdio.h>

// Constructor, takes in a FAILED HRESULT and an optional list of
// messages and stores it in memory for future what() call.

HrException::HrException(int line, const char* file, long hr, const char** infoMsgs) noexcept
	: Exception(line, file), hr(hr)
{
	// join all info messages with newlines into single string
	char msgs[2048] = {};

	unsigned i = 0u, j = 0u, c = 0u;
	while (infoMsgs && infoMsgs[i])
	{
		while (infoMsgs[i][j] && c < 2047)
			msgs[c++] = infoMsgs[i][j];

		while (infoMsgs[++i] && c < 2047)
			msgs[c++] = '\n';
	}
	msgs[c] = '\0';

	char description[512] = {};

	// Try Win32 message for HRESULT_FROM_WIN32 codes
	DWORD err = HRESULT_CODE(hr);
	DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS;

	char* msg = nullptr;

	// First: treat as Win32 error
	if (HRESULT_FACILITY(hr) == FACILITY_WIN32)
	{
		if (FormatMessageA(flags, nullptr, err, 0, (LPSTR)&msg, 0, nullptr) && msg)
		{
			snprintf(description, 512, "%s", msg);
			LocalFree(msg);
			goto description_done;
		}
		msg = nullptr;
	}
	// Second: try directly on the HRESULT (sometimes works)
	if (FormatMessageA(flags, nullptr, (DWORD)hr, 0, (LPSTR)&msg, 0, nullptr) && msg)
	{
		snprintf(description, 512, "%s", msg);
		LocalFree(msg);
		goto description_done;
	}
	// Fallback
	snprintf(description, 512, "Unknown error (0x%8X)", (unsigned)hr);
description_done:

	char e_string[64] = {};
	snprintf(e_string, 64, "0x%8X", (unsigned)hr);

	if (msgs[0])
		snprintf(info, 2048,
			"[Error Code]\n%lu\n "
			"[Error String]\n%s\n"
			"[Description]\n%s\n"
			"[Error Info]\n%s\n"
			"%s"
			, hr, e_string, description, msgs, GetOriginString());

	else
		snprintf(info, 2048,
			"[Error Code]\n%lu\n "
			"[Error String]\n%s\n"
			"[Description]\n%s\n"
			"%s"
			, hr, e_string, description, GetOriginString());
}
