#include "Interpolated.h"

Interpolated::~Interpolated()
{
	if (Vertexs)
		free(Vertexs);
	if (Coef0)
		free(Coef0);
	if (Coef1)
		free(Coef1);
}

Interpolated::Interpolated(Graphics& gfx, FourierSurface* surface0, FourierSurface* surface1, float t)
{
	create(gfx, surface0, surface1, t);
}

Interpolated::Interpolated(Graphics& gfx, Interpolated* interpolation0, Interpolated* interpolation1, int s0, int s1, float t)
{
	create(gfx, interpolation0, interpolation1, s0, s1, t);
}

Interpolated::Interpolated(Graphics& gfx, Interpolated* interpolation0, FourierSurface* surface1, float t)
{
	create(gfx, interpolation0, surface1, t);
}

void Interpolated::create(Graphics& gfx, FourierSurface* surface0, FourierSurface* surface1, float t)
{
	if (isInit)
		throw std::exception("You cannot create a surface over one that is already initialized");
	else
		isInit = true;

	while (t > 1)t--;
	while (t < 0)t++;
	vscBuff.tvalue.x = t;

	FourierSurface::Vertex* V0 = surface0->getVertexPtr();
	FourierSurface::Vertex* V1 = surface1->getVertexPtr();
	unsigned int N = FourierSurface::getNvertexs();

	Vertexs = (InterpolatedVertex*)calloc(N, sizeof(InterpolatedVertex));

	for (unsigned int i = 0; i < N; i++)
	{
		Vertexs[i].vector = V0[i].vector;
		Vertexs[i].dYlm0 = V0[i].dYlm;
		Vertexs[i].dYlm1 = V1[i].dYlm;
		Vertexs[i].color = Color::White;
	}

	Ncoef0 = surface0->getNcoefficients();
	Ncoef1 = surface1->getNcoefficients();
	Coef0 = (Coefficient*)calloc(Ncoef0, sizeof(Coefficient));
	Coef1 = (Coefficient*)calloc(Ncoef1, sizeof(Coefficient));

	Coefficient* pCoef = surface0->getCoefficients();
	for (unsigned int i = 0; i < Ncoef0; i++)
		Coef0[i] = pCoef[i];
	pCoef = surface1->getCoefficients();
	for (unsigned int i = 0; i < Ncoef1; i++)
		Coef1[i] = pCoef[i];

	AddBind(std::make_unique<VertexBuffer>(gfx, Vertexs, N));

	AddBind(std::make_unique<IndexBuffer>(gfx, FourierSurface::getTrianglesIcosphere(), 3 * FourierSurface::getNtriangles()));

	auto pvs = (VertexShader*)AddBind(std::move(std::make_unique<VertexShader>(gfx, SHADERS_DIR L"InterpolatedVS.cso")));

	AddBind(std::make_unique<PixelShader>(gfx, SHADERS_DIR L"InterpolatedPS.cso"));

	D3D11_INPUT_ELEMENT_DESC ied[4] =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Dylm0a",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Dylm1a",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Color",0,DXGI_FORMAT_B8G8R8A8_UNORM,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};

	AddBind(std::make_unique<InputLayout>(gfx, ied, 4u, pvs->GetBytecode()));

	AddBind(std::make_unique<Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_unique<Rasterizer>(gfx, true));

	pVSCB = (ConstantBuffer<VSconstBuffer>*)AddBind(std::make_unique<ConstantBuffer<VSconstBuffer>>(gfx, VERTEX_CONSTANT_BUFFER_TYPE));

	pscBuff.lightsource[0] = { { 600.f, 320.f, 0.f, 0.f } , { 1.f, 1.f, 1.f, 1.f } , { 30.f, 10.f, 20.f, 0.f } };

	pPSCB = (ConstantBuffer<PSconstBuffer>*)AddBind(std::make_unique<ConstantBuffer<PSconstBuffer>>(gfx, pscBuff, PIXEL_CONSTANT_BUFFER_TYPE));

	// Create curves

	unsigned int L = 0;
	for (unsigned int i = 0; i < Ncoef0; i++)
	{
		if (Coef0[i].L > L) L = Coef0[i].L;
	}
	for (unsigned int i = 0; i < Ncoef1; i++)
	{
		if (Coef1[i].L > L) L = Coef1[i].L;
	}
	Coefficient* Coef = (Coefficient*)calloc((L + 1) * (L + 1), sizeof(Coefficient));

	for (unsigned int l = 0; l <= L; l++)
	{
		for (int m = -int(l); m <= int(l); m++)
		{
			Coef[l * l + m + l].L = l;
			Coef[l * l + m + l].M = m;
			Coef[l * l + m + l].C = 0.f;

			for (unsigned int i = 0; i < Ncoef0; i++)
			{
				if (Coef0[i].L == l && Coef0[i].M == m) Coef[i].C += (1 - T) * Coef0[i].C;
			}
			for (unsigned int i = 0; i < Ncoef1; i++)
			{
				if (Coef1[i].L == l && Coef1[i].M == m) Coef[i].C += T * Coef1[i].C;
			}
		}
	}

	curves.create(gfx, Coef, (L + 1) * (L + 1), 0.f, 0.f, NULL);
	free(Coef);
}

