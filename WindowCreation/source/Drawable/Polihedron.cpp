#include "Drawable/Polihedron.h"
#include "Bindable/BindableBase.h"

#include "Exception/_exDefault.h"

/*
-----------------------------------------------------------------------------------------------------------
 Polihedron Internals
-----------------------------------------------------------------------------------------------------------
*/

// Struct that stores the internal data for a given Polihedron object.
struct PolihedronInternals
{
	struct Vertex
	{
		_float4vector vector;
		_float4vector norm;
	}*Vertices = nullptr;

	struct ColorVertex
	{
		_float4vector vector;
		_float4vector norm;
		_float4color color;
	}*ColVertices = nullptr;

	struct TextureVertex
	{
		_float4vector vector;
		_float4vector norm;
		_float4vector coord;
	}*TexVertices = nullptr;

	unsigned image_width = 0u;
	unsigned image_height = 0u;

	struct VSconstBuffer
	{
		Quaternion rotation			= 1.f;
		_float4vector position		= { 0.f, 0.f, 0.f, 0.f };
		_float4vector displacement	= { 0.f, 0.f, 0.f, 0.f };
	}vscBuff;

	struct PSconstBuffer
	{
		struct {
			_float4vector intensity = { 0.f,0.f,0.f,0.f };
			_float4color  color		= { 0.f,0.f,0.f,0.f };
			_float4vector position	= { 0.f,0.f,0.f,0.f };
		}lightsource[8];
	}
	pscBuff = // Default light settings
	{
			60.f,10.f,NULL,NULL,1.0f, 0.2f, 0.2f, 1.f , 0.f, 8.f, 8.f,NULL,
			60.f,10.f,NULL,NULL,0.0f, 1.0f, 0.0f, 1.f , 0.f,-8.f, 8.f,NULL,
			60.f,10.f,NULL,NULL,0.5f, 0.0f, 1.0f, 1.f ,-8.f, 0.f,-8.f,NULL,
			60.f,10.f,NULL,NULL,1.0f, 1.0f, 0.0f, 1.f , 8.f, 0.f, 8.f,NULL,
	};

	ConstantBuffer* pVSCB = nullptr;
	ConstantBuffer* pPSCB = nullptr;

	ConstantBuffer* pGlobalColorCB = nullptr;

	VertexBuffer* pUpdateVB = nullptr;

	POLIHEDRON_DESC desc = {};
};

/*
-----------------------------------------------------------------------------------------------------------
 Constructors / Destructors
-----------------------------------------------------------------------------------------------------------
*/

// Polihedron constructor, if the pointer is valid it will call the initializer.

Polihedron::Polihedron(const POLIHEDRON_DESC* pDesc)
{
	if (pDesc)
		initialize(pDesc);
}

// Frees the GPU pointers and all the stored data.

Polihedron::~Polihedron()
{
	if (!isInit)
		return;

	PolihedronInternals& data = *(PolihedronInternals*)polihedronData;

	if (data.Vertices)
		delete[] data.Vertices;

	if (data.ColVertices)
		delete[] data.ColVertices;

	if (data.TexVertices)
		delete[] data.TexVertices;

	if (data.desc.triangle_list)
		delete[] data.desc.triangle_list;

	delete &data;
}

// Initializes the Polihedron object, it expects a valid pointer to a descriptor
// and will initialize everything as specified, can only be called once per object.

