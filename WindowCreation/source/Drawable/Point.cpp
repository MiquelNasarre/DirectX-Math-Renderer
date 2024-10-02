#include "Drawable/Point.h"

Point::Point(Graphics& gfx, Vector3f position, float radius, Color color)
{
	UINT depth = 4;

	unsigned int currentdepth = 0;
	unsigned int V = 12u;
	unsigned int C = 20u;
	unsigned int A = 30u;

	struct arista {
		unsigned int v0;
		unsigned int v1;
	};

	struct triangle {
		int a0;
		int a1;
		int a2;
	};

	Vector3f* vertexs = (Vector3f*)calloc(V, sizeof(Vector3f));
	arista* aristas = (arista*)calloc(A, sizeof(arista));
	triangle* triangles = (triangle*)calloc(C, sizeof(triangle));

	float gold = (1.f + sqrtf(5)) / 2.f;

	vertexs[0] = { 0.f, 1.f, gold };
	vertexs[1] = { 0.f, 1.f,-gold };
	vertexs[2] = { 0.f,-1.f, gold };
	vertexs[3] = { 0.f,-1.f,-gold };
	vertexs[4] = { 1.f, gold, 0.f };
	vertexs[5] = { 1.f,-gold, 0.f };
	vertexs[6] = { -1.f, gold, 0.f };
	vertexs[7] = { -1.f,-gold, 0.f };
	vertexs[8] = { gold, 0.f, 1.f };
	vertexs[9] = { -gold, 0.f, 1.f };
	vertexs[10] = { gold, 0.f,-1.f };
	vertexs[11] = { -gold, 0.f,-1.f };


	aristas[0] = { 0, 2 };
	aristas[1] = { 0, 4 };
	aristas[2] = { 0, 6 };
	aristas[3] = { 0, 8 };
	aristas[4] = { 0, 9 };
	aristas[5] = { 1, 3 };
	aristas[6] = { 1, 4 };
	aristas[7] = { 1, 6 };
	aristas[8] = { 1,10 };
	aristas[9] = { 1,11 };
	aristas[10] = { 2, 5 };
	aristas[11] = { 2, 7 };
	aristas[12] = { 2, 8 };
	aristas[13] = { 2, 9 };
	aristas[14] = { 3, 5 };
	aristas[15] = { 3, 7 };
	aristas[16] = { 3,10 };
	aristas[17] = { 3,11 };
	aristas[18] = { 4, 6 };
	aristas[19] = { 4, 8 };
	aristas[20] = { 4,10 };
	aristas[21] = { 5, 7 };
	aristas[22] = { 5, 8 };
	aristas[23] = { 5,10 };
	aristas[24] = { 6, 9 };
	aristas[25] = { 6,11 };
	aristas[26] = { 7, 9 };
	aristas[27] = { 7,11 };
	aristas[28] = { 8,10 };
	aristas[29] = { 9,11 };


	triangles[0] = { 3,-19, -2 };
	triangles[1] = { 7, 19, -8 };
	triangles[2] = { 11, 22,-12 };
	triangles[3] = { 16,-22,-15 };
	triangles[4] = { 21,-29,-20 };
	triangles[5] = { 23, 29,-24 };
	triangles[6] = { 25, 30,-26 };
	triangles[7] = { 28,-30,-27 };
	triangles[8] = { -13, -1,  4 };
	triangles[9] = { -5,  1, 14 };
	triangles[10] = { -9,  6, 17 };
	triangles[11] = { -18, -6, 10 };
	triangles[12] = { 2, 20, -4 };
	triangles[13] = { 5,-25, -3 };
	triangles[14] = { -7,  9,-21 };
	triangles[15] = { 26,-10,  8 };
	triangles[16] = { 18,-28,-16 };
	triangles[17] = { 15, 24,-17 };
	triangles[18] = { 27,-14, 12 };
	triangles[19] = { -11, 13,-23 };

	unsigned int pV;
	unsigned int pC;
	unsigned int pA;
	triangle* ptriangles = NULL;
	arista* paristas = NULL;
	Vector3f* pvertexs = NULL;

	while (currentdepth < depth)
	{
		if (ptriangles) free(ptriangles);
		if (paristas)	free(paristas);
		if (pvertexs)	free(pvertexs);

		pV = V;
		pC = C;
		pA = A;
		ptriangles = triangles;
		pvertexs = vertexs;
		paristas = aristas;

		currentdepth++;
		V += A;
		A *= 4;
		C *= 4;

		vertexs = (Vector3f*)calloc(V, sizeof(Vector3f));
		aristas = (arista*)calloc(A, sizeof(arista));
		triangles = (triangle*)calloc(C, sizeof(triangle));

		for (unsigned int i = 0; i < pV; i++)
			vertexs[i] = pvertexs[i];

		for (unsigned int i = 0; i < pA; i++)
		{
			vertexs[pV + i] = (vertexs[paristas[i].v0] + vertexs[paristas[i].v1]) / 2.f;
			aristas[2 * i] = { paristas[i].v0, pV + i };
			aristas[2 * i + 1] = { pV + i, paristas[i].v1 };
		}

		for (unsigned int i = 0; i < pC; i++)
		{
			unsigned int aris0 = abs(ptriangles[i].a0) - 1;
			bool ornt0 = (ptriangles[i].a0 > 0) ? true : false;
			unsigned int aris1 = abs(ptriangles[i].a1) - 1;
			bool ornt1 = (ptriangles[i].a1 > 0) ? true : false;
			unsigned int aris2 = abs(ptriangles[i].a2) - 1;
			bool ornt2 = (ptriangles[i].a2 > 0) ? true : false;

			aristas[2 * pA + 3 * i] = { aristas[2 * aris0].v1, aristas[2 * aris1].v1 };
			aristas[2 * pA + 3 * i + 1] = { aristas[2 * aris1].v1, aristas[2 * aris2].v1 };
			aristas[2 * pA + 3 * i + 2] = { aristas[2 * aris2].v1, aristas[2 * aris0].v1 };

			triangles[4 * i] = { int(2 * pA + 3 * i + 1) , int(2 * pA + 3 * i + 2) , int(2 * pA + 3 * i + 3) };

			if (ornt0 && ornt1)
				triangles[4 * i + 1] = { int(2 * aris0 + 2), int(2 * aris1 + 1), -int(2 * pA + 3 * i + 1) };
			else if (ornt0 && !ornt1)
				triangles[4 * i + 1] = { int(2 * aris0 + 2), -int(2 * aris1 + 2), -int(2 * pA + 3 * i + 1) };
			else if (!ornt0 && ornt1)
				triangles[4 * i + 1] = { -int(2 * aris0 + 1), int(2 * aris1 + 1), -int(2 * pA + 3 * i + 1) };
			else if (!ornt0 && !ornt1)
				triangles[4 * i + 1] = { -int(2 * aris0 + 1), -int(2 * aris1 + 2), -int(2 * pA + 3 * i + 1) };

			if (ornt1 && ornt2)
				triangles[4 * i + 2] = { int(2 * aris1 + 2), int(2 * aris2 + 1), -int(2 * pA + 3 * i + 2) };
			else if (ornt1 && !ornt2)
				triangles[4 * i + 2] = { int(2 * aris1 + 2), -int(2 * aris2 + 2), -int(2 * pA + 3 * i + 2) };
			else if (!ornt1 && ornt2)
				triangles[4 * i + 2] = { -int(2 * aris1 + 1), int(2 * aris2 + 1), -int(2 * pA + 3 * i + 2) };
			else if (!ornt1 && !ornt2)
				triangles[4 * i + 2] = { -int(2 * aris1 + 1), -int(2 * aris2 + 2), -int(2 * pA + 3 * i + 2) };

			if (ornt2 && ornt0)
				triangles[4 * i + 3] = { int(2 * aris2 + 2), int(2 * aris0 + 1), -int(2 * pA + 3 * i + 3) };
			else if (ornt2 && !ornt0)
				triangles[4 * i + 3] = { int(2 * aris2 + 2), -int(2 * aris0 + 2), -int(2 * pA + 3 * i + 3) };
			else if (!ornt2 && ornt0)
				triangles[4 * i + 3] = { -int(2 * aris2 + 1), int(2 * aris0 + 1), -int(2 * pA + 3 * i + 3) };
			else if (!ornt2 && !ornt0)
				triangles[4 * i + 3] = { -int(2 * aris2 + 1), -int(2 * aris0 + 2), -int(2 * pA + 3 * i + 3) };

		}
	}

	if (ptriangles) free(ptriangles);
	if (paristas)	free(paristas);
	if (pvertexs)	free(pvertexs);

	unsigned int* indexs = (unsigned int*)calloc(3 * C, sizeof(unsigned int));

	for (unsigned int i = 0; i < C; i++)
	{
		unsigned int aris0 = abs(triangles[i].a0) - 1;
		bool ornt0 = (triangles[i].a0 > 0) ? true : false;
		unsigned int aris1 = abs(triangles[i].a1) - 1;
		bool ornt1 = (triangles[i].a1 > 0) ? true : false;
		unsigned int aris2 = abs(triangles[i].a2) - 1;
		bool ornt2 = (triangles[i].a2 > 0) ? true : false;

		indexs[3 * i] = ornt0 ? aristas[aris0].v0 : aristas[aris0].v1;
		indexs[3 * i + 1] = ornt1 ? aristas[aris1].v0 : aristas[aris1].v1;
		indexs[3 * i + 2] = ornt2 ? aristas[aris2].v0 : aristas[aris2].v1;

	}

	for (unsigned int i = 0; i < V; i++) vertexs[i].normalize();

	AddBind(std::make_unique<VertexBuffer>(gfx, vertexs, V));
	AddBind(std::make_unique<IndexBuffer>(gfx, indexs, 3 * C));

	auto pvs = (VertexShader*)AddBind(std::move(std::make_unique<VertexShader>(gfx, SHADERS_DIR L"PointVS.cso")));
	AddBind(std::make_unique<PixelShader>(gfx, SHADERS_DIR L"PointPS.cso"));

	D3D11_INPUT_ELEMENT_DESC ied[1] =
	{
		{ "Normal",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};

	AddBind(std::make_unique<InputLayout>(gfx, ied, 1u, pvs->GetBytecode()));
	AddBind(std::make_unique<Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	AddBind(std::make_unique<Rasterizer>(gfx, false));
	AddBind(std::make_unique<Blender>(gfx, color.A != 255));

	vscBuff = { position.getVector4(), 1.f, radius, gfx.getScale() };
	pscBuff = { color.getColor4() };

	pVSCB = (ConstantBuffer<VSconstBuffer>*)AddBind(std::make_unique<ConstantBuffer<VSconstBuffer>>(gfx, vscBuff, VERTEX_CONSTANT_BUFFER_TYPE));
	pPSCB = (ConstantBuffer<PSconstBuffer>*)AddBind(std::make_unique<ConstantBuffer<PSconstBuffer>>(gfx, pscBuff, PIXEL_CONSTANT_BUFFER_TYPE));
}

void Point::updatePosition(Graphics& gfx, Vector3f position)
{
	vscBuff.position = position.getVector4();
	pVSCB->Update(gfx, &vscBuff);
}

void Point::updateRadius(Graphics& gfx, float radius)
{
	vscBuff.radius = radius;
	pVSCB->Update(gfx, &vscBuff);
}

void Point::updateColor(Graphics& gfx, Color col)
{
	pscBuff.color = col.getColor4();
	pPSCB->Update(gfx, &pscBuff);
}

void Point::updateRotation(Graphics& gfx, Vector3f axis, float angle, bool multiplicative)
{
	if (!multiplicative)
		vscBuff.rotation = rotationQuaternion(axis, angle);
	else
		vscBuff.rotation *= rotationQuaternion(axis, angle);

	vscBuff.rotation.normalize();
	pVSCB->Update(gfx, vscBuff);
}

void Point::updateRotation(Graphics& gfx, Quaternion rotation, bool multiplicative)
{
	if (!multiplicative)
		vscBuff.rotation = rotation;
	else
		vscBuff.rotation *= rotation;

	vscBuff.rotation.normalize();
	pVSCB->Update(gfx, vscBuff);
}

void Point::Draw(Graphics& gfx)
{
	vscBuff.scale = gfx.getScale();
	pVSCB->Update(gfx, &vscBuff);

	_draw(gfx);
}