void Interpolated::create(Graphics& gfx, Interpolated* interpolation0, Interpolated* interpolation1, int s0, int s1, float t)
{
	if (isInit)
		throw std::exception("You cannot create a surface over one that is already initialized");
	else
		isInit = true;

	while (t > 1)t--;
	while (t < 0)t++;
	vscBuff.tvalue.x = t;

	Interpolated::InterpolatedVertex* V0 = interpolation0->getVertexPtr();
	Interpolated::InterpolatedVertex* V1 = interpolation1->getVertexPtr();
	unsigned int N = FourierSurface::getNvertexs();

	Vertexs = (InterpolatedVertex*)calloc(N, sizeof(InterpolatedVertex));

	for (unsigned int i = 0; i < N; i++)
	{
		Vertexs[i].vector = V0[i].vector;

		if(s0)
			Vertexs[i].dYlm0 = V0[i].dYlm1;
		else
			Vertexs[i].dYlm0 = V0[i].dYlm0;
		if(s1)
			Vertexs[i].dYlm1 = V1[i].dYlm1;
		else
			Vertexs[i].dYlm1 = V1[i].dYlm0;

		Vertexs[i].color = Color::White;
	}

	Ncoef0 = interpolation0->getNcoefficients(s0);
	Ncoef1 = interpolation1->getNcoefficients(s1);

	Coef0 = (Coefficient*)calloc(Ncoef0, sizeof(Coefficient));
	Coef1 = (Coefficient*)calloc(Ncoef1, sizeof(Coefficient));

	Coefficient* pCoef = interpolation0->getCoefficients(s0);
	for (unsigned int i = 0; i < Ncoef0; i++)
		Coef0[i] = pCoef[i];
	pCoef = interpolation1->getCoefficients(s1);
	for (unsigned int i = 0; i < Ncoef1; i++)
		Coef1[i] = pCoef[i];

	AddBind(std::make_unique<VertexBuffer>(gfx, Vertexs, N));

	AddBind(std::make_unique<IndexBuffer>(gfx, FourierSurface::getTrianglesIcosphere(), 3 * FourierSurface::getNtriangles()));

	auto pvs = (VertexShader*)AddBind(std::move(std::make_unique<VertexShader>(gfx, SHADERS_DIR L"InterpolatedVS.cso")));

	AddBind(std::make_unique<PixelShader>(gfx, SHADERS_DIR L"InterpolatedPS.cso"));

	D3D11_INPUT_ELEMENT_DESC ied[4] =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Dylm0a",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Dylm1a",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Color",0,DXGI_FORMAT_B8G8R8A8_UNORM,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};

	AddBind(std::make_unique<InputLayout>(gfx, ied, 4u, pvs->GetBytecode()));

	AddBind(std::make_unique<Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_unique<Rasterizer>(gfx, true));

	pVSCB = (ConstantBuffer<VSconstBuffer>*)AddBind(std::make_unique<ConstantBuffer<VSconstBuffer>>(gfx, VERTEX_CONSTANT_BUFFER_TYPE));

	pscBuff.lightsource[0] = { { 600.f, 320.f, 0.f, 0.f } , { 1.f, 1.f, 1.f, 1.f } , { 30.f, 10.f, 20.f, 0.f } };

	pPSCB = (ConstantBuffer<PSconstBuffer>*)AddBind(std::make_unique<ConstantBuffer<PSconstBuffer>>(gfx, pscBuff, PIXEL_CONSTANT_BUFFER_TYPE));

	// Create curves

	unsigned int L = 0;
	for (unsigned int i = 0; i < Ncoef0; i++)
	{
		if (Coef0[i].L > L) L = Coef0[i].L;
	}
	for (unsigned int i = 0; i < Ncoef1; i++)
	{
		if (Coef1[i].L > L) L = Coef1[i].L;
	}
	Coefficient* Coef = (Coefficient*)calloc((L + 1) * (L + 1), sizeof(Coefficient));

	for (unsigned int l = 0; l <= L; l++)
	{
		for (int m = -int(l); m <= int(l); m++)
		{
			Coef[l * l + m + l].L = l;
			Coef[l * l + m + l].M = m;
			Coef[l * l + m + l].C = 0.f;

			for (unsigned int i = 0; i < Ncoef0; i++)
			{
				if (Coef0[i].L == l && Coef0[i].M == m) Coef[i].C += (1 - T) * Coef0[i].C;
			}
			for (unsigned int i = 0; i < Ncoef1; i++)
			{
				if (Coef1[i].L == l && Coef1[i].M == m) Coef[i].C += T * Coef1[i].C;
			}
		}
	}

	curves.create(gfx, Coef, (L + 1) * (L + 1), 0.f, 0.f, NULL);
	free(Coef);
}