void Polihedron::initialize(const POLIHEDRON_DESC* pDesc)
{
	if (!pDesc)
		throw INFO_EXCEPT("Trying to initialize a Polihedron with an invalid descriptor pointer.");

	if (isInit)
		throw INFO_EXCEPT("Trying to initialize a Polihedron that has already been initialized.");
	else
		isInit = true;

	polihedronData = new PolihedronInternals;
	PolihedronInternals& data = *(PolihedronInternals*)polihedronData;

	data.desc = *pDesc;

	if (!data.desc.vertex_list)
		throw INFO_EXCEPT("Found nullptr when trying to access a vertex list to create a Polihedron.");

	if (!data.desc.triangle_list)
		throw INFO_EXCEPT("Found nullptr when trying to access a triangle list to create a Polihedron.");

	switch (data.desc.coloring)
	{
		case POLIHEDRON_DESC::GLOBAL_COLORING:
		{
			data.Vertices = new PolihedronInternals::Vertex[3 * data.desc.triangle_count];

			for (unsigned i = 0u; i < data.desc.triangle_count; i++)
			{
				const Vector3f& v0 = data.desc.vertex_list[data.desc.triangle_list[i].x];
				const Vector3f& v1 = data.desc.vertex_list[data.desc.triangle_list[i].y];
				const Vector3f& v2 = data.desc.vertex_list[data.desc.triangle_list[i].z];

				Vector3f norm = data.desc.enable_iluminated ? ((v1 - v0) * (v2 - v0)).normalize() : Vector3f();

				data.Vertices[3 * i + 0].vector = v0.getVector4();
				data.Vertices[3 * i + 1].vector = v1.getVector4();
				data.Vertices[3 * i + 2].vector = v2.getVector4();

				data.Vertices[3 * i + 0].norm = norm.getVector4();
				data.Vertices[3 * i + 1].norm = norm.getVector4();
				data.Vertices[3 * i + 2].norm = norm.getVector4();
			}
			data.pUpdateVB = AddBind(new VertexBuffer(data.Vertices, 3u * data.desc.triangle_count, data.desc.enable_updates ? VB_USAGE_DYNAMIC : VB_USAGE_DEFAULT));

			// If updates disabled delete the vertexs
			if (!data.desc.enable_updates)
			{
				delete[] data.Vertices;
				data.Vertices = nullptr;
			}
			// Create the corresponding Vertex Shader
			VertexShader* pvs = AddBind(new VertexShader(SHADERS_DIR L"GlobalColorVS.cso"));
			// Create the corresponding Pixel Shader and Blender
			if (data.desc.enable_transparency)
			{
				AddBind(new PixelShader(data.desc.enable_iluminated ? SHADERS_DIR L"OITGlobalColorPS.cso" : SHADERS_DIR L"OITUnlitGlobalColorPS.cso"));
				AddBind(new Blender(BLEND_MODE_OIT_WEIGHTED));
			}
			else
			{
				AddBind(new PixelShader(data.desc.enable_iluminated ? SHADERS_DIR L"GlobalColorPS.cso" : SHADERS_DIR L"UnlitGlobalColorPS.cso"));
				AddBind(new Blender(BLEND_MODE_OPAQUE));
			}
			// Create the corresponding input layout
			INPUT_ELEMENT_DESC ied[2] =
			{
				{ "Position",	_4_FLOAT },
				{ "Normal",		_4_FLOAT },
			};
			AddBind(new InputLayout(ied, 2u, pvs));

			// Create the constant buffer for the global color.
			_float4color col = data.desc.global_color.getColor4();
			data.pGlobalColorCB = AddBind(new ConstantBuffer(&col, PIXEL_CONSTANT_BUFFER, 1u /*Slot*/));
			break;
		}

		case POLIHEDRON_DESC::PER_VERTEX_COLORING:
		{
			if (!data.desc.color_list)
				throw INFO_EXCEPT("Found nullptr when trying to access a color list to create a vertex colored Polihedron.");

			data.ColVertices = new PolihedronInternals::ColorVertex[3 * data.desc.triangle_count];

			for (unsigned i = 0u; i < data.desc.triangle_count; i++)
			{
				const Vector3f& v0 = data.desc.vertex_list[data.desc.triangle_list[i].x];
				const Vector3f& v1 = data.desc.vertex_list[data.desc.triangle_list[i].y];
				const Vector3f& v2 = data.desc.vertex_list[data.desc.triangle_list[i].z];

				Vector3f norm = data.desc.enable_iluminated ? ((v1 - v0) * (v2 - v0)).normalize() : Vector3f();

				data.ColVertices[3 * i + 0].vector = v0.getVector4();
				data.ColVertices[3 * i + 1].vector = v1.getVector4();
				data.ColVertices[3 * i + 2].vector = v2.getVector4();

				data.ColVertices[3 * i + 0].norm = norm.getVector4();
				data.ColVertices[3 * i + 1].norm = norm.getVector4();
				data.ColVertices[3 * i + 2].norm = norm.getVector4();

				data.ColVertices[3 * i + 0].color = data.desc.color_list[3 * i + 0].getColor4();
				data.ColVertices[3 * i + 1].color = data.desc.color_list[3 * i + 1].getColor4();
				data.ColVertices[3 * i + 2].color = data.desc.color_list[3 * i + 2].getColor4();
			}
			data.pUpdateVB = AddBind(new VertexBuffer(data.ColVertices, 3u * data.desc.triangle_count, data.desc.enable_updates ? VB_USAGE_DYNAMIC : VB_USAGE_DEFAULT));

			// If updates disabled delete the vertexs
			if (!data.desc.enable_updates)
			{
				delete[] data.ColVertices;
				data.ColVertices = nullptr;
			}
			// Create the corresponding Vertex Shader
			VertexShader* pvs = AddBind(new VertexShader(SHADERS_DIR L"VertexColorVS.cso"));
			// Create the corresponding Pixel Shader and Blender
			if (data.desc.enable_transparency)
			{
				AddBind(new PixelShader(data.desc.enable_iluminated ? SHADERS_DIR L"OITVertexColorPS.cso" : SHADERS_DIR L"OITUnlitVertexColorPS.cso"));
				AddBind(new Blender(BLEND_MODE_OIT_WEIGHTED));
			}
			else
			{
				AddBind(new PixelShader(data.desc.enable_iluminated ? SHADERS_DIR L"VertexColorPS.cso" : SHADERS_DIR L"UnlitVertexColorPS.cso"));
				AddBind(new Blender(BLEND_MODE_OPAQUE));
			}
			// Create the corresponding input layout
			INPUT_ELEMENT_DESC ied[3] =
			{
				{ "Position",	_4_FLOAT },
				{ "Normal",		_4_FLOAT },
				{ "Color",		_4_FLOAT },
			};
			AddBind(new InputLayout(ied, 3u, pvs));
			break;
		}

		case POLIHEDRON_DESC::TEXTURED_COLORING:
		{
			if (!data.desc.texture_image)
				throw INFO_EXCEPT("Found nullptr when trying to access an Image to create a textured Polihedron.");

			if (!data.desc.texture_coordinates_list)
				throw INFO_EXCEPT("Found nullptr when trying to access a texture coordinate list to create a textured Polihedron.");

			data.TexVertices = new PolihedronInternals::TextureVertex[3 * data.desc.triangle_count];

			data.image_height = data.desc.texture_image->height();
			data.image_width = data.desc.texture_image->width();

			for (unsigned i = 0u; i < data.desc.triangle_count; i++)
			{
				const Vector3f& v0 = data.desc.vertex_list[data.desc.triangle_list[i].x];
				const Vector3f& v1 = data.desc.vertex_list[data.desc.triangle_list[i].y];
				const Vector3f& v2 = data.desc.vertex_list[data.desc.triangle_list[i].z];

				Vector3f norm = data.desc.enable_iluminated ? ((v1 - v0) * (v2 - v0)).normalize() : Vector3f();

				data.TexVertices[3 * i + 0].vector = v0.getVector4();
				data.TexVertices[3 * i + 1].vector = v1.getVector4();
				data.TexVertices[3 * i + 2].vector = v2.getVector4();

				data.TexVertices[3 * i + 0].norm = norm.getVector4();
				data.TexVertices[3 * i + 1].norm = norm.getVector4();
				data.TexVertices[3 * i + 2].norm = norm.getVector4();

				data.TexVertices[3 * i + 0].coord = {
					float(data.desc.texture_coordinates_list[3 * i + 0].x) / data.image_width,
					float(data.desc.texture_coordinates_list[3 * i + 0].y) / data.image_height,
				0.f,0.f };

				data.TexVertices[3 * i + 1].coord = {
					float(data.desc.texture_coordinates_list[3 * i + 1].x) / data.image_width,
					float(data.desc.texture_coordinates_list[3 * i + 1].y) / data.image_height,
				0.f,0.f };

				data.TexVertices[3 * i + 2].coord = {
					float(data.desc.texture_coordinates_list[3 * i + 2].x) / data.image_width,
					float(data.desc.texture_coordinates_list[3 * i + 2].y) / data.image_height,
				0.f,0.f };
			}
			data.pUpdateVB = AddBind(new VertexBuffer(data.TexVertices, 3u * data.desc.triangle_count, data.desc.enable_updates ? VB_USAGE_DYNAMIC : VB_USAGE_DEFAULT));

			// If updates disabled delete the vertexs
			if (!data.desc.enable_updates)
			{
				delete[] data.TexVertices;
				data.TexVertices = nullptr;
			}
			// Create the corresponding Vertex Shader
			VertexShader* pvs = AddBind(new VertexShader(SHADERS_DIR L"VertexTextureVS.cso"));
			// Create the corresponding Pixel Shader and Blender
			if (data.desc.enable_transparency)
			{
				AddBind(new PixelShader(data.desc.enable_iluminated ? SHADERS_DIR L"OITVertexTexturePS.cso" : SHADERS_DIR L"OITUnlitVertexTexturePS.cso"));
				AddBind(new Blender(BLEND_MODE_OIT_WEIGHTED));
			}
			else
			{
				AddBind(new PixelShader(data.desc.enable_iluminated ? SHADERS_DIR L"VertexTexturePS.cso" : SHADERS_DIR L"UnlitVertexTexturePS.cso"));
				AddBind(new Blender(BLEND_MODE_OPAQUE));
			}
			// Create the corresponding input layout
			INPUT_ELEMENT_DESC ied[3] =
			{
				{ "Position",	_4_FLOAT },
				{ "Normal",		_4_FLOAT },
				{ "TexCoor",	_4_FLOAT },
			};
			AddBind(new InputLayout(ied, 3u, pvs));

			// Create the texture from the image
			AddBind(new Texture(data.desc.texture_image));

			// Set the sampler as linear for the texture
			AddBind(new Sampler(data.desc.pixelated_texture ? SAMPLE_FILTER_POINT : SAMPLE_FILTER_LINEAR, SAMPLE_ADDRESS_WRAP));
			break;
		}

		default:
			throw INFO_EXCEPT("Found an unrecognized coloring mode when trying to create a Polihedron.");
	}

	// If update enabled save a copy to update vertices
	if (data.desc.enable_updates)
		data.desc.triangle_list = (Vector3i*)memcpy(new Vector3i[data.desc.triangle_count], data.desc.triangle_list, data.desc.triangle_count * sizeof(Vector3i));
	// Else forget about it.
	else
		data.desc.triangle_list = nullptr;

	unsigned* indexs = new unsigned[3u * data.desc.triangle_count];
	for (unsigned i = 0; i < 3u * data.desc.triangle_count; i++)
		indexs[i] = i;

	AddBind(new IndexBuffer(indexs, 3u * data.desc.triangle_count));
	delete[] indexs;

	AddBind(new Topology(TRIANGLE_LIST));
	AddBind(new Rasterizer(data.desc.double_sided_rendering, data.desc.wire_frame_topology));

	data.pVSCB = AddBind(new ConstantBuffer(&data.vscBuff, VERTEX_CONSTANT_BUFFER));

	// If iluminated is enabled bind the default lights
	if (data.desc.enable_iluminated)
		data.pPSCB = AddBind(new ConstantBuffer(&data.pscBuff, PIXEL_CONSTANT_BUFFER));
}

