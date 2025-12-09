#include "Tester.h"
#include "Exception/Exception.h"

int __stdcall WinMain()
{
	try {

		return Tester().Run();

	}
	catch (const Exception& exc) { exc.PopMessageBox(); }

	return -1;
}