void Interpolated::create(Graphics& gfx, Interpolated* interpolation0, FourierSurface* surface1, float t)
{
	if (isInit)
		throw std::exception("You cannot create a surface over one that is already initialized");
	else
		isInit = true;

	while (t > 1)t--;
	while (t < 0)t++;
	vscBuff.tvalue.x = t;

	Interpolated::InterpolatedVertex* V0 = interpolation0->getVertexPtr();
	FourierSurface::Vertex* V1 = surface1->getVertexPtr();
	unsigned int N = FourierSurface::getNvertexs();

	Vertexs = (InterpolatedVertex*)calloc(N, sizeof(InterpolatedVertex));

	for (unsigned int i = 0; i < N; i++)
	{
		Vertexs[i].vector = V0[i].vector;
		Vertexs[i].dYlm0 = V0[i].dYlm1;
		Vertexs[i].dYlm1 = V1[i].dYlm;
		Vertexs[i].color = Color::White;
	}

	Ncoef0 = interpolation0->getNcoefficients(1);
	Ncoef1 = surface1->getNcoefficients();
	Coef0 = (Coefficient*)calloc(Ncoef0, sizeof(Coefficient));
	Coef1 = (Coefficient*)calloc(Ncoef1, sizeof(Coefficient));

	Coefficient* pCoef = interpolation0->getCoefficients(1);
	for (unsigned int i = 0; i < Ncoef0; i++)
		Coef0[i] = pCoef[i];
	pCoef = surface1->getCoefficients();
	for (unsigned int i = 0; i < Ncoef1; i++)
		Coef1[i] = pCoef[i];

	AddBind(std::make_unique<VertexBuffer>(gfx, Vertexs, N));

	AddBind(std::make_unique<IndexBuffer>(gfx, FourierSurface::getTrianglesIcosphere(), 3 * FourierSurface::getNtriangles()));

	auto pvs = (VertexShader*)AddBind(std::move(std::make_unique<VertexShader>(gfx, SHADERS_DIR L"InterpolatedVS.cso")));

	AddBind(std::make_unique<PixelShader>(gfx, SHADERS_DIR L"InterpolatedPS.cso"));

	D3D11_INPUT_ELEMENT_DESC ied[4] =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Dylm0a",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Dylm1a",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Color",0,DXGI_FORMAT_B8G8R8A8_UNORM,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};

	AddBind(std::make_unique<InputLayout>(gfx, ied, 4u, pvs->GetBytecode()));

	AddBind(std::make_unique<Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_unique<Rasterizer>(gfx, true));

	pVSCB = (ConstantBuffer<VSconstBuffer>*)AddBind(std::make_unique<ConstantBuffer<VSconstBuffer>>(gfx, VERTEX_CONSTANT_BUFFER_TYPE));

	pscBuff.lightsource[0] = { { 600.f, 320.f, 0.f, 0.f } , { 1.f, 1.f, 1.f, 1.f } , { 30.f, 10.f, 20.f, 0.f } };

	pPSCB = (ConstantBuffer<PSconstBuffer>*)AddBind(std::make_unique<ConstantBuffer<PSconstBuffer>>(gfx, pscBuff, PIXEL_CONSTANT_BUFFER_TYPE));

	// Create curves

	unsigned int L = 0;
	for (unsigned int i = 0; i < Ncoef0; i++)
	{
		if (Coef0[i].L > L) L = Coef0[i].L;
	}
	for (unsigned int i = 0; i < Ncoef1; i++)
	{
		if (Coef1[i].L > L) L = Coef1[i].L;
	}
	Coefficient* Coef = (Coefficient*)calloc((L + 1) * (L + 1), sizeof(Coefficient));

	for (unsigned int l = 0; l <= L; l++)
	{
		for (int m = -int(l); m <= int(l); m++)
		{
			Coef[l * l + m + l].L = l;
			Coef[l * l + m + l].M = m;
			Coef[l * l + m + l].C = 0.f;

			for (unsigned int i = 0; i < Ncoef0; i++)
			{
				if (Coef0[i].L == l && Coef0[i].M == m) Coef[i].C += (1 - T) * Coef0[i].C;
			}
			for (unsigned int i = 0; i < Ncoef1; i++)
			{
				if (Coef1[i].L == l && Coef1[i].M == m) Coef[i].C += T * Coef1[i].C;
			}
		}
	}

	curves.create(gfx, Coef, (L + 1) * (L + 1), 0.f, 0.f, NULL);
	free(Coef);
}

