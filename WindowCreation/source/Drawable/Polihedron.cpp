#include "Drawable/Polihedron.h"
#include "Bindable/BindableBase.h"
#include "Exception/_exDefault.h"

#include "WinHeader.h"

Polihedron::Polihedron(const Vector3f* vertexs, const Vector3i* triangles, unsigned numT, const  Color* colors, bool vertexColor, bool transparency, bool doubleSided)
{
	create(vertexs, triangles, numT, colors, vertexColor, transparency, doubleSided);
}

void Polihedron::create(const Vector3f* vertexs, const Vector3i* triangles, UINT numT, const Color* colors, bool vertexColor, bool transparency, bool doubleSided)
{
	if (isInit)
		throw INFO_EXCEPT("You cannot create a polihedron over one that is already initialized");

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

	AddBind(new VertexBuffer(V, 3 * numT));

	AddBind(new IndexBuffer(indexs, 3 * numT));

	VertexShader* pvs = (VertexShader*)AddBind(new VertexShader(SHADERS_DIR L"TriangleVS.cso"));

	AddBind(new PixelShader(SHADERS_DIR L"TrianglePS.cso"));

	D3D11_INPUT_ELEMENT_DESC ied[3] =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Color",0,DXGI_FORMAT_B8G8R8A8_UNORM,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};

	AddBind(new InputLayout(ied, 3u, pvs->GetBytecode()));

	AddBind(new Topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(new Rasterizer(doubleSided));

	AddBind(new Blender(transparency));

	pVSCB = (ConstantBuffer*)AddBind(new ConstantBuffer(vscBuff, VERTEX_CONSTANT_BUFFER_TYPE));

	float unused = 0.f;
	pscBuff = {
		60.f,10.f,unused,unused,1.0f, 0.2f, 0.2f, 1.f , 0.f, 8.f, 8.f,unused,
		60.f,10.f,unused,unused,0.0f, 1.0f, 0.0f, 1.f , 0.f,-8.f, 8.f,unused,
		60.f,10.f,unused,unused,0.5f, 0.0f, 1.0f, 1.f ,-8.f, 0.f,-8.f,unused,
		60.f,10.f,unused,unused,1.0f, 1.0f, 0.0f, 1.f , 8.f, 0.f, 8.f,unused,
	};

	pPSCB = (ConstantBuffer*)AddBind(new ConstantBuffer(pscBuff, PIXEL_CONSTANT_BUFFER_TYPE));

	free(V);
	free(indexs);
}

void Polihedron::updateShape(const Vector3f* vertexs, const Vector3i* triangles,unsigned numT, const Color* colors, bool vertexColor)
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

	changeBind(new VertexBuffer(V, 3 * numT), 0u);


	unsigned int* indexs = (unsigned int*)calloc(3 * numT, sizeof(unsigned int));

	for (UINT i = 0; i < 3 * numT; i++)
		indexs[i] = i;

	changeBind(new IndexBuffer(indexs, 3 * numT), 1u);

	free(V);
	free(indexs);
}

void Polihedron::updateRotation(Quaternion rotation, bool multiplicative)
{
	if (!isInit)
		throw INFO_EXCEPT("You cannot update rotation to an uninitialized polihedron");

	if (!multiplicative)
		vscBuff.rotation = rotation;
	else
		vscBuff.rotation *= rotation;

	vscBuff.rotation.normalize();
	((ConstantBuffer*)pVSCB)->Update(vscBuff);
}

void Polihedron::updatePosition(Vector3f position, bool additive)
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

void Polihedron::updateScreenPosition(Vector2f screenDisplacement)
{
	if (!isInit)
		throw INFO_EXCEPT("You cannot update screen position to an uninitialized polihedron");

	vscBuff.screenDisplacement = screenDisplacement.getVector4();

	((ConstantBuffer*)pVSCB)->Update(vscBuff);
}

void Polihedron::updateLight(unsigned id, Vector2f intensity, Color color, Vector3f position)
{
	pscBuff.lightsource[id] = { intensity.getVector4() , color.getColor4() , position.getVector4() };
	((ConstantBuffer*)pPSCB)->Update(pscBuff);
}

void Polihedron::clearLights()
{
	pscBuff = {};
	((ConstantBuffer*)pPSCB)->Update(pscBuff);
}

Quaternion Polihedron::getRotation()
{
	return vscBuff.rotation;
}

Vector3f Polihedron::getPosition()
{
	return Vector3f(vscBuff.translation.x, vscBuff.translation.y, vscBuff.translation.z);
}
