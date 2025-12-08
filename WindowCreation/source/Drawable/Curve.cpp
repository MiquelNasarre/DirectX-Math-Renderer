#include "Drawable/Curve.h"
#include "Bindable/BindableBase.h"

#include "WinHeader.h"
#include "Exception/_exDefault.h"

//	Constructors

Curve::Curve(Vector3f(*F)(float), Vector2f rangeT, unsigned Npoints, Color(*color)(float), bool transparency)
{
	isInit = true;

	Vertex* vertexs = (Vertex*)calloc(Npoints + 1, sizeof(Vertex));

	for (UINT i = 0; i <= Npoints; i++) {
		float t = rangeT.x + float(i) / Npoints * (rangeT.y - rangeT.x);

		vertexs[i].position = F(t).getVector4();
		vertexs[i].color = color(t).getColor4();
	}

	unsigned int* indexs = (unsigned int*)calloc(Npoints + 1, sizeof(unsigned int));

	for (UINT i = 0; i <= Npoints; i++)
		indexs[i] = i;

	AddBind(new VertexBuffer(vertexs, Npoints + 1));
	AddBind(new IndexBuffer(indexs, Npoints + 1));

	free(vertexs);
	free(indexs);

	addDefaultBinds(transparency);
}

void Curve::updateShape(Vector3f(*F)(float), Vector2f rangeT, unsigned Npoints, Color(*color)(float))
{
	if (!isInit)
		throw INFO_EXCEPT("You cannot update the shape of a curve if you havent initialized it first");

	Vertex* vertexs = (Vertex*)calloc(Npoints + 1, sizeof(Vertex));

	for (UINT i = 0; i <= Npoints; i++) {
		float t = rangeT.x + float(i) / Npoints * (rangeT.y - rangeT.x);

		vertexs[i].position = F(t).getVector4();
		vertexs[i].color = color(t).getColor4();
	}

	unsigned int* indexs = (unsigned int*)calloc(Npoints + 1, sizeof(unsigned int));

	for (UINT i = 0; i <= Npoints; i++)
		indexs[i] = i;

	changeBind(new VertexBuffer(vertexs, Npoints + 1), 0u);
	changeBind(new IndexBuffer(indexs, Npoints + 1), 1u);

	free(vertexs);
	free(indexs);
}

//	Public

void Curve::updateRotation(Quaternion rotation, bool multiplicative)
{
	if (!multiplicative)
		vscBuff.rotation = rotation;
	else
		vscBuff.rotation *= rotation;

	vscBuff.rotation.normalize();
	((ConstantBuffer*)pVSCB)->Update(vscBuff);
}

void Curve::updatePosition(Vector3f position, bool additive)
{
	if (!additive)
		vscBuff.translation = position.getVector4();
	else
	{
		vscBuff.translation.x += position.x;
		vscBuff.translation.y += position.y;
		vscBuff.translation.z += position.z;
	}

	((ConstantBuffer*)pVSCB)->Update(vscBuff);
}

Quaternion Curve::getRotation()
{
	return vscBuff.rotation;
}

Vector3f Curve::getPosition()
{
	return Vector3f(vscBuff.translation.x, vscBuff.translation.y, vscBuff.translation.z);
}

//	Private

void Curve::addDefaultBinds(bool transparency)
{
	VertexShader* pvs = (VertexShader*)AddBind(new VertexShader(SHADERS_DIR L"CurveVS.cso"));
	AddBind(new PixelShader(SHADERS_DIR L"CurvePS.cso"));

	D3D11_INPUT_ELEMENT_DESC ied[2] =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Color",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};

	AddBind(new InputLayout(ied, 2u, pvs->GetBytecode()));
	AddBind(new Topology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP));
	AddBind(new Blender(transparency));

	pVSCB = AddBind(new ConstantBuffer(vscBuff, VERTEX_CONSTANT_BUFFER_TYPE));
}