/*
-----------------------------------------------------------------------------------------------------------
 User Functions
-----------------------------------------------------------------------------------------------------------
*/

// If updates are enabled this function allows to change the current vertex positions
// for the new ones specified. It expects a valid pointer with a list as long as the 
// highest index found in the triangle list used for initialization.

void Polihedron::updateVertices(const Vector3f* vertex_list)
{
	if (!isInit)
		throw INFO_EXCEPT("Trying to update the vertices on an uninitialized Polihedron");

	if (!vertex_list)
		throw INFO_EXCEPT("Trying to update the vertices with an invalid vertex list");

	PolihedronInternals& data = *(PolihedronInternals*)polihedronData;

	if (!data.desc.enable_updates)
		throw INFO_EXCEPT("Trying to update the vertices on a Polihedron with updates disabled");

	switch (data.desc.coloring)
	{
		case POLIHEDRON_DESC::GLOBAL_COLORING:
		{
			for (unsigned i = 0u; i < data.desc.triangle_count; i++)
			{
				const Vector3f& v0 = vertex_list[data.desc.triangle_list[i].x];
				const Vector3f& v1 = vertex_list[data.desc.triangle_list[i].y];
				const Vector3f& v2 = vertex_list[data.desc.triangle_list[i].z];

				Vector3f norm = data.desc.enable_iluminated ? ((v1 - v0) * (v2 - v0)).normalize() : Vector3f();

				data.Vertices[3 * i + 0].vector = v0.getVector4();
				data.Vertices[3 * i + 1].vector = v1.getVector4();
				data.Vertices[3 * i + 2].vector = v2.getVector4();

				data.Vertices[3 * i + 0].norm = norm.getVector4();
				data.Vertices[3 * i + 1].norm = norm.getVector4();
				data.Vertices[3 * i + 2].norm = norm.getVector4();
			}
			data.pUpdateVB->updateVertices(data.Vertices, 3u * data.desc.triangle_count);
			break;
		}

		case POLIHEDRON_DESC::PER_VERTEX_COLORING:
		{
			for (unsigned i = 0u; i < data.desc.triangle_count; i++)
			{
				const Vector3f& v0 = vertex_list[data.desc.triangle_list[i].x];
				const Vector3f& v1 = vertex_list[data.desc.triangle_list[i].y];
				const Vector3f& v2 = vertex_list[data.desc.triangle_list[i].z];

				Vector3f norm = data.desc.enable_iluminated ? ((v1 - v0) * (v2 - v0)).normalize() : Vector3f();

				data.ColVertices[3 * i + 0].vector = v0.getVector4();
				data.ColVertices[3 * i + 1].vector = v1.getVector4();
				data.ColVertices[3 * i + 2].vector = v2.getVector4();

				data.ColVertices[3 * i + 0].norm = norm.getVector4();
				data.ColVertices[3 * i + 1].norm = norm.getVector4();
				data.ColVertices[3 * i + 2].norm = norm.getVector4();
			}
			data.pUpdateVB->updateVertices(data.ColVertices, 3u * data.desc.triangle_count);
			break;
		}

		case POLIHEDRON_DESC::TEXTURED_COLORING:
		{
			for (unsigned i = 0u; i < data.desc.triangle_count; i++)
			{
				const Vector3f& v0 = vertex_list[data.desc.triangle_list[i].x];
				const Vector3f& v1 = vertex_list[data.desc.triangle_list[i].y];
				const Vector3f& v2 = vertex_list[data.desc.triangle_list[i].z];

				Vector3f norm = data.desc.enable_iluminated ? ((v1 - v0) * (v2 - v0)).normalize() : Vector3f();

				data.TexVertices[3 * i + 0].vector = v0.getVector4();
				data.TexVertices[3 * i + 1].vector = v1.getVector4();
				data.TexVertices[3 * i + 2].vector = v2.getVector4();

				data.TexVertices[3 * i + 0].norm = norm.getVector4();
				data.TexVertices[3 * i + 1].norm = norm.getVector4();
				data.TexVertices[3 * i + 2].norm = norm.getVector4();
			}
			data.pUpdateVB->updateVertices(data.TexVertices, 3u * data.desc.triangle_count);
			break;
		}
	}
}

