#include "Drawable/Curve.h"
#include "Bindable/BindableBase.h"

#include "Exception/_exDefault.h"

//	Constructors

Curve::Curve(Vector3f(*F)(float), Vector2f rangeT, unsigned Npoints, Color(*color)(float), bool transparency)
{
	isInit = true;

	Vertex* vertexs = (Vertex*)calloc(Npoints + 1, sizeof(Vertex));

	for (unsigned i = 0; i <= Npoints; i++) {
		float t = rangeT.x + float(i) / Npoints * (rangeT.y - rangeT.x);

		vertexs[i].position = F(t).getVector4();
		vertexs[i].color = color(t).getColor4();
	}

	unsigned int* indexs = (unsigned int*)calloc(Npoints + 1, sizeof(unsigned int));

	for (unsigned i = 0; i <= Npoints; i++)
		indexs[i] = i;

	AddBind(new VertexBuffer(vertexs, Npoints + 1));
	AddBind(new IndexBuffer(indexs, Npoints + 1));

	free(vertexs);
	free(indexs);

	VertexShader* pvs = AddBind(new VertexShader(SHADERS_DIR L"CurveVS.cso"));
	AddBind(new PixelShader(SHADERS_DIR L"CurvePS.cso"));

	INPUT_ELEMENT_DESC ied[2] =
	{
		{ "Position",	_4_FLOAT },
		{ "Color",		_4_FLOAT },
	};

	AddBind(new InputLayout(ied, 2u, pvs));
	AddBind(new Topology(LINE_STRIP));
	AddBind(new Blender(transparency ? BLEND_MODE_OIT_WEIGHTED : BLEND_MODE_OPAQUE));

	pVSCB = AddBind(new ConstantBuffer(&vscBuff, VERTEX_CONSTANT_BUFFER));
}

void Curve::updateShape(Vector3f(*F)(float), Vector2f rangeT, unsigned Npoints, Color(*color)(float))
{
	if (!isInit)
		throw INFO_EXCEPT("You cannot update the shape of a curve if you havent initialized it first");

	Vertex* vertexs = (Vertex*)calloc(Npoints + 1, sizeof(Vertex));

	for (unsigned i = 0; i <= Npoints; i++) {
		float t = rangeT.x + float(i) / Npoints * (rangeT.y - rangeT.x);

		vertexs[i].position = F(t).getVector4();
		vertexs[i].color = color(t).getColor4();
	}

	unsigned int* indexs = (unsigned int*)calloc(Npoints + 1, sizeof(unsigned int));

	for (unsigned i = 0; i <= Npoints; i++)
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
	((ConstantBuffer*)pVSCB)->update(&vscBuff);
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

	((ConstantBuffer*)pVSCB)->update(&vscBuff);
}

Quaternion Curve::getRotation()
{
	return vscBuff.rotation;
}

Vector3f Curve::getPosition()
{
	return Vector3f(vscBuff.translation.x, vscBuff.translation.y, vscBuff.translation.z);
}
