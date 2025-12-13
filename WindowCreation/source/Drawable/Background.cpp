#include "Drawable/Background.h"
#include "Bindable/BindableBase.h"

#include "Exception/_exGraphics.h"

struct BackgroundInternals
{
	struct PSconstBuffer 
	{
		Quaternion obs = 1.f;
		_float4vector ei = {};
		_float4vector zp = {};
	}projection = {};

	Vector2i imageDim = {};

	Texture* textureUpdates = nullptr;

	ConstantBuffer* pscBuff0 = nullptr;
	ConstantBuffer* pscBuff1 = nullptr;
	ConstantBuffer* vscBuff = nullptr;

	BACKGROUND_DESC desc = {};
};


Background::Background(const BACKGROUND_DESC* pDesc)
{
	if (pDesc)
		initialize(pDesc);
}

Background::~Background()
{
	if (!isInit)
		return;
	
	BackgroundInternals& data = *(BackgroundInternals*)backgroundData;

	delete &data;
}

void Background::initialize(const BACKGROUND_DESC* pDesc)
{
	if (!pDesc)
		throw INFO_EXCEPT("Trying to initialize a Background with an invalid descriptor pointer.");

	if (isInit)
		throw INFO_EXCEPT("Trying to initialize a Background that has already been initialized.");
	else
		isInit = true;

	backgroundData = new BackgroundInternals;
	BackgroundInternals& data = *(BackgroundInternals*)backgroundData;

	data.desc = *pDesc;

	if (!data.desc.image)
		throw INFO_EXCEPT("Found nullptr when trying to access an Image to create a Background.");

	data.imageDim = { data.desc.image->width(), data.desc.image->height() };

	VertexShader* pvs;
	if (!data.desc.make_dynamic) 
	{
		pvs = AddBind(new VertexShader(SHADERS_DIR L"BackgroundVS.cso"));
		AddBind(new PixelShader(SHADERS_DIR L"BackgroundPS.cso"));

		_float4vector rectangle = { 0.f, 0.f, 1.f, 1.f };
		data.vscBuff = AddBind(new ConstantBuffer(&rectangle, VERTEX_CONSTANT_BUFFER));
	}
	else 
	{
		pvs = AddBind(new VertexShader(SHADERS_DIR L"DynamicBgVS.cso"));

		switch (data.desc.projection_type)
		{
		case BACKGROUND_DESC::PT_MERCATOR:
			AddBind(new PixelShader(SHADERS_DIR L"DyBgMercatorPS.cso"));
			break;

		case BACKGROUND_DESC::PT_AZIMUTHAL:
			AddBind(new PixelShader(SHADERS_DIR L"DyBgAzimuthPS.cso"));
			break;

		default:
			throw INFO_EXCEPT("Unrecognized projection type found when trying to initialize a dynamic Background.");
		}

		data.pscBuff0 = AddBind(new ConstantBuffer(&data.projection, PIXEL_CONSTANT_BUFFER));
	}

	data.textureUpdates = AddBind(new Texture(data.desc.image, data.desc.texture_updates ? TEXTURE_USAGE_DYNAMIC : TEXTURE_USAGE_DEFAULT));

	AddBind(new Sampler(data.desc.pixelated_texture ? SAMPLE_FILTER_POINT : SAMPLE_FILTER_LINEAR, SAMPLE_ADDRESS_WRAP));

	_float4vector vertexs[4] = 
	{ 
		{ -1.f, -1.f,  0.999999f, 1.f }, 
		{ +1.f, -1.f,  0.999999f, 1.f },
		{ -1.f, +1.f,  0.999999f, 1.f },
		{ +1.f, +1.f,  0.999999f, 1.f }
	};
	AddBind(new VertexBuffer(vertexs, 4u));

	INPUT_ELEMENT_DESC ied[1] = { { "Position", _4_FLOAT } };
	AddBind(new InputLayout(ied, 1u, pvs));

	unsigned indexs[4] = { 0u,1u,2u,3u };
	AddBind(new IndexBuffer(indexs, 4u));

	AddBind(new Rasterizer(false));
	AddBind(new Topology(TRIANGLE_STRIP));
	AddBind(new Blender(BLEND_MODE_OPAQUE));
}

void Background::updateTexture(Image* image)
{
	if (!isInit)
		throw INFO_EXCEPT("Trying to update the texture on an uninitialized Background.");

	BackgroundInternals& data = *(BackgroundInternals*)backgroundData;

	if (!image)
		throw INFO_EXCEPT("Found nullptr when trying to access an Image to update a Background.");

	if (!data.desc.texture_updates)
		throw INFO_EXCEPT(
			"Trying to update the texture on a Background without updates enabled.\n"
			"To update the texture on a background set texture_updates on the descriptor to true."
		);

	data.textureUpdates->update(image);
}

void Background::updateObserver(Quaternion observer)
{
	if (!isInit)
		throw INFO_EXCEPT("Trying to update the observer on an uninitialized Background.");

	BackgroundInternals& data = *(BackgroundInternals*)backgroundData;

	if (!data.desc.make_dynamic)
		throw INFO_EXCEPT(
			"Trying to update the observer on a non-dynamic Background. \n"
			"To update the Texture visualization on a non-dynamic Backgroud you can use updateRectangle."
		);

	if (!observer)
		throw INFO_EXCEPT(
			"Invalid quaternion found when trying to update the observer on a Background.\n"
			"Quaternion 0 can not be normalized and therefore can not describe an observer POV."
		);

	data.projection.obs = observer.normalize();

	data.pscBuff0->update(&data.projection);
}

void Background::updateWideness(float FOV, Vector2f WindowDimensions)
{
	if (!isInit)
		throw INFO_EXCEPT("Trying to update the wideness on an uninitialized Background.");

}

void Background::updateRectangle(Vector2i min_coords, Vector2i max_coords)
{
	if (!isInit)
		throw INFO_EXCEPT("Trying to update the rectangle view on an uninitialized Background.");

	BackgroundInternals& data = *(BackgroundInternals*)backgroundData;

	if (data.desc.make_dynamic)
		throw INFO_EXCEPT(
			"Trying to update the rectangle view on a dynamic Background. \n"
			"To update the view on a dynamic Background the functions to call are updateObserver and updateWideness."
		);

	_float4vector rectangle =
	{
		float(min_coords.x) / data.imageDim.x,
		float(min_coords.y) / data.imageDim.y,
		float(max_coords.x) / data.imageDim.x,
		float(max_coords.y) / data.imageDim.y
	};
	data.vscBuff->update(&rectangle);
}

Vector2i Background::getImageDim() const
{
	if (!isInit)
		throw INFO_EXCEPT("Trying to get the image dimensions of an uninitialized Background.");

	BackgroundInternals& data = *(BackgroundInternals*)backgroundData;

	return data.imageDim;
}
