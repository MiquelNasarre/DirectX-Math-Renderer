#pragma once
#include "Color.h"

/* IMAGE CLASS HEADER
-------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------
This image class allows for easy image manipulation from your code. To create an image
you can create one yourself by specifying the dimensions and the color of each pixel 
or you can load them from your computer from raw bitmap files using the load() function.

To manipulate the image you can access the pixels directly via tha operator(row,col), or 
via the pixels() function, the Color class is a BGRA color format class that stores the 
channels as unsinged chars (bytes), for more information about the color class please check 
its header file.

Then after some image manipulation, using the save() function you can store them back into 
your computer to your specified path, or in the 3D rendering library you can send it to the 
GPU as textures for your renderings.

To obtain raw bitmap files from your images I strongly suggest the use of ImageMagick, 
a simple console command like: "> magick initial_image.*** -compress none image.bmp"
will give you a raw bitmap of any image.

For information on how to install and use ImageMagick you can check https://imagemagick.org/
-------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------
*/

// Simple class for handling images, and storing and loading them as BMP files.
class Image
{
private:
	// Private variables

	Color* pixels_ = nullptr;	// Pointer to the image pixels as a color array

	unsigned int width_	 = 0u;	// Stores the width of the image
	unsigned int height_ = 0u;	// Stores the height of the image

public:
	// Constructors/Destructors

	// Initializes the image as stored in the bitmap file.
	Image(const char* fmt_filename, ...);

	// Copies the other image.
	Image(const Image& other);

	// Copies the other image.
	Image& operator=(const Image& other);

	// Stores a copy of the color pointer.
	Image(Color* pixels, unsigned int width, unsigned int height);

	// Creates an image with the specified size and color.
	Image(unsigned int width, unsigned int height, Color color = Color::Transparent);

	// Frees the pixel pointer.
	~Image();

	// Getters

	// Returns the pointer to the image pixels as a color array.
	Color* pixels();

	// Returns the image width.
	unsigned width() const;

	// Returns the image height.
	unsigned height() const;

	// Accessors

	// Returns a color reference to the specified pixel coordinates.
	Color& operator()(unsigned int row, unsigned int col);

	// Returns a constant color reference to the specified pixel coordinates.
	const Color& operator()(unsigned int row, unsigned int col) const;

	// File functions

	// Loads an image from the specified file path.
	bool load(const char* fmt_filename, ...);

	// Saves the image to the specified file path.
	bool save(const char* fmt_filename, ...) const;
};