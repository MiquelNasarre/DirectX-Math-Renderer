#include "Drawable/Curve.h"

//	Constructors

Curve::Curve(Graphics& gfx, Vector3f F(float), Vector2f rangeT, UINT Npoints, Color color, bool transparency)
{
	create(gfx, F, rangeT, Npoints, color);
}

Curve::Curve(Graphics& gfx, Vector3f F(float), Vector2f rangeT, UINT Npoints, std::vector<Color> colors, bool transparency)
{
	create(gfx, F, rangeT, Npoints, colors);
}

void Curve::create(Graphics& gfx, Vector3f F(float), Vector2f rangeT, UINT Npoints, Color color, bool transparency)
{
	if (isInit)
		throw std::exception("You cannot create a curve over one that is already initialized");
	else
		isInit = true;

	Vertex* vertexs = (Vertex*)calloc(Npoints + 1, sizeof(Vertex));

	for (UINT i = 0; i <= Npoints; i++)
	{
		vertexs[i].color = color.getColor4();
		vertexs[i].position = F(rangeT.x + float(i) / Npoints * (rangeT.y - rangeT.x)).getVector4();
	}

	unsigned int* indexs = (unsigned int*)calloc(Npoints + 1, sizeof(unsigned int));

	for (UINT i = 0; i <= Npoints; i++)
		indexs[i] = i;

	AddBind(std::make_unique<VertexBuffer>(gfx, vertexs, Npoints + 1));
	AddBind(std::make_unique<IndexBuffer>(gfx, indexs, Npoints + 1));

	addDefaultBinds(gfx, transparency);
}

void Curve::create(Graphics& gfx, Vector3f F(float), Vector2f rangeT, UINT Npoints, std::vector<Color> colors, bool transparency)
{
	if (isInit)
		throw std::exception("You cannot create a curve over one that is already initialized");
	else
		isInit = true;

	if (!colors.size())
		throw std::exception("The color vector for a curve must have at least one color!!");

	Vertex* vertexs = (Vertex*)calloc(Npoints + 1, sizeof(Vertex));

	for (UINT i = 0; i <= Npoints; i++) {
		float c = (colors.size() - 1) * float(i) / Npoints;
		UINT C0 = UINT(c);
		UINT C1 = (C0 + 1) % colors.size();
		c -= C0;

		vertexs[i].position = F(rangeT.x + float(i) / Npoints * (rangeT.y - rangeT.x)).getVector4();
		vertexs[i].color = (colors[C0] * (1 - c) + colors[C1] * c).getColor4();
	}


	unsigned int* indexs = (unsigned int*)calloc(Npoints + 1, sizeof(unsigned int));

	for (UINT i = 0; i <= Npoints; i++)
		indexs[i] = i;

	AddBind(std::make_unique<VertexBuffer>(gfx, vertexs, Npoints + 1));
	AddBind(std::make_unique<IndexBuffer>(gfx, indexs, Npoints + 1));

	addDefaultBinds(gfx, transparency);
}

void Curve::updateShape(Graphics& gfx, Vector3f F(float), Vector2f rangeT, UINT Npoints, Color color)
{
	if (!isInit)
		throw std::exception("You cannot update the shape of a curve if you havent initialized it first");

	Vertex* vertexs = (Vertex*)calloc(Npoints + 1, sizeof(Vertex));

	for (UINT i = 0; i <= Npoints; i++)
	{
		vertexs[i].color = color.getColor4();
		vertexs[i].position = F(rangeT.x + float(i) / Npoints * (rangeT.y - rangeT.x)).getVector4();
	}

	unsigned int* indexs = (unsigned int*)calloc(Npoints + 1, sizeof(unsigned int));

	for (UINT i = 0; i <= Npoints; i++)
		indexs[i] = i;

	changeBind(std::make_unique<VertexBuffer>(gfx, vertexs, Npoints + 1), 0u);
	changeBind(std::make_unique<IndexBuffer>(gfx, indexs, Npoints + 1), 1u);
}

void Curve::updateShape(Graphics& gfx, Vector3f F(float), Vector2f rangeT, UINT Npoints, std::vector<Color> colors)
{
	if (!isInit)
		throw std::exception("You cannot update the shape of a curve if you havent initialized it first");

	if (!colors.size())
		throw std::exception("The color vector for a curve must have at least one color!!");

	Vertex* vertexs = (Vertex*)calloc(Npoints + 1, sizeof(Vertex));

	for (UINT i = 0; i <= Npoints; i++) {
		float c = (colors.size() - 1) * float(i) / Npoints;
		UINT C0 = UINT(c);
		UINT C1 = (C0 + 1) % colors.size();
		c -= C0;

		vertexs[i].position = F(rangeT.x + float(i) / Npoints * (rangeT.y - rangeT.x)).getVector4();
		vertexs[i].color = (colors[C0] * (1 - c) + colors[C1] * c).getColor4();
	}


	unsigned int* indexs = (unsigned int*)calloc(Npoints + 1, sizeof(unsigned int));

	for (UINT i = 0; i <= Npoints; i++)
		indexs[i] = i;

	changeBind(std::make_unique<VertexBuffer>(gfx, vertexs, Npoints + 1), 0u);
	changeBind(std::make_unique<IndexBuffer>(gfx, indexs, Npoints + 1), 1u);
}

//	Public

void Curve::updateRotation(Graphics& gfx, float rotationX, float rotationY, float rotationZ)
{
	vscBuff.rotation = rotationQuaternion({ 1,0,0 }, rotationX) * rotationQuaternion({ 0,1,0 }, rotationY) * rotationQuaternion({ 0,0,1 }, rotationZ);

	pVSCB->Update(gfx, vscBuff);
}

void Curve::updateRotation(Graphics& gfx, Vector3f axis, float angle, bool multiplicative)
{
	if (!multiplicative)
		vscBuff.rotation = rotationQuaternion(axis, angle);
	else
		vscBuff.rotation *= rotationQuaternion(axis, angle);

	vscBuff.rotation.normalize();
	pVSCB->Update(gfx, vscBuff);
}

void Curve::updateRotation(Graphics& gfx, Quaternion rotation, bool multiplicative)
{
	if (!multiplicative)
		vscBuff.rotation = rotation;
	else
		vscBuff.rotation *= rotation;

	vscBuff.rotation.normalize();
	pVSCB->Update(gfx, vscBuff);
}

void Curve::updatePosition(Graphics& gfx, Vector3f position, bool additive)
{
	if (!additive)
		vscBuff.translation = position.getVector4();
	else
	{
		vscBuff.translation.x += position.x;
		vscBuff.translation.y += position.y;
		vscBuff.translation.z += position.z;
	}

	pVSCB->Update(gfx, vscBuff);
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

void Curve::addDefaultBinds(Graphics& gfx, bool transparency)
{
	auto pvs = (VertexShader*)AddBind(std::move(std::make_unique<VertexShader>(gfx, SHADERS_DIR L"CurveVS.cso")));
	AddBind(std::make_unique<PixelShader>(gfx, SHADERS_DIR L"CurvePS.cso"));

	D3D11_INPUT_ELEMENT_DESC ied[2] =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Color",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};

	AddBind(std::make_unique<InputLayout>(gfx, ied, 2u, pvs->GetBytecode()));
	AddBind(std::make_unique<Topology>(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP));
	AddBind(std::make_unique<Blender>(gfx, transparency));

	pVSCB = (ConstantBuffer<VSconstBuffer>*)AddBind(std::make_unique<ConstantBuffer<VSconstBuffer>>(gfx, vscBuff, VERTEX_CONSTANT_BUFFER_TYPE));
}
