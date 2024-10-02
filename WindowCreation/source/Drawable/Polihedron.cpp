#include "Drawable/Polihedron.h"

Polihedron::Polihedron(Graphics& gfx, const Vector3f* vertexs, const Vector3i* triangles, UINT numT, const  Color* colors, bool vertexColor, bool transparency, bool doubleSided)
{

	create(gfx, vertexs, triangles, numT, colors, vertexColor, transparency, doubleSided);

}

Polihedron::Polihedron(Graphics& gfx, const Vector3f* vertexs, const unsigned short* triangles, UINT numT, const Color* colors, bool vertexColor, bool transparency, bool doubleSided)
{

	create(gfx, vertexs, triangles, numT, colors, vertexColor, transparency, doubleSided);

}

void Polihedron::create(Graphics& gfx, const Vector3f* vertexs, const Vector3i* triangles, UINT numT, const Color* colors, bool vertexColor, bool transparency, bool doubleSided)
{
	if (isInit)
		throw std::exception("You cannot create a polihedron over one that is already initialized");
	else
		isInit = true;

	Vertex* V = (Vertex*)calloc(3 * numT, sizeof(Vertex));

	for (UINT i = 0; i < numT; i++)
	{

		V[3 * i].vector = vertexs[triangles[i].x];
		V[3 * i + 1].vector = vertexs[triangles[i].y];
		V[3 * i + 2].vector = vertexs[triangles[i].z];

		if (vertexColor && colors)
		{
			V[3 * i].color = colors[triangles[i].x];
			V[3 * i + 1].color = colors[triangles[i].y];
			V[3 * i + 2].color = colors[triangles[i].z];
		}

		else if (colors)
		{
			V[3 * i].color = colors[i];
			V[3 * i + 1].color = colors[i];
			V[3 * i + 2].color = colors[i];
		}

		else
		{
			V[3 * i].color = Color::White;
			V[3 * i + 1].color = Color::White;
			V[3 * i + 2].color = Color::White;
		}

		Vector3f norm = ((V[3 * i + 1].vector - V[3 * i].vector) * (V[3 * i + 2].vector - V[3 * i].vector)).normalize();
		V[3 * i].norm = norm;
		V[3 * i + 1].norm = norm;
		V[3 * i + 2].norm = norm;
	}

	unsigned int* indexs = (unsigned int*)calloc(3 * numT, sizeof(unsigned int));

	for (UINT i = 0; i < 3 * numT; i++)
		indexs[i] = i;

	AddBind(std::make_unique<VertexBuffer>(gfx, V, 3 * numT));

	AddBind(std::make_unique<IndexBuffer>(gfx, indexs, 3 * numT));

	auto pvs = (VertexShader*)AddBind(std::move(std::make_unique<VertexShader>(gfx, SHADERS_DIR L"TriangleVS.cso")));

	AddBind(std::make_unique<PixelShader>(gfx, SHADERS_DIR L"TrianglePS.cso"));

	D3D11_INPUT_ELEMENT_DESC ied[3] =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Color",0,DXGI_FORMAT_B8G8R8A8_UNORM,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};

	AddBind(std::make_unique<InputLayout>(gfx, ied, 3u, pvs->GetBytecode()));

	AddBind(std::make_unique<Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_unique<Rasterizer>(gfx, doubleSided));

	AddBind(std::make_unique<Blender>(gfx, transparency));

	pVSCB = (ConstantBuffer<VSconstBuffer>*)AddBind(std::make_unique<ConstantBuffer<VSconstBuffer>>(gfx, VERTEX_CONSTANT_BUFFER_TYPE));

	float unused = 0.f;
	pscBuff = {
		60.f,10.f,unused,unused,1.0f, 0.2f, 0.2f, 1.f , 0.f, 8.f, 8.f,unused,
		60.f,10.f,unused,unused,0.0f, 1.0f, 0.0f, 1.f , 0.f,-8.f, 8.f,unused,
		60.f,10.f,unused,unused,0.5f, 0.0f, 1.0f, 1.f ,-8.f, 0.f,-8.f,unused,
		60.f,10.f,unused,unused,1.0f, 1.0f, 0.0f, 1.f , 8.f, 0.f, 8.f,unused,
	};

	pPSCB = (ConstantBuffer<PSconstBuffer>*)AddBind(std::make_unique<ConstantBuffer<PSconstBuffer>>(gfx, pscBuff, PIXEL_CONSTANT_BUFFER_TYPE));

	free(V);
	free(indexs);
}