// If updates are enabled, and coloring is per vertex, this function allows to change 
// the current vertex colors for the new ones specified. It expects a valid pointer 
// with a list of colors containing one color per every vertex of every triangle. 
// Three times the triangle count.

void Polihedron::updateColors(const Color* color_list)
{
	if (!isInit)
		throw INFO_EXCEPT("Trying to update the colors on an uninitialized Polihedron");

	if (!color_list)
		throw INFO_EXCEPT("Trying to update the colors with an invalid color list");

	PolihedronInternals& data = *(PolihedronInternals*)polihedronData;

	if (data.desc.coloring != POLIHEDRON_DESC::PER_VERTEX_COLORING)
		throw INFO_EXCEPT("Trying to update the colors on a Polihedron with a different coloring");

	if (!data.desc.enable_updates)
		throw INFO_EXCEPT("Trying to update the colors on a Polihedron with updates disabled");

	for (unsigned i = 0u; i < 3u * data.desc.triangle_count; i++)
		data.ColVertices[i].color = color_list[i].getColor4();

	data.pUpdateVB->updateVertices(data.ColVertices, 3u * data.desc.triangle_count);
}

// If updates are enabled, and coloring is texured, this functions allows o change the 
// current vertex texture coordinates for the new ones specified. It expects a valid 
// pointer with a list of pixels containing one coordinates per every vertex of every
// triangle. Three times the triangle count.