void Interpolated::updateShape(Graphics& gfx, FourierSurface* surface0, FourierSurface* surface1, float t)
{
	if (!isInit)
		throw std::exception("You cannot update shape to an uninitialized surface");

	while (t > 1)t--;
	while (t < 0)t++;
	vscBuff.tvalue.x = t;

	FourierSurface::Vertex* V0 = surface0->getVertexPtr();
	FourierSurface::Vertex* V1 = surface1->getVertexPtr();
	unsigned int N = FourierSurface::getNvertexs();

	for (unsigned int i = 0; i < N; i++)
	{
		Vertexs[i].vector = V0[i].vector;
		Vertexs[i].dYlm0 = V0[i].dYlm;
		Vertexs[i].dYlm1 = V1[i].dYlm;
		Vertexs[i].color = Color::White;
	}

	free(Coef0);
	free(Coef1);
	Ncoef0 = surface0->getNcoefficients();
	Ncoef1 = surface1->getNcoefficients();
	Coef0 = (Coefficient*)calloc(Ncoef0, sizeof(Coefficient));
	Coef1 = (Coefficient*)calloc(Ncoef1, sizeof(Coefficient));

	Coefficient* pCoef = surface0->getCoefficients();
	for (unsigned int i = 0; i < Ncoef0; i++)
		Coef0[i] = pCoef[i];
	pCoef = surface1->getCoefficients();
	for (unsigned int i = 0; i < Ncoef1; i++)
		Coef1[i] = pCoef[i];

	changeBind(std::make_unique<VertexBuffer>(gfx, Vertexs, N), 0u);

	pVSCB->Update(gfx, vscBuff);
	

}

void Interpolated::updateInterpolation(Graphics& gfx, float t)
{
	if (!isInit)
		throw std::exception("You cannot update screen position to an uninitialized surface");

	while (t > 1)t--;
	while (t < 0)t++;
	vscBuff.tvalue.x = t;
	T = t;

	pVSCB->Update(gfx, vscBuff);
}

void Interpolated::saveCoefficients(const char* filename)
{
	unsigned int L = 0;
	for (unsigned int i = 0; i < Ncoef0; i++)
	{
		if (Coef0[i].L > L) L = Coef0[i].L;
	}
	for (unsigned int i = 0; i < Ncoef1; i++)
	{
		if (Coef1[i].L > L) L = Coef1[i].L;
	}
	Coefficient* Coef = (Coefficient*)calloc((L + 1) * (L + 1), sizeof(Coefficient));

	for (unsigned int l = 0; l <= L; l++)
	{
		for (int m = -int(l); m <= int(l); m++)
		{
			Coef[l * l + m + l].L = l;
			Coef[l * l + m + l].M = m;
			Coef[l * l + m + l].C = 0.f;
		
			for (unsigned int i = 0; i < Ncoef0; i++)
			{
				if (Coef0[i].L == l && Coef0[i].M == m) Coef[i].C += (1 - T) * Coef0[i].C;
			}
			for (unsigned int i = 0; i < Ncoef1; i++)
			{
				if (Coef1[i].L == l && Coef1[i].M == m) Coef[i].C += T * Coef1[i].C;
			}
		}
	}

	FourierSurface::FileManager::saveCoefficients(Coef, (L + 1) * (L + 1), filename);
}

void Interpolated::saveInterpolation(const char* filename)
{
}

void Interpolated::updateLight(Graphics& gfx, UINT id, Vector2f intensity, Color color, Vector3f position)
{
	if (!isInit)
		throw std::exception("You cannot update light to an uninitialized surface");

	pscBuff.lightsource[id] = { intensity.getVector4() , color.getColor4() , position.getVector4() };
	pPSCB->Update(gfx, pscBuff);
}

void Interpolated::updateLight(Graphics& gfx, UINT id, _float4vector intensity, _float4color color, _float4vector position)
{
	if (!isInit)
		throw std::exception("You cannot update light to an uninitialized surface");

	pscBuff.lightsource[id] = { intensity , color , position };
	pPSCB->Update(gfx, pscBuff);
}

void Interpolated::clearLights(Graphics& gfx)
{
	if (!isInit)
		throw std::exception("You cannot update clear lights to an uninitialized surface");

	pscBuff = {};
	pPSCB->Update(gfx, pscBuff);
}

void Interpolated::updateTexture(Graphics& gfx, Color color, bool def, bool random)
{
	if (!isInit)
		throw std::exception("You cannot update texture to an uninitialized surface");

	for (unsigned int i = 0; i < FourierSurface::getNvertexs(); i++)
	{
		if (def)
			Vertexs[i].color = Color::White;
		else if (random)
			Vertexs[i].color = Color(rand() % 256, rand() % 256, rand() % 256);
		else
			Vertexs[i].color = color;
	}

	changeBind(std::make_unique<VertexBuffer>(gfx, Vertexs, FourierSurface::getNvertexs()), 0u);
}

