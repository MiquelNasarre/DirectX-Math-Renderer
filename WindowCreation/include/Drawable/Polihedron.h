#pragma once
#include "Drawable.h"

/* POLIHEDRON DRAWABLE CLASS
-------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------
Drawable class to draw triangle meshes, it is a must for any rendering library and in
this case here we have it. To initialize it, it expects a valid pointer to a descriptor
and will create the Shape with the specified data.

As it is standard on this library it has multiple setting to set the object rotation,
position, and screen shifting and it is displayed in relation to the perspective of the
Graphics currently set as render target.

It allows for ilumination, texturing, transparencies and figure updates. For information 
on how to hadle transparencies you can check the Graphics header. For information on how 
to create images for the texture you can check the Image class header.
-------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------
*/

// Polihedron descriptor struct, to be created and passed as a pointer to initialize
// a polihedron, it allows for different coloring and rendering settings. The 
// pointers memory is to be managed by the user, the creation function will not 
// modify or store any of the original pointers in the descriptor.
struct POLIHEDRON_DESC
{
	// Expects a valid pointer to a list of vertices. The list must be 
	// as long as the highest index found in the triangle list.
	Vector3f* vertex_list = nullptr;

	// Expects a valid pointer to a list of oriented triangles in integer
	// format that will be interpreted as a Vector3i pointer. Must be as
	// long as three intergers times the triangle count.
	Vector3i* triangle_list = nullptr;

	// Number of triangles that form the Polihedron.
	unsigned triangle_count = 0u;

	// Specifies how the coloring will be done for the Polihedron.
	enum POLIHEDRON_COLORING
	{
		TEXTURED_COLORING,
		PER_VERTEX_COLORING,
		GLOBAL_COLORING
	} 
	coloring = GLOBAL_COLORING; // Defaults to global color

	// If coloring is set to global the shape will have this color.
	Color global_color = Color::White;

	// If coloring is set to per vertex it expects a valid pointer 
	// to a list of colors containing one color per every vertex
	// of every triangle. Three times the triangle count.
	Color* color_list = nullptr;

	// If coloring is set to textured it expects a valid pointer to 
	// an image containing the texture to be used by the Polihedron.
	Image* texture_image = nullptr;

	// If coloring is set to textured it expects a valid pointer to 
	// a list of pixel coordinates containing one coordinate per every
	// vertex of every triangle. Three times the traiangle count.
	Vector2i* texture_coordinates_list = nullptr;

	// Whether both sides of each triangle are rendered or not.
	bool double_sided_rendering = true;

	// Whether the polihedron uses ilumination or not.
	bool enable_iluminated = true;

	// Sets Order Indepentdent Transparency for the Polihedrom. Check 
	// Graphics.h or Blender.h for more information on how to use it.
	bool enable_transparency = false;

	// Whether the polihedron allows for shape updates, leave at false if 
	// you don't intend to update the shape of the Polihedron. Only the functions 
	// updateVertices(), updateColors(), undateTextureCoordinates() require it.
	bool enable_updates	= false;

	// IF true renders only the aristas of the Polihedron.
	bool wire_frame_topology = false;
};

class Polihedron : public Drawable
{
public:
	// Polihedron constructor, if the pointer is valid it will call the initializer.
	Polihedron(const POLIHEDRON_DESC* pDesc = nullptr);

	// Frees the GPU pointers and all the stored data.
	~Polihedron();

	// Initializes the Polihedron object, it expects a valid pointer to a descriptor
	// and will initialize everything as specified, can only be called once per object.
	void initialize(const POLIHEDRON_DESC* pDesc);

	// If updates are enabled this function allows to change the current vertex positions
	// for the new ones specified. It expects a valid pointer with a list as long as the 
	// highest index found in the triangle list used for initialization.
	void updateVertices(const Vector3f* vertex_list);

	// If updates are enabled, and coloring is per vertex, this function allows to change 
	// the current vertex colors for the new ones specified. It expects a valid pointer 
	// with a list of colors containing one color per every vertex of every triangle. 
	// Three times the triangle count.
	void updateColors(const Color* color_list);

	// If updates are enabled, and coloring is texured, this functions allows o change the 
	// current vertex texture coordinates for the new ones specified. It expects a valid 
	// pointer with a list of pixels containing one coordinates per every vertex of every
	// triangle. Three times the triangle count.
	void undateTextureCoordinates(const Vector2i* texture_coordinates_list);

	// If the coloring is set to global, updates the global Polihedron color.
	void updateGlobalColor(Color color);

	// Updates the rotation quaternion of the Polihedron. If multiplicative it will apply
	// the rotation on top of the current rotation. For more information on how to rotate
	// with quaternions check the Quaternion header file.
	void updateRotation(Quaternion rotation, bool multiplicative = false);

	// Updates the scene position of the Polihedrom. I additive it will add the vector
	// to the current position vector of the Polihedron.
	void updatePosition(Vector3f position, bool additive = false);

	// Updates the screen displacement of the figure. To be used if you intend to render 
	// multiple scenes/plots on the same render target.
	void updateScreenPosition(Vector2f screenDisplacement);

	// If ilumination is enabled it sets the specified light to the specified parameters.
	// Eight lights are allowed. And the intensities are directional and diffused.
	void updateLight(unsigned id, Vector2f intensities, Color color, Vector3f position);

	// If ilumination is enabled clears all lights for the Polihedron.
	void clearLights();

	// Returns the current rotation quaternion.
	Quaternion getRotation() const;

	// Returns the current scene position.
	Vector3f getPosition() const;

	// Returns the current screen position.
	Vector2f getScreenPosition() const;
	
private:
	// Pointer to the internal class storage.
	void* polihedronData = nullptr;
};