void Polihedron::updateTextureCoordinates(const Vector2i* texture_coordinates_list)
{
	if (!isInit)
		throw INFO_EXCEPT("Trying to update the texture coordinates on an uninitialized Polihedron");

	if (!texture_coordinates_list)
		throw INFO_EXCEPT("Trying to update the texture coordinates with an invalid texture coordinate list");

	PolihedronInternals& data = *(PolihedronInternals*)polihedronData;

	if (data.desc.coloring != POLIHEDRON_DESC::TEXTURED_COLORING)
		throw INFO_EXCEPT("Trying to update the texture coordinates on a Polihedron with a different coloring");

	if (!data.desc.enable_updates)
		throw INFO_EXCEPT("Trying to update the texture coordinates on a Polihedron with updates disabled");

	for (unsigned i = 0u; i < 3u * data.desc.triangle_count; i++)
		data.TexVertices[i].coord = {
			float(texture_coordinates_list[i].x) / data.image_width,
			float(texture_coordinates_list[i].y) / data.image_height,
			0.f, 0.f };

	data.pUpdateVB->updateVertices(data.TexVertices, 3u * data.desc.triangle_count);
}

// If the coloring is set to global, updates the global Polihedron color.

void Polihedron::updateGlobalColor(Color color)
{
	if (!isInit)
		throw INFO_EXCEPT("Trying to update the global color on an uninitialized Polihedron");

	PolihedronInternals& data = *(PolihedronInternals*)polihedronData;

	if (data.desc.coloring != POLIHEDRON_DESC::GLOBAL_COLORING)
		throw INFO_EXCEPT("Trying to update the global color on a Polihedron with a different coloring");

	_float4color col = color.getColor4();
	data.pGlobalColorCB->update(&col);
}