void Interpolated::updateRotation(Graphics& gfx, Vector3f axis, float angle, bool multiplicative)
{
	if (!isInit)
		throw std::exception("You cannot update rotation to an uninitialized surface");

	if (!multiplicative)
		vscBuff.rotation = rotationQuaternion(axis, angle);
	else
		vscBuff.rotation *= rotationQuaternion(axis, angle);

	vscBuff.rotation.normalize();
	pVSCB->Update(gfx, vscBuff);

	curves.updateRotation(gfx, rotationQuaternion(axis, angle), multiplicative);
}

void Interpolated::updateRotation(Graphics& gfx, Quaternion rotation, bool multiplicative)
{
	if (!isInit)
		throw std::exception("You cannot update rotation to an uninitialized surface");

	if (!multiplicative)
		vscBuff.rotation = rotation;
	else
		vscBuff.rotation *= rotation;

	vscBuff.rotation.normalize();
	pVSCB->Update(gfx, vscBuff);

	curves.updateRotation(gfx, rotation, multiplicative);
}

void Interpolated::updateScreenPosition(Graphics& gfx, Vector2f screenDisplacement)
{
	if (!isInit)
		throw std::exception("You cannot update screen position to an uninitialized surface");

	vscBuff.screenDisplacement = screenDisplacement.getVector4();

	pVSCB->Update(gfx, vscBuff);

	curves.updateScreenPosition(gfx, screenDisplacement);
}

void Interpolated::updateCurves(Graphics& gfx, float phi, float theta)
{
	unsigned int L = 0;
	for (unsigned int i = 0; i < Ncoef0; i++)
	{
		if (Coef0[i].L > L) L = Coef0[i].L;
	}
	for (unsigned int i = 0; i < Ncoef1; i++)
	{
		if (Coef1[i].L > L) L = Coef1[i].L;
	}
	Coefficient* Coef = (Coefficient*)calloc((L + 1) * (L + 1), sizeof(Coefficient));

	for (unsigned int l = 0; l <= L; l++)
	{
		for (int m = -int(l); m <= int(l); m++)
		{
			Coef[l * l + m + l].L = l;
			Coef[l * l + m + l].M = m;
			Coef[l * l + m + l].C = 0.f;

			for (unsigned int i = 0; i < Ncoef0; i++)
			{
				if (Coef0[i].L == l && Coef0[i].M == m) Coef[i].C += (1 - T) * Coef0[i].C;
			}
			for (unsigned int i = 0; i < Ncoef1; i++)
			{
				if (Coef1[i].L == l && Coef1[i].M == m) Coef[i].C += T * Coef1[i].C;
			}
		}
	}

	curves.updateShape(gfx, Coef, (L + 1) * (L + 1), phi, theta);
	free(Coef);
}

Quaternion Interpolated::getRotation()
{
	return vscBuff.rotation;
}

Interpolated::InterpolatedVertex* Interpolated::getVertexPtr()
{
	return Vertexs;
}

unsigned int Interpolated::getNcoefficients(int i)
{
	if (i == 0)
		return Ncoef0;
	if (i == 1)
		return Ncoef1;
	return 0;
}

Coefficient* Interpolated::getCoefficients(int i)
{
	if (i == 0)
		return Coef0;
	if (i == 1)
		return Coef1;
	return NULL;
}

void Interpolated::DrawCurves(Graphics& gfx)
{
	curves.Draw(gfx);
}

//	Interpolated String Functions

InterpolatedString::~InterpolatedString()
{
	if (!Nsurfaces)
		return;
	for (unsigned int i = 0; i < Nsurfaces - 1; i++)
	{
		Interpolations[i]->~Interpolated();
		free(Interpolations[i]);
	}
	if (Nsurfaces == 1u)
	{
		Interpolations[0]->~Interpolated();
		free(Interpolations[0]);
	}

	if (Interpolations)
		free(Interpolations);
}

InterpolatedString::InterpolatedString(Graphics& gfx, FourierSurface** surfaces, unsigned int nsurfaces, float t)
{
	create(gfx, surfaces, nsurfaces, t);
}

void InterpolatedString::create(Graphics& gfx, FourierSurface** surfaces, unsigned int nsurfaces, float t)
{
	if (Interpolations)
	{
		Interpolations[0]->~Interpolated();
		for (unsigned int i = 1; i < Nsurfaces - 1; i++)
			Interpolations[i]->~Interpolated();
		free(Interpolations);
	}
	
	Nsurfaces = nsurfaces;
	Interpolations = NULL;
	T = 0.f;

	if (!nsurfaces) return;
	if (nsurfaces == 1u) return;

	while (t > float(nsurfaces) - 1.f) t -= nsurfaces - 1;
	while (t < 0.f) t += nsurfaces - 1;
	T = t;

	Interpolations = (Interpolated**)calloc(nsurfaces - 1u, sizeof(void*));
	for (unsigned int i = 0; i < nsurfaces - 1u; i++)
	{
		Interpolations[i] = new(Interpolated);
		Interpolations[i]->create(gfx, surfaces[i], surfaces[i + 1]);
		if (t >= i && t < i + 1)
			Interpolations[i]->updateInterpolation(gfx, t - i);
	}

	if (Nsurfaces == 1)
	{
		Interpolations = (Interpolated**)calloc(1, sizeof(void*));
		Interpolations[0] = new(Interpolated);
		Interpolations[0]->create(gfx, surfaces[0], surfaces[0]);
	}
}

