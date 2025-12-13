#pragma once
#include "Drawable.h"

struct BACKGROUND_DESC
{
	Image* image = nullptr;

	bool texture_updates = false;
	bool make_dynamic = false;
	bool pixelated_texture = false;

	enum PROJECTION_TYPES 
	{
		PT_MERCATOR,
		PT_AZIMUTHAL
	} projection_type = PT_MERCATOR;
};

class Background : public Drawable
{
public:
	Background(const BACKGROUND_DESC* pDesc = nullptr);
	~Background();

	void initialize(const BACKGROUND_DESC* pDesc);

	void updateTexture(Image* image);
	void updateObserver(Quaternion observer);
	void updateWideness(float FOV, Vector2f WindowDimensions);
	void updateRectangle(Vector2i min_coords, Vector2i max_coords);

	Vector2i getImageDim() const;

private:
	void* backgroundData = nullptr;
};