// Updates the rotation quaternion of the Polihedron. If multiplicative it will apply
// the rotation on top of the current rotation. For more information on how to rotate
// with quaternions check the Quaternion header file.

void Polihedron::updateRotation(Quaternion rotation, bool multiplicative)
{
	if (!isInit)
		throw INFO_EXCEPT("Trying to update the rotation on an uninitialized Polihedron");

	if (!rotation)
		throw INFO_EXCEPT(
			"Invalid quaternion found when trying to update rotation on a Polihedron.\n"
			"Quaternion 0 can not be normalized and therefore can not describe an objects rotation."
		);

	PolihedronInternals& data = *(PolihedronInternals*)polihedronData;

	if (multiplicative)
		data.vscBuff.rotation *= rotation;
	else
		data.vscBuff.rotation = rotation;

	data.vscBuff.rotation.normalize();
	data.pVSCB->update(&data.vscBuff);
}

// Updates the scene position of the Polihedrom. I additive it will add the vector
// to the current position vector of the Polihedron.

void Polihedron::updatePosition(Vector3f position, bool additive)
{
	if (!isInit)
		throw INFO_EXCEPT("Trying to update the position on an uninitialized Polihedron");

	PolihedronInternals& data = *(PolihedronInternals*)polihedronData;

	if (additive)
		position += Vector3f(data.vscBuff.position.x, data.vscBuff.position.y, data.vscBuff.position.z);

	data.vscBuff.position = position.getVector4();
	data.pVSCB->update(&data.vscBuff);
}