void InterpolatedString::addSurface(Graphics& gfx, FourierSurface* surface)
{
	Nsurfaces++;

	if (Nsurfaces == 1u)
	{
		Interpolations = (Interpolated**)calloc(1, sizeof(void*));
		Interpolations[0] = new(Interpolated);
		Interpolations[0]->create(gfx, surface, surface);
		return;
	}

	if (Nsurfaces == 2u)
	{
		Interpolated* temp = new(Interpolated);
		temp->create(gfx, Interpolations[0], surface);
		Interpolations[0]->~Interpolated();
		free(Interpolations[0]);
		Interpolations[0] = temp;
		return;
	}

	add1to(Interpolations, Nsurfaces - 2);
	Interpolations[Nsurfaces - 2] = new(Interpolated);
	Interpolations[Nsurfaces - 2]->create(gfx, Interpolations[Nsurfaces - 3], surface);
}

void InterpolatedString::deleteSurface(Graphics& gfx, unsigned int s)
{
	Nsurfaces--;

	if (!Nsurfaces)
	{
		Interpolations[0]->~Interpolated();
		free(Interpolations[0]);
		free(Interpolations);
		Interpolations = NULL;
		return;
	}

	if (Nsurfaces == 1u)
	{
		Interpolated* temp = new(Interpolated);
		temp->create(gfx, Interpolations[0], Interpolations[0], 1 - s, 1 - s);
		Interpolations[0]->~Interpolated();
		free(Interpolations[0]);
		Interpolations[0] = temp;
		return;
	}

	if (!s)
	{
		Interpolations[0]->~Interpolated();
		free(Interpolations[0]);
		for (unsigned int i = 0; i < Nsurfaces - 1u; i++)
			Interpolations[i] = Interpolations[i + 1];
		return;
	}

	if (s == Nsurfaces)
	{
		Interpolations[Nsurfaces - 1u]->~Interpolated();
		free(Interpolations[Nsurfaces - 1u]);
		return;
	}

	Interpolated* temp = new(Interpolated);
	temp->create(gfx, Interpolations[s - 1], Interpolations[s], 0, 1);
	Interpolations[s - 1]->~Interpolated();
	Interpolations[s]->~Interpolated();
	free(Interpolations[s - 1]);
	free(Interpolations[s]);
	Interpolations[s - 1] = temp;
	for (unsigned int i = s; i < Nsurfaces - 1u; i++)
		Interpolations[i] = Interpolations[i + 1];
}

void InterpolatedString::updateInterpolation(Graphics& gfx, float t)
{
	if (!Nsurfaces || Nsurfaces == 1u)
		return;

	while (t > float(Nsurfaces) - 1.f) t -= Nsurfaces - 1;
	while (t < 0.f) t += Nsurfaces - 1;
	T = t;

	for (unsigned int i = 0; i < Nsurfaces - 1u; i++)
	{
		if (t >= i && t <= i + 1)
			Interpolations[i]->updateInterpolation(gfx, t - i);
	}
}

void InterpolatedString::saveCoefficients(const char* filename)
{
	if (!Nsurfaces)
		return;
	if (Nsurfaces == 1)
		return Interpolations[0]->saveCoefficients(filename);

	for (unsigned int i = 0; i < Nsurfaces - 1u; i++)
	{
		if (T >= i && T <= i + 1)
			return Interpolations[i]->saveCoefficients(filename);
	}
}

void InterpolatedString::saveInterpolation(const char* filename)
{
}

void InterpolatedString::updateLight(Graphics& gfx, UINT id, Vector2f intensity, Color color, Vector3f position)
{
	if (!Nsurfaces)return;

	if (Nsurfaces == 1u)
		Interpolations[0]->updateLight(gfx, id, intensity, color, position);

	for (unsigned int i = 0; i < Nsurfaces - 1; i++)
		Interpolations[i]->updateLight(gfx, id, intensity, color, position);
}

void InterpolatedString::updateLight(Graphics& gfx, UINT id, _float4vector intensity, _float4color color, _float4vector position)
{
	if (!Nsurfaces)return;

	if (Nsurfaces == 1u)
		Interpolations[0]->updateLight(gfx, id, intensity, color, position);

	for (unsigned int i = 0; i < Nsurfaces - 1; i++)
		Interpolations[i]->updateLight(gfx, id, intensity, color, position);
}