void Polihedron::create(Graphics& gfx, const Vector3f* vertexs, const unsigned short* triangles, UINT numT, const Color* colors, bool vertexColor, bool transparency, bool doubleSided)
{
	if (isInit)
		throw std::exception("You cannot create a polihedron over one that is already initialized");
	else
		isInit = true;

	Vertex* V = (Vertex*)calloc(3 * numT, sizeof(Vertex));

	for (UINT i = 0; i < numT; i++)
	{

		V[3 * i].vector = vertexs[triangles[3 * i]];
		V[3 * i + 1].vector = vertexs[triangles[3 * i + 1]];
		V[3 * i + 2].vector = vertexs[triangles[3 * i + 2]];

		if (vertexColor && colors)
		{
			V[3 * i].color = colors[triangles[3 * i]];
			V[3 * i + 1].color = colors[triangles[3 * i + 1]];
			V[3 * i + 2].color = colors[triangles[3 * i + 2]];
		}

		else if (colors)
		{
			V[3 * i].color = colors[i];
			V[3 * i + 1].color = colors[i];
			V[3 * i + 2].color = colors[i];
		}

		else
		{
			V[3 * i].color = Color::White;
			V[3 * i + 1].color = Color::White;
			V[3 * i + 2].color = Color::White;
		}

		Vector3f norm = ((V[3 * i + 1].vector - V[3 * i].vector) * (V[3 * i + 2].vector - V[3 * i].vector)).normalize();
		V[3 * i].norm = norm;
		V[3 * i + 1].norm = norm;
		V[3 * i + 2].norm = norm;
	}

	unsigned int* indexs = (unsigned int*)calloc(3 * numT, sizeof(unsigned int));

	for (UINT i = 0; i < 3 * numT; i++)
		indexs[i] = i;

	AddBind(std::make_unique<VertexBuffer>(gfx, V, 3 * numT));

	AddBind(std::make_unique<IndexBuffer>(gfx, indexs, 3 * numT));

	auto pvs = (VertexShader*)AddBind(std::move(std::make_unique<VertexShader>(gfx, SHADERS_DIR L"TriangleVS.cso")));

	AddBind(std::make_unique<PixelShader>(gfx, SHADERS_DIR L"TrianglePS.cso"));

	D3D11_INPUT_ELEMENT_DESC ied[3] =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Color",0,DXGI_FORMAT_B8G8R8A8_UNORM,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};

	AddBind(std::make_unique<InputLayout>(gfx, ied, 3u, pvs->GetBytecode()));

	AddBind(std::make_unique<Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_unique<Rasterizer>(gfx, doubleSided));

	AddBind(std::make_unique<Blender>(gfx, transparency));

	pVSCB = (ConstantBuffer<VSconstBuffer>*)AddBind(std::make_unique<ConstantBuffer<VSconstBuffer>>(gfx, VERTEX_CONSTANT_BUFFER_TYPE));

	float unused = 0.f;
	pscBuff = {
		60.f,10.f,unused,unused,1.0f, 0.2f, 0.2f, 1.f , 0.f, 8.f, 8.f,unused,
		60.f,10.f,unused,unused,0.0f, 1.0f, 0.0f, 1.f , 0.f,-8.f, 8.f,unused,
		60.f,10.f,unused,unused,0.5f, 0.0f, 1.0f, 1.f ,-8.f, 0.f,-8.f,unused,
		60.f,10.f,unused,unused,1.0f, 1.0f, 0.0f, 1.f , 8.f, 0.f, 8.f,unused,
	};

	pPSCB = (ConstantBuffer<PSconstBuffer>*)AddBind(std::make_unique<ConstantBuffer<PSconstBuffer>>(gfx, pscBuff, PIXEL_CONSTANT_BUFFER_TYPE));

	free(V);
	free(indexs);
}