// Updates the screen displacement of the figure. To be used if you intend to render 
// multiple scenes/plots on the same render target.

void Polihedron::updateScreenPosition(Vector2f screenDisplacement)
{
	if (!isInit)
		throw INFO_EXCEPT("Trying to update the screen position on an uninitialized Polihedron");

	PolihedronInternals& data = *(PolihedronInternals*)polihedronData;

	data.vscBuff.displacement = screenDisplacement.getVector4();
	data.pVSCB->update(&data.vscBuff);
}

// If ilumination is enabled it sets the specified light to the specified parameters.
// Eight lights are allowed. And the intensities are directional and diffused.

void Polihedron::updateLight(unsigned id, Vector2f intensities, Color color, Vector3f position)
{
	if (!isInit)
		throw INFO_EXCEPT("Trying to update a light on an uninitialized Polihedron");

	PolihedronInternals& data = *(PolihedronInternals*)polihedronData;

	if (!data.desc.enable_iluminated)
		throw INFO_EXCEPT("Trying to update a light on a Polihedron with ilumination disabled");

	if (id >= 8)
		throw INFO_EXCEPT("Trying to update a light with an invalid id (must be 0-7)");

	data.pscBuff.lightsource[id] = { intensities.getVector4(), color.getColor4(), position.getVector4() };
	data.pPSCB->update(&data.pscBuff);
}

// If ilumination is enabled clears all lights for the Polihedron.

void Polihedron::clearLights()
{
	if (!isInit)
		throw INFO_EXCEPT("Trying to clear the lights on an uninitialized Polihedron");

	PolihedronInternals& data = *(PolihedronInternals*)polihedronData;

	if (!data.desc.enable_iluminated)
		throw INFO_EXCEPT("Trying to clear the lights on a Polihedron with ilumination disabled");

	for (auto& light : data.pscBuff.lightsource)
		light = {};

	data.pPSCB->update(&data.pscBuff);
}

/*
-----------------------------------------------------------------------------------------------------------
 Getters
-----------------------------------------------------------------------------------------------------------
*/

// Returns the current rotation quaternion.

Quaternion Polihedron::getRotation() const
{
	if (!isInit)
		throw INFO_EXCEPT("Trying to get the rotation on an uninitialized Polihedron");

	PolihedronInternals& data = *(PolihedronInternals*)polihedronData;

	return data.vscBuff.rotation;
}

// Returns the current scene position.

Vector3f Polihedron::getPosition() const
{
	if (!isInit)
		throw INFO_EXCEPT("Trying to get the position on an uninitialized Polihedron");

	PolihedronInternals& data = *(PolihedronInternals*)polihedronData;

	return { data.vscBuff.position.x, data.vscBuff.position.y, data.vscBuff.position.z };
}

// Returns the current screen position.

Vector2f Polihedron::getScreenPosition() const
{
	if (!isInit)
		throw INFO_EXCEPT("Trying to get the screen position of an uninitialized Polihedron");

	PolihedronInternals& data = *(PolihedronInternals*)polihedronData;

	return { data.vscBuff.displacement.x, data.vscBuff.displacement.y };
}