void InterpolatedString::clearLights(Graphics& gfx)
{
	if (!Nsurfaces)return;

	if (Nsurfaces == 1u)
		Interpolations[0]->clearLights(gfx);

	for (unsigned int i = 0; i < Nsurfaces - 1; i++)
		Interpolations[i]->clearLights(gfx);
}

void InterpolatedString::updateTexture(Graphics& gfx, Color color, bool def, bool random)
{
	if (!Nsurfaces)return;

	if (Nsurfaces == 1u)
		Interpolations[0]->updateTexture(gfx, color, def, random);

	for (unsigned int i = 0; i < Nsurfaces - 1; i++)
		Interpolations[i]->updateTexture(gfx, color, def, random);
}

void InterpolatedString::updateRotation(Graphics& gfx, Vector3f axis, float angle, bool multiplicative)
{
	if (!Nsurfaces)return;

	if (Nsurfaces == 1u)
		Interpolations[0]->updateRotation(gfx, axis, angle, multiplicative);

	for (unsigned int i = 0; i < Nsurfaces - 1; i++)
		Interpolations[i]->updateRotation(gfx, axis, angle, multiplicative);

}

void InterpolatedString::updateRotation(Graphics& gfx, Quaternion rotation, bool multiplicative)
{
	if (!Nsurfaces)return;

	if (Nsurfaces == 1u)
		Interpolations[0]->updateRotation(gfx, rotation, multiplicative);

	for (unsigned int i = 0; i < Nsurfaces - 1; i++)
		Interpolations[i]->updateRotation(gfx, rotation, multiplicative);
}

void InterpolatedString::updateScreenPosition(Graphics& gfx, Vector2f screenDisplacement)
{
	if (!Nsurfaces)return;

	if (Nsurfaces == 1u)
		Interpolations[0]->updateScreenPosition(gfx, screenDisplacement);

	for (unsigned int i = 0; i < Nsurfaces - 1; i++)
		Interpolations[i]->updateScreenPosition(gfx, screenDisplacement);
}

void InterpolatedString::updateCurves(Graphics& gfx, float phi, float theta)
{
	if (!Nsurfaces)
		return;
	if (Nsurfaces == 1)
		return Interpolations[0]->updateCurves(gfx, phi, theta);

	for (unsigned int i = 0; i < Nsurfaces - 1u; i++)
	{
		if (T >= i && T <= i + 1)
			return Interpolations[i]->updateCurves(gfx, phi, theta);
	}
}

Quaternion InterpolatedString::getRotation()
{
	if (Interpolations)
		return Interpolations[0]->getRotation();
	else
		return 0.f;
}

void InterpolatedString::Draw(Graphics& gfx)
{
	if (!Nsurfaces)
		return;
	if (Nsurfaces == 1)
		return Interpolations[0]->Draw(gfx);

	for (unsigned int i = 0; i < Nsurfaces - 1u; i++)
	{
		if (T >= i && T <= i + 1)
			return Interpolations[i]->Draw(gfx);
	}
}

void InterpolatedString::DrawCurves(Graphics& gfx)
{
	if (!Nsurfaces)
		return;
	if (Nsurfaces == 1)
		return Interpolations[0]->DrawCurves(gfx);

	for (unsigned int i = 0; i < Nsurfaces - 1u; i++)
	{
		if (T >= i && T <= i + 1)
			return Interpolations[i]->DrawCurves(gfx);
	}
}

//	Curves functions

//	Private static

void Interpolated::Curves::generatePhiCurveAsync(const unsigned int t0, const unsigned int t1, const Coefficient* coef, const unsigned int ncoef, const float phi, const float theta, Vertex* V)
{
	constexpr float error = 1.0035f;

	for (unsigned int j = t0; j <= t1; j++)
	{
		float iphi = j * 2 * pi / Npoints + phi;
		float Z = 0.f;
		for (unsigned int k = 0; k < ncoef; k++)
			Z += coef[k].C * FourierSurface::Functions::Ylm(coef[k].L, coef[k].M, iphi, theta);

		V[unsigned int(theta * Npoints / pi) + j + 1] = { (error * Z * Vector3f(sinf(theta) * cosf(iphi), sinf(theta) * sinf(iphi), cosf(theta))).getVector4(), { 1.f, 1.f, 1.f, 1.f } };
	}
}

void Interpolated::Curves::generateThetaCurveAsync(const unsigned int t0, const unsigned int t1, const Coefficient* coef, const unsigned int ncoef, const float phi, const float theta, Vertex* V)
{
	constexpr float error = 1.0035f;

	for (unsigned int i = t0; i <= t1; i++)
	{
		float itheta = i * pi / Npoints;
		float Y = 0.f;

		for (unsigned int k = 0; k < ncoef; k++)
			Y += coef[k].C * FourierSurface::Functions::Ylm(coef[k].L, coef[k].M, phi, itheta);

		if (i <= unsigned int(theta * Npoints / pi))
			V[i] = { (error * Y * Vector3f(sinf(itheta) * cosf(phi), sinf(itheta) * sinf(phi), cosf(itheta))).getVector4(), { 1.f, 1.f, 1.f, 1.f } };
		else
			V[i + Npoints + 1] = { (error * Y * Vector3f(sinf(itheta) * cosf(phi), sinf(itheta) * sinf(phi), cosf(itheta))).getVector4(), { 1.f, 1.f, 1.f, 1.f } };
	}
}

