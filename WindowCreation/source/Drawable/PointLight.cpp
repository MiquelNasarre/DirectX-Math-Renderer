#include "Drawable/PointLight.h"
#include "Bindable/BindableBase.h"

#include "Graphics.h"
#include "WinHeader.h"

#include <math.h>
#include <memory>

PointLight::PointLight(Color color, Vector3f Position, float Radius, unsigned circlePoints)
{
	Vertex* vertexs = (Vertex*)calloc(circlePoints + 1, sizeof(Vertex));
	vertexs[0].intensity = 1;
	vertexs[0].reference = { 0,0 };

	for (unsigned i = 1; i <= circlePoints; i++)
	{
		vertexs[i].intensity = 0;
		vertexs[i].reference = { cosf(2.f * MATH_PI * float(i) / circlePoints) , sinf(2.f * MATH_PI * float(i) / circlePoints) };
	}

	unsigned int* indexs = (unsigned int*)calloc(circlePoints * 3, sizeof(unsigned int));
	for (unsigned i = 1; i <= circlePoints; i++)
	{
		indexs[3 * (i - 1)] = 0;
		indexs[3 * (i - 1) + 1] = i % circlePoints + 1;
		indexs[3 * (i - 1) + 2] = i;
	}

	AddBind(new VertexBuffer(vertexs, circlePoints + 1));
	AddBind(new IndexBuffer(indexs, circlePoints * 3));

	VertexShader* pvs = (VertexShader*)AddBind(new VertexShader(SHADERS_DIR L"PointLightVS.cso"));
	AddBind(new PixelShader(SHADERS_DIR L"PointLightPS.cso"));

	D3D11_INPUT_ELEMENT_DESC ied[2] =
	{
		{ "Reference",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Intensity",0,DXGI_FORMAT_R32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};

	AddBind(new InputLayout(ied, 2u, pvs->GetBytecode()));

	AddBind(new Topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(new Rasterizer(false));

	AddBind(new Blender(true));

	vscBuff = { Position.getVector4(), _float4vector(), {Radius, 0.f, 0.f, 0.f}};
	pscBuff = { color.getColor4() };

	pVSCB = (ConstantBuffer*)AddBind(new ConstantBuffer(vscBuff, VERTEX_CONSTANT_BUFFER_TYPE));
	pPSCB = (ConstantBuffer*)AddBind(new ConstantBuffer(pscBuff, PIXEL_CONSTANT_BUFFER_TYPE));
}

void PointLight::updateRadius(float radius)
{
	vscBuff.radius = { radius, 0.f, 0.f, 0.f };

	((ConstantBuffer*)pVSCB)->Update(vscBuff);
}

void PointLight::updatePosition(Vector3f position)
{
	vscBuff.Position = position.getVector4();

	((ConstantBuffer*)pVSCB)->Update(vscBuff);
}

void PointLight::updateColor(Color color)
{
	pscBuff.color = color.getColor4();

	((ConstantBuffer*)pPSCB)->Update(pscBuff);
}

void PointLight::Draw(Window& _w)
{
	vscBuff.observer = _w.graphics().getObserver().getVector4();
	((ConstantBuffer*)pVSCB)->Update(vscBuff);

	_draw(_w);
}
