#include "QuaternionMotion.h"
#include "Exception/Exception.h"

int __stdcall WinMain()
{
	try 
	{
		return QuaternionMotion().Run();
	}
	catch (const Exception& exc) { exc.PopMessageBox(); }

	return -1;
}