//	Construction

void Interpolated::Curves::create(Graphics& gfx, const Coefficient* coef, const unsigned int ncoef, const float phi, const float theta, std::mutex* mtx)
{
	constexpr float error = 1.0035f;
	if (isInit)
		throw std::exception("You cannot create a curve over one that is already initialized");
	else
		isInit = true;

	Vertex* vertexs = (Vertex*)calloc(2 * (Npoints + 1), sizeof(Vertex));

	std::thread worker0 = std::thread(generatePhiCurveAsync, 0u, Npoints / 2u, coef, ncoef, phi, theta, vertexs);
	std::thread worker1 = std::thread(generatePhiCurveAsync, Npoints / 2u + 1, Npoints, coef, ncoef, phi, theta, vertexs);
	std::thread worker2 = std::thread(generateThetaCurveAsync, 0u, Npoints / 2u, coef, ncoef, phi, theta, vertexs);
	std::thread worker3 = std::thread(generateThetaCurveAsync, Npoints / 2u + 1, Npoints, coef, ncoef, phi, theta, vertexs);

	unsigned int* indexs = (unsigned int*)calloc(2 * (Npoints + 1), sizeof(unsigned int));
	for (UINT i = 0; i <= 2 * Npoints + 1; i++)
		indexs[i] = i;

	worker0.join();
	worker1.join();
	worker2.join();
	worker3.join();

	if (mtx)
		mtx->lock();

	AddBind(std::make_unique<VertexBuffer>(gfx, vertexs, 2 * (Npoints + 1)));
	AddBind(std::make_unique<IndexBuffer>(gfx, indexs, 2 * (Npoints + 1)));

	auto pvs = (VertexShader*)AddBind(std::move(std::make_unique<VertexShader>(gfx, SHADERS_DIR L"CurvesVS.cso")));
	AddBind(std::make_unique<PixelShader>(gfx, SHADERS_DIR L"CurvesPS.cso"));

	D3D11_INPUT_ELEMENT_DESC ied[2] =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Color",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};

	AddBind(std::make_unique<InputLayout>(gfx, ied, 2u, pvs->GetBytecode()));
	AddBind(std::make_unique<Topology>(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP));
	AddBind(std::make_unique<Blender>(gfx, false));

	pVSCB = (ConstantBuffer<VSconstBuffer>*)AddBind(std::make_unique<ConstantBuffer<VSconstBuffer>>(gfx, vscBuff, VERTEX_CONSTANT_BUFFER_TYPE));

	free(vertexs);
	free(indexs);

	if (mtx)
		mtx->unlock();
}

void Interpolated::Curves::updateShape(Graphics& gfx, const Coefficient* coef, const unsigned int ncoef, const float phi, const float theta)
{
	constexpr float error = 1.0035f;
	if (!isInit)
		throw std::exception("You cannot update the shape of a curve if you havent initialized it first");

	Vertex* vertexs = (Vertex*)calloc(2 * (Npoints + 1), sizeof(Vertex));

	std::thread worker0 = std::thread(generatePhiCurveAsync, 0u, Npoints / 2u, coef, ncoef, phi, theta, vertexs);
	std::thread worker1 = std::thread(generatePhiCurveAsync, Npoints / 2u + 1, Npoints, coef, ncoef, phi, theta, vertexs);
	std::thread worker2 = std::thread(generateThetaCurveAsync, 0u, Npoints / 2u, coef, ncoef, phi, theta, vertexs);
	std::thread worker3 = std::thread(generateThetaCurveAsync, Npoints / 2u + 1, Npoints, coef, ncoef, phi, theta, vertexs);

	worker0.join();
	worker1.join();
	worker2.join();
	worker3.join();

	changeBind(std::make_unique<VertexBuffer>(gfx, vertexs, 2 * (Npoints + 1)), 0u);

	free(vertexs);
}

//	User friendly

void Interpolated::Curves::updateRotation(Graphics& gfx, Quaternion rotation, bool multiplicative)
{
	if (!multiplicative)
		vscBuff.rotation = rotation;
	else
		vscBuff.rotation *= rotation;

	vscBuff.rotation.normalize();
	pVSCB->Update(gfx, vscBuff);
}

void Interpolated::Curves::updateScreenPosition(Graphics& gfx, Vector2f screenDisplacement)
{
	vscBuff.screenDisplacement = screenDisplacement.getVector4();

	pVSCB->Update(gfx, vscBuff);
}