void Polihedron::updateShape(Graphics& gfx, const Vector3f* vertexs, const Vector3i* triangles, UINT numT, const Color* colors, bool vertexColor)
{
	Vertex* V = (Vertex*)calloc(3 * numT, sizeof(Vertex));

	for (UINT i = 0; i < numT; i++)
	{

		V[3 * i].vector = vertexs[triangles[i].x];
		V[3 * i + 1].vector = vertexs[triangles[i].y];
		V[3 * i + 2].vector = vertexs[triangles[i].z];

		if (vertexColor && colors)
		{
			V[3 * i].color = colors[triangles[i].x];
			V[3 * i + 1].color = colors[triangles[i].y];
			V[3 * i + 2].color = colors[triangles[i].z];
		}

		else if (colors)
		{
			V[3 * i].color = colors[i];
			V[3 * i + 1].color = colors[i];
			V[3 * i + 2].color = colors[i];
		}

		else
		{
			V[3 * i].color = Color::White;
			V[3 * i + 1].color = Color::White;
			V[3 * i + 2].color = Color::White;
		}

		Vector3f norm = ((V[3 * i + 1].vector - V[3 * i].vector) * (V[3 * i + 2].vector - V[3 * i].vector)).normalize();
		V[3 * i].norm = norm;
		V[3 * i + 1].norm = norm;
		V[3 * i + 2].norm = norm;
	}

	changeBind(std::make_unique<VertexBuffer>(gfx, V, 3 * numT), 0u);


	unsigned int* indexs = (unsigned int*)calloc(3 * numT, sizeof(unsigned int));

	for (UINT i = 0; i < 3 * numT; i++)
		indexs[i] = i;

	changeBind(std::make_unique<IndexBuffer>(gfx, indexs, 3 * numT), 1u);

	free(V);
	free(indexs);
}

void Polihedron::updateRotation(Graphics& gfx, float rotationX, float rotationY, float rotationZ)
{
	vscBuff.rotation = rotationQuaternion({ 1,0,0 }, rotationX) * rotationQuaternion({ 0,1,0 }, rotationY) * rotationQuaternion({ 0,0,1 }, rotationZ);

	pVSCB->Update(gfx, vscBuff);
}

void Polihedron::updateRotation(Graphics& gfx, Vector3f axis, float angle, bool multiplicative)
{
	if (!multiplicative)
		vscBuff.rotation = rotationQuaternion(axis, angle);
	else
		vscBuff.rotation *= rotationQuaternion(axis, angle);

	vscBuff.rotation.normalize();
	pVSCB->Update(gfx, vscBuff);
}

void Polihedron::updateRotation(Graphics& gfx, Quaternion rotation, bool multiplicative)
{
	if (!isInit)
		throw std::exception("You cannot update rotation to an uninitialized polihedron");

	if (!multiplicative)
		vscBuff.rotation = rotation;
	else
		vscBuff.rotation *= rotation;

	vscBuff.rotation.normalize();
	pVSCB->Update(gfx, vscBuff);
}

void Polihedron::updatePosition(Graphics& gfx, Vector3f position, bool additive)
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

void Polihedron::updateScreenPosition(Graphics& gfx, Vector2f screenDisplacement)
{
	if (!isInit)
		throw std::exception("You cannot update screen position to an uninitialized polihedron");

	vscBuff.screenDisplacement = screenDisplacement.getVector4();

	pVSCB->Update(gfx, vscBuff);
}

void Polihedron::updateLight(Graphics& gfx, UINT id, Vector2f intensity, Color color, Vector3f position)
{
	pscBuff.lightsource[id] = { intensity.getVector4() , color.getColor4() , position.getVector4() };
	pPSCB->Update(gfx, pscBuff);
}

void Polihedron::updateLight(Graphics& gfx, UINT id, _float4vector intensity, _float4color color, _float4vector position)
{
	pscBuff.lightsource[id] = { intensity , color , position };
	pPSCB->Update(gfx, pscBuff);
}

void Polihedron::clearLights(Graphics& gfx)
{
	pscBuff = {};
	pPSCB->Update(gfx, pscBuff);
}

Quaternion Polihedron::getRotation()
{
	return vscBuff.rotation;
}

Vector3f Polihedron::getPosition()
{
	return Vector3f(vscBuff.translation.x, vscBuff.translation.y, vscBuff.translation.z);
}
