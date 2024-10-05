#pragma once
#include "iGManager.h"

class IG_Mandelbrot : public iGManager
{
public:
	static void render();

private:
	static enum QY
	{
		QUALITY_CURRENT,
		QUALITY_720x480,
		QUALITY_1280x720,
		QUALITY_1920x1080,
		QUALITY_3840x2160,
		QUALITY_7680x4320,
		QUALITY_CUSTOM,
	} QUALITY;

	static int customX;
	static int customY;

	static void saveMenu();
};
