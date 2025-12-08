#include "$safeprojectname$.h"
#include "Exception/Exception.h"

int __stdcall WinMain()
{
	try { return $safeprojectname$().Run(); }

	catch (const Exception& exc) { exc.PopMessageBox(); }
	return -1;
}