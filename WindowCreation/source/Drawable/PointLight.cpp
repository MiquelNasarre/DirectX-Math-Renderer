#include "Drawable/PointLight.h"
#include "Bindable/BindableBase.h"

#include <math.h>

PointLight::PointLight(Color color, Vector3f Position, float Radius, unsigned circlePoints)
{
	Vertex* vertexs = new Vertex[circlePoints + 1];
	vertexs[0].intensity = 1;
	vertexs[0].reference = { 0,0 };

	for (unsigned i = 1; i <= circlePoints; i++)
	{
		vertexs[i].intensity = 0;
		vertexs[i].reference = { cosf(2.f * MATH_PI * float(i) / circlePoints) , sinf(2.f * MATH_PI * float(i) / circlePoints) };
	}

	unsigned* indexs = new unsigned[circlePoints * 3];
	for (unsigned i = 1; i <= circlePoints; i++)
	{
		indexs[3 * (i - 1)] = 0;
		indexs[3 * (i - 1) + 1] = i % circlePoints + 1;
		indexs[3 * (i - 1) + 2] = i;
	}

	AddBind(new VertexBuffer(vertexs, circlePoints + 1));
	AddBind(new IndexBuffer(indexs, circlePoints * 3));

	delete[] vertexs;
	delete[] indexs;

	VertexShader* pvs = AddBind(new VertexShader(SHADERS_DIR L"PointLightVS.cso"));
	AddBind(new PixelShader(SHADERS_DIR L"PointLightPS.cso"));

	INPUT_ELEMENT_DESC ied[2] =
	{
		{ "Reference", _2_FLOAT },
		{ "Intensity", _1_FLOAT },
	};

	AddBind(new InputLayout(ied, 2u, pvs));

	AddBind(new Topology(TRIANGLE_LIST));

	AddBind(new Rasterizer(false));

	AddBind(new Blender(BLEND_MODE_ADDITIVE));

	vscBuff = { Position.getVector4(), _float4vector(), {Radius, 0.f, 0.f, 0.f}};
	pscBuff = { color.getColor4() };

	pVSCB = (ConstantBuffer*)AddBind(new ConstantBuffer(&vscBuff, VERTEX_CONSTANT_BUFFER));
	pPSCB = (ConstantBuffer*)AddBind(new ConstantBuffer(&pscBuff, PIXEL_CONSTANT_BUFFER));
}

void PointLight::updateRadius(float radius)
{
	vscBuff.radius = { radius, 0.f, 0.f, 0.f };

	((ConstantBuffer*)pVSCB)->update(&vscBuff);
}

void PointLight::updatePosition(Vector3f position)
{
	vscBuff.Position = position.getVector4();

	((ConstantBuffer*)pVSCB)->update(&vscBuff);
}

void PointLight::updateColor(Color color)
{
	pscBuff.color = color.getColor4();

	((ConstantBuffer*)pPSCB)->update(&pscBuff);
}

void PointLight::Draw(Window& _w)
{
	vscBuff.observer = _w.graphics().getObserver().getVector4();
	((ConstantBuffer*)pVSCB)->update(&vscBuff);

	_draw();
}
