#include "Drawable/Background.h"
#include "Bindable/BindableBase.h"

#include "WinHeader.h"
#include "Exception/_exGraphics.h"

Background::Background(Image& texture, bool MakeDynamic, PROJECTION_TYPES ProjectionType)
{
	if (isInit)
		throw INFO_EXCEPT("You cannot create a background over one that is already initialized");
	else
		isInit = true;

	AddBind(new Texture(texture));

	AddBind(new Sampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR));

	_float4vector vertexs[4] = {
		{ -1.f,-1.f,1.f,1.f },
		{  1.f,-1.f,1.f,1.f },
		{ -1.f, 1.f,1.f,1.f },
		{  1.f, 1.f,1.f,1.f },
	};

	AddBind(new VertexBuffer(vertexs, 4u));

	unsigned int indexs[6] = { 0u,2u,1u,1u,2u,3u };

	AddBind(new IndexBuffer(indexs, 6u));

	VertexShader* pvs;

	if (!MakeDynamic) {
		pvs = (VertexShader*)AddBind(new VertexShader(SHADERS_DIR L"BackgroundVS.cso"));
		AddBind(new PixelShader(SHADERS_DIR L"BackgroundPS.cso"));

		vscBuff = AddBind(new ConstantBuffer(_float4vector{ 0.f,0.f,1.f,1.f }, VERTEX_CONSTANT_BUFFER_TYPE));
	}
	else {
		pvs = (VertexShader*)AddBind(new VertexShader(SHADERS_DIR L"DynamicBgVS.cso"));

		if (ProjectionType == PT_MERCATOR)
			AddBind(new PixelShader(SHADERS_DIR L"DyBgMercatorPS.cso"));
		else if (ProjectionType == PT_AZIMUTHAL)
			AddBind(new PixelShader(SHADERS_DIR L"DyBgAzimuthPS.cso"));
		else
			throw INFO_EXCEPT("This Projection Type is not suported by the dynamic bacground");

		pscBuff0 = AddBind(new ConstantBuffer(pscBuff0, PIXEL_CONSTANT_BUFFER_TYPE));
		pscBuff1 = AddBind(new ConstantBuffer(_float4vector(), PIXEL_CONSTANT_BUFFER_TYPE, 1u));
	}

	AddBind(new Topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	D3D11_INPUT_ELEMENT_DESC ied[1] =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};

	AddBind(new InputLayout(ied, 1u, pvs->GetBytecode()));

	AddBind(new Rasterizer(false));

	AddBind(new Blender(false));
}

void Background::updateTexture(Image& texture)
{
	changeBind(new Texture(texture), 0u);
}

void Background::updateObserver(Vector3f obs)
{
	if (!pscBuff0)
		throw INFO_EXCEPT("Not possible to call updateObserver() on a non dynamic background!!");

	cBuff0.obs = obs.normalize().getVector4();
	cBuff0.ei = Vector3f(obs.y, -obs.x, 0.f).normalize().getVector4();
	cBuff0.zp = (obs * Vector3f(obs.y, -obs.x, 0.f)).normalize().getVector4();
	((ConstantBuffer*)pscBuff0)->Update(cBuff0);
}

void Background::updateWideness(float FOV, Vector2f WindowDimensions)
{
	if (!pscBuff1)
		throw INFO_EXCEPT("Not possible to call updateWideness() on a non dynamic background!!");

	((ConstantBuffer*)pscBuff1)->Update(Vector3f(FOV, WindowDimensions.x, WindowDimensions.y).getVector4());
}

void Background::updateRectangle(_float4vector rectangle)
{
	if (!vscBuff)
		throw INFO_EXCEPT("Not possible to call updateRectangle() on a dynamic background!!");

	((ConstantBuffer*)vscBuff)->Update(rectangle);
}
