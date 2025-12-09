#pragma once
#include "iGManager.h"

class IG_Tester : public iGManager
{
public:
	IG_Tester(Window& _w) : iGManager(_w) {}

	void render();
};
