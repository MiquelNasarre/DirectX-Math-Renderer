#include "Drawable/Surface.h"
#include "Bindable/BindableBase.h"

#include "Exception/_exDefault.h"

//	Surface shape

SURFACE_SHAPE::SURFACE_SHAPE(SURFACE_TYPE Type, float(*_explicit)(float, float), Vector2f min, Vector2f max, unsigned numX, unsigned numY)
	:Type{ Type }, Explicit{ _explicit }, minRect{ min }, maxRect{ max }, numU{ numX }, numV{ numY }
{
	switch (Type)
	{
	case _EXPLICIT:
		break;
	case _IMPLICIT:
		throw INFO_EXCEPT("The surface type specified is iconrrect for the given constructor arguments\nTo create an Implicit surface you must provide just one function that takes three arguments");
	case _PARAMETRIC:
		throw INFO_EXCEPT("The surface type specified is iconrrect for the given constructor arguments\nTo create a Parametric surface you must provide three functions that take two arguments");
	case _EXPLICIT_ICOSPHERE:
		throw INFO_EXCEPT("The surface type specified is iconrrect for the given constructor arguments\nTo create a Radial Icosphere surface you must specify the depth of the icosphere\n(depth 5 recomended)");
	default:
		throw INFO_EXCEPT("The surface type specified is not supported");
	}
}

SURFACE_SHAPE::SURFACE_SHAPE(SURFACE_TYPE Type, float(*_radius)(float, float), unsigned depth)
	:Type{ Type }, Explicit{ _radius }, ICOSPHERE_DEPHT{ depth }
{
	switch (Type)
	{
	case _EXPLICIT_ICOSPHERE:
		break;
	case _EXPLICIT:
		throw INFO_EXCEPT("The surface type specified is iconrrect for the given constructor arguments\nTo create an Explicit surface you must remove the last value that provides a depth for the icosphere");
	case _IMPLICIT:
		throw INFO_EXCEPT("The surface type specified is iconrrect for the given constructor arguments\nTo create an Implicit surface you must provide just one function that takes three arguments");
	case _PARAMETRIC:
		throw INFO_EXCEPT("The surface type specified is iconrrect for the given constructor arguments\nTo create a Parametric surface you must provide three functions that take two arguments");
	default:
		throw INFO_EXCEPT("The surface type specified is not supported");
	}
}

SURFACE_SHAPE::SURFACE_SHAPE(SURFACE_TYPE Type, Vector3f(*_pv)(float, float), Vector2f min, Vector2f max, unsigned U, unsigned V)
	:Type{ Type }, Parametric_V{ _pv }, minRect{ min }, maxRect{ max }, numU{ U }, numV{ V }
{
	switch (Type)
	{
	case _PARAMETRIC:
		break;
	case _EXPLICIT_ICOSPHERE:
		throw INFO_EXCEPT("The surface type specified is iconrrect for the given constructor arguments\nTo create a Radial Icosphere surface you must provide a single function that take two arguments and a depth value");
	case _EXPLICIT:
		throw INFO_EXCEPT("The surface type specified is iconrrect for the given constructor arguments\nTo create an Explicit surface you must provide a single function that take two arguments");
	case _IMPLICIT:
		throw INFO_EXCEPT("The surface type specified is iconrrect for the given constructor arguments\nTo create an Implicit surface you must provide just one function that takes three arguments");
	default:
		throw INFO_EXCEPT("The surface type specified is not supported");
	}
}

SURFACE_SHAPE::SURFACE_SHAPE(SURFACE_TYPE Type, float(*_imp)(float, float, float), Vector3f min, Vector3f max)
	:Type{ Type }, Implicit{ _imp }, minCube{ min }, maxCube{ max }
{
	switch (Type)
	{
	case _IMPLICIT:
		break;
	case _PARAMETRIC:
		throw INFO_EXCEPT("The surface type specified is iconrrect for the given constructor arguments\nTo create a Parametric surface you must provide three functions that take two arguments");
	case _EXPLICIT_ICOSPHERE:
		throw INFO_EXCEPT("The surface type specified is iconrrect for the given constructor arguments\nTo create a Radial Icosphere surface you must provide a single function that take two arguments and a depth value");
	case _EXPLICIT:
		throw INFO_EXCEPT("The surface type specified is iconrrect for the given constructor arguments\nTo create an Explicit surface you must provide a single function that take two arguments");
	default:
		throw INFO_EXCEPT("The surface type specified is not supported");
	}
}

//	Constructors

Surface::Surface(SURFACE_SHAPE* ss, SURFACE_COLORING* psc)
{
	create(ss, psc);
}

void Surface::create(SURFACE_SHAPE* ss, SURFACE_COLORING* psc)
{
	if (isInit)
		throw INFO_EXCEPT("You cannot create a surface over one that is already initialized");

	else
		isInit = true;

	if (psc != NULL)
		sc = *psc;
	else
		sc = {};

	void** out = getShapeBuffers(ss);

	VertexBuffer* vertex_buff = (VertexBuffer*)out[0];
	IndexBuffer* index_buff = (IndexBuffer*)out[1];
	free(out);

	AddBind(vertex_buff);
	AddBind(index_buff);

	AddBind(new Topology(TRIANGLE_LIST));
	AddBind(new Rasterizer(true));
	AddBind(new Blender(sc.transparency));

	pVSCB = AddBind(new ConstantBuffer(vscBuff, VERTEX_CONSTANT_BUFFER_TYPE));

	pscBuffc.color = sc.color.getColor4();
	pPSCBc = AddBind(new ConstantBuffer(pscBuffc, PIXEL_CONSTANT_BUFFER_TYPE, 1u));

	if (sc.Textured)
	{
		AddBind(new Texture(*sc.texture0));

		if (sc.texture1)
			AddBind(new Texture(*sc.texture1, 1u));
		else
			AddBind(new Texture(*sc.texture0, 1u));

		void* pvs = nullptr;
		if (sc.Lighted)
		{
			pvs = AddBind(new VertexShader(SHADERS_DIR L"TexSurfaceVS.cso"));
			AddBind(new PixelShader(SHADERS_DIR L"TexSurfacePS.cso"));


			float unused = 0.f;
			if (sc.DefaultInitialLights)
				pscBuff = { 32000.f,5000.f,unused,unused,1.f, 1.f, 1.f, 1.f ,160.f, 0.f, 60.f,unused };
			else {
				for (int i = 0; i < 8; i++)
					pscBuff.lightsource[i] = sc.lightsource[i];
			}
			pPSCB = AddBind(new ConstantBuffer(pscBuff, PIXEL_CONSTANT_BUFFER_TYPE));
		}
		else
		{
			pvs = AddBind(new VertexShader(SHADERS_DIR L"UnlitSurfaceVS.cso"));
			AddBind(new PixelShader(SHADERS_DIR L"UnlitSurfacePS.cso"));
		}


		INPUT_ELEMENT_DESC ied[3] =
		{
			{ "Position", _4_FLOAT },
			{ "Normal", _3_FLOAT },
			{ "TexCoord", _2_FLOAT },
		};

		AddBind(new InputLayout(ied, 3u, (VertexShader*)pvs));

		AddBind(new Sampler(SAMPLE_FILTER_LINEAR, SAMPLE_ADDRESS_WRAP));


	}
	else
	{
		void* pvs = nullptr;
		if (sc.Lighted)
		{
			pvs = AddBind(new VertexShader(SHADERS_DIR L"SurfaceVS.cso"));
			AddBind(new PixelShader(SHADERS_DIR L"SurfacePS.cso"));

			float unused = 0.f;
			if (sc.DefaultInitialLights)
				pscBuff = {
					60.f,10.f,unused,unused,1.0f, 0.2f, 0.2f, 1.f , 0.f, 8.f, 8.f,unused,
					60.f,10.f,unused,unused,0.0f, 1.0f, 0.0f, 1.f , 0.f,-8.f, 8.f,unused,
					60.f,10.f,unused,unused,0.5f, 0.0f, 1.0f, 1.f ,-8.f, 0.f,-8.f,unused,
					60.f,10.f,unused,unused,1.0f, 1.0f, 0.0f, 1.f , 8.f, 0.f, 8.f,unused,
			};
			else {
				for (int i = 0; i < 8; i++)
					pscBuff.lightsource[i] = sc.lightsource[i];
			}
			pPSCB = AddBind(new ConstantBuffer(pscBuff, PIXEL_CONSTANT_BUFFER_TYPE));
		}
		else
		{
			pvs = AddBind(new VertexShader(SHADERS_DIR L"UnlitUntexVS.cso"));
			AddBind(new PixelShader(SHADERS_DIR L"UnlitUntexPS.cso"));
		}

		INPUT_ELEMENT_DESC ied[2] =
		{
			{ "Position",_3_FLOAT },
			{ "Normal",_3_FLOAT },
		};

		AddBind(new InputLayout(ied, 2u, (VertexShader*)pvs));
	}
}

//	Public

void Surface::updateRotation(Quaternion rotation, bool multiplicative)
{
	if (!isInit)
		throw INFO_EXCEPT("You cannot update the rotation on an uninitialized surface");

	if (!multiplicative)
		vscBuff.rotation = rotation;
	else
		vscBuff.rotation *= rotation;

	vscBuff.rotation.normalize();
	((ConstantBuffer*)pVSCB)->Update(vscBuff);
}

void Surface::updatePosition(Vector3f position, bool additive)
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

void Surface::updateTexture(unsigned id, Image& image)
{
	if (!sc.Textured)
		throw INFO_EXCEPT("You cannot call a texture update in a surface that wasn't initialized as textured");
	if (id > 1u)
		throw INFO_EXCEPT("The given id to update the texture is not valid, id must be 0 or 1");

	((Texture*)changeBind(new Texture(image), id + 2))->setSlot(id);
}

void Surface::updateTextures(Image& image0, Image& image1)
{
	if (!sc.Textured)
		throw INFO_EXCEPT("You cannot call a texture update in a surface that wasn't initialized as textured");

	changeBind(new Texture(image0, 0u), 4u);
	changeBind(new Texture(image1, 1u), 5u);
}

void Surface::updateLight(unsigned id, Vector2f intensity, Color color, Vector3f position)
{
	if(!pPSCB)
		throw INFO_EXCEPT("You cannot call a light update in a surface that wasn't initialized as lighted");

	pscBuff.lightsource[id] = { intensity.getVector4() , color.getColor4() , position.getVector4() };
	((ConstantBuffer*)pPSCB)->Update(pscBuff);
}

void Surface::clearLights()
{
	pscBuff = {};
	((ConstantBuffer*)pPSCB)->Update(pscBuff);
}

void Surface::updateShape(SURFACE_SHAPE* ss)
{
	void** out = getShapeBuffers(ss);

	VertexBuffer* vertex_buff = (VertexBuffer*)out[0];
	IndexBuffer* index_buff = (IndexBuffer*)out[1];
	free(out);

	changeBind(vertex_buff, 0u);
	changeBind(index_buff, 1u);
}

void Surface::updateColor(Color color)
{
	if (!isInit)
		throw INFO_EXCEPT("You cannot update the color in a surface that hasn't been initialized");

	sc.color = color;
	pscBuffc.color = color.getColor4();

	((ConstantBuffer*)pPSCBc)->Update(pscBuffc);
}

Quaternion Surface::getRotation()
{
	return vscBuff.rotation;
}

Vector3f Surface::getPosition()
{
	return Vector3f(vscBuff.translation.x, vscBuff.translation.y, vscBuff.translation.z);
}

//	Private

void** Surface::getShapeBuffers(SURFACE_SHAPE* ss)
{
	void* vertex_buff = nullptr;
	void* index_buff = nullptr;

	switch (ss->Type)
	{
	case _EXPLICIT:

		if (ss->Explicit)
		{
			float epsilon = error_epsilon;
			auto numX = ss->numU;
			auto numY = ss->numV;
			auto minRect = ss->minRect;
			auto maxRect = ss->maxRect;
			auto F = ss->Explicit;

			if (!sc.Textured) {
				VertexArr vertexs((numX + 1) * (numY + 1));

				for (unsigned i = 0; i < numX + 1; i++) {
					for (unsigned j = 0; j < numY + 1; j++) {
						float x = ((numX - i) * minRect.x + i * maxRect.x) / numX;
						float y = ((numY - j) * minRect.y + j * maxRect.y) / numY;
						vertexs.push_back(Vector3f(x, y, F(x, y)),
							((Vector3f(x + epsilon, y, F(x + epsilon, y)) - Vector3f(x - epsilon, y, F(x - epsilon, y))) *
								(Vector3f(x, y + epsilon, F(x, y + epsilon)) - Vector3f(x, y - epsilon, F(x, y - epsilon)))).normalize()
						);
					}
				}
				vertex_buff = new VertexBuffer(vertexs.data, vertexs.counter);
			}

			else {
				TexVertexArr vertexs((numX + 1) * (numY + 1));

				for (unsigned i = 0; i < numX + 1; i++) {
					for (unsigned j = 0; j < numY + 1; j++) {
						float x = ((numX - i) * minRect.x + i * maxRect.x) / numX;
						float y = ((numY - j) * minRect.y + j * maxRect.y) / numY;
						vertexs.push_back(Vector3f(x, y, F(x, y)),
							((Vector3f(x + epsilon, y, F(x + epsilon, y)) - Vector3f(x - epsilon, y, F(x - epsilon, y))) *
								(Vector3f(x, y + epsilon, F(x, y + epsilon)) - Vector3f(x, y - epsilon, F(x, y - epsilon)))).normalize(),
							Vector2f((float)i / numX, (float)j / numY)
						);
					}
				}
				vertex_buff = new VertexBuffer(vertexs.data, vertexs.counter);
			}

			IndexArr indexs(6 * numX * numY);

			for (unsigned i = 0; i < numX; i++) {
				for (unsigned j = 0; j < numY; j++) {
					indexs.push_back(i * (numY + 1) + j);
					indexs.push_back(i * (numY + 1) + j + 1);
					indexs.push_back((i + 1) * (numY + 1) + j + 1);

					indexs.push_back(i * (numY + 1) + j);
					indexs.push_back((i + 1) * (numY + 1) + j + 1);
					indexs.push_back((i + 1) * (numY + 1) + j);
				}
			}

			index_buff = new IndexBuffer(indexs.data, indexs.counter);
		}

		else throw INFO_EXCEPT("Found nullptr while trying to read Explicit function");

		break;
	case _EXPLICIT_ICOSPHERE:

		if (ss->Explicit)
		{
			float epsilon = error_epsilon;
			auto depth = ss->ICOSPHERE_DEPHT;
			auto r = ss->Explicit;

			void** icosphere = generateIcosphere(ss->ICOSPHERE_DEPHT);
			unsigned int nV = ((unsigned int*)icosphere[2])[0];

			Vertex* vertexs = (Vertex*)calloc(nV, sizeof(Vertex));
			Vector2f* texCoord = (Vector2f*)calloc(nV, sizeof(Vector2f));

			for (unsigned int i = 0; i < nV; i++) {
				Vertex& v = vertexs[i];
				float phi = asinf(v.vector.z);
				float theta = 0.f;
				if (v.vector.x || v.vector.y) {
					Vector2f temp = Vector2f(v.vector.x, v.vector.y).normalize();
					theta = acosf(temp.x);
					if (temp.y < 0)
						theta = 2 * MATH_PI - theta;
				}
				if (!v.vector.x && !v.vector.y)
					v.norm = v.vector;
				else
					v.norm =
					((makePolar({ theta + epsilon, phi, r(theta + epsilon, phi) }) - makePolar({ theta - epsilon, phi, r(theta - epsilon, phi) })) *
						(makePolar({ theta, phi + epsilon, r(theta, phi + epsilon) }) - makePolar({ theta, phi - epsilon, r(theta, phi - epsilon) })))
					.normalize();

				v.vector *= r(theta, phi);
				texCoord[i] = { theta / 2.f / MATH_PI, (MATH_PI / 2.f - phi) / MATH_PI };
			}

			if (sc.Textured) {
				TexVertex* texvertexs = (TexVertex*)calloc(nV, sizeof(TexVertex));
				for (unsigned i = 0; i < nV; i++)
					texvertexs[i] = { vertexs[i].vector,vertexs[i].norm,texCoord[i] };
				vertex_buff = new VertexBuffer(texvertexs, nV);
			}
			else
				vertex_buff = new VertexBuffer(vertexs, nV);

			index_buff = new IndexBuffer((unsigned int*)icosphere[1], ((unsigned int*)icosphere[2])[1]);

			free(icosphere[0]);
			free(icosphere[1]);
			free(icosphere[2]);
			free(icosphere);
		}
		else throw INFO_EXCEPT("Found nullptr while trying to read Explicit function");
		break;
	case _IMPLICIT:

		if (ss->Implicit)
		{
			auto H = ss->Implicit;
			auto regionBegin = ss->minCube;
			auto regionEnd = ss->maxCube;

			constexpr int cubes = 60;
			float minx = regionBegin.x;
			float maxx = regionEnd.x;
			float miny = regionBegin.y;
			float maxy = regionEnd.y;
			float minz = regionBegin.z;
			float maxz = regionEnd.z;

			_float4vector*** h = (_float4vector***)calloc(cubes + 1, sizeof(void**));

			for (int i = 0; i <= cubes; i++)
			{
				h[i] = (_float4vector**)calloc(cubes + 1, sizeof(void*));

				for (int j = 0; j <= cubes; j++)
				{
					h[i][j] = (_float4vector*)calloc(cubes + 1, sizeof(_float4vector));

					for (int k = 0; k <= cubes; k++)
					{
						float di = float(i) / cubes;
						float dj = float(j) / cubes;
						float dk = float(k) / cubes;

						float x = minx * (1 - di) + maxx * di;
						float y = miny * (1 - dj) + maxy * dj;
						float z = minz * (1 - dk) + maxz * dk;
						h[i][j][k] = { x , y , z , H(x, y, z) };
					}
				}
			}

			VertexArr vertexs(cubes * cubes * cubes * 8u);
			IndexArr indexs(cubes * cubes * cubes * 8u);

			for (int i = 0; i < cubes; i++)
			{
				for (int j = 0; j < cubes; j++)
				{
					for (int k = 0; k < cubes; k++)
					{
						_float4vector cube[8] = { h[i][j][k] , h[i][j][k + 1] , h[i][j + 1][k + 1] , h[i][j + 1][k] , h[i + 1][j][k] , h[i + 1][j][k + 1] , h[i + 1][j + 1][k + 1] , h[i + 1][j + 1][k] };

						addVertexsCube(cube, vertexs, indexs);
					}
				}
			}

			for (int i = 0; i <= cubes; i++)
			{
				for (int j = 0; j <= cubes; j++)
				{
					free(h[i][j]);
				}
				free(h[i]);
			}
			free(h);

			vertex_buff = new VertexBuffer(vertexs.data, vertexs.counter);
			index_buff = new IndexBuffer(indexs.data, indexs.counter);
		}

		else throw INFO_EXCEPT("Found nullptr while trying to read Implicit function");
		break;
	case _PARAMETRIC:

		if (ss->Parametric_V)
		{
			float epsilon = error_epsilon;
			auto numU = ss->numU;
			auto numV = ss->numV;
			auto minRect = ss->minRect;
			auto maxRect = ss->maxRect;
			auto P = ss->Parametric_V;

			if (!sc.Textured) {
				VertexArr vertexs((numU + 1) * (numV + 1));

				for (unsigned i = 0; i < numU + 1; i++) {
					for (unsigned j = 0; j < numV + 1; j++) {
						float u = ((numU - i) * minRect.x + i * maxRect.x) / numU;
						float v = ((numV - j) * minRect.y + j * maxRect.y) / numV;
						vertexs.push_back(
							P(u, v),
							-((P(u + epsilon, v) - P(u - epsilon, v)) *
								(P(u, v + epsilon) - P(u, v - epsilon))).normalize()
						);
					}
				}
				vertex_buff = new VertexBuffer(vertexs.data, vertexs.counter);
			}
			else {
				TexVertexArr vertexs((numU + 1) * (numV + 1));

				for (unsigned i = 0; i < numU + 1; i++) {
					for (unsigned j = 0; j < numV + 1; j++) {
						float u = ((numU - i) * minRect.x + i * maxRect.x) / numU;
						float v = ((numV - j) * minRect.y + j * maxRect.y) / numV;
						vertexs.push_back(
							P(u, v),
							-((P(u + epsilon, v) - P(u - epsilon, v)) *
								(P(u, v + epsilon) - P(u, v - epsilon))).normalize(),
							Vector2f((float)i / numU, (float)j / numV)
						);
					}
				}
				vertex_buff = new VertexBuffer(vertexs.data, vertexs.counter);
			}

			IndexArr indexs(6 * numU * numV);

			for (unsigned i = 0; i < numU; i++) {
				for (unsigned j = 0; j < numV; j++) {
					indexs.push_back(i * (numV + 1) + j);
					indexs.push_back(i * (numV + 1) + j + 1);
					indexs.push_back((i + 1) * (numV + 1) + j + 1);

					indexs.push_back(i * (numV + 1) + j);
					indexs.push_back((i + 1) * (numV + 1) + j + 1);
					indexs.push_back((i + 1) * (numV + 1) + j);
				}
			}

			index_buff = new IndexBuffer(indexs.data, indexs.counter);
		}

		else throw INFO_EXCEPT("Found nullptr while trying to read Parametric functions");
		break;
	default:
		throw INFO_EXCEPT("The surface type specified is not supported");
	}

	void** out = (void**)calloc(2, sizeof(void*));
	out[0] = vertex_buff;
	out[1] = index_buff;

	return out;
}

void** Surface::generateIcosphere(unsigned int depth)
{
	void** returnptr = (void**)calloc(3, sizeof(void*));
	returnptr[2] = calloc(2, sizeof(unsigned int));

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

	returnptr[1] = calloc(3 * C, sizeof(unsigned int));
	unsigned int* trianglesIcosphere = (unsigned int*)returnptr[1];

	for (unsigned int i = 0; i < C; i++)
	{
		unsigned int aris0 = abs(triangles[i].a0) - 1;
		bool ornt0 = (triangles[i].a0 > 0) ? true : false;
		unsigned int aris1 = abs(triangles[i].a1) - 1;
		bool ornt1 = (triangles[i].a1 > 0) ? true : false;
		unsigned int aris2 = abs(triangles[i].a2) - 1;
		bool ornt2 = (triangles[i].a2 > 0) ? true : false;

		trianglesIcosphere[3 * i] = ornt0 ? aristas[aris0].v0 : aristas[aris0].v1;
		trianglesIcosphere[3 * i + 1] = ornt1 ? aristas[aris1].v0 : aristas[aris1].v1;
		trianglesIcosphere[3 * i + 2] = ornt2 ? aristas[aris2].v0 : aristas[aris2].v1;

	}
	returnptr[0] = vertexs;

	((unsigned int*)returnptr[2])[0] = V;
	((unsigned int*)returnptr[2])[1] = C;

	free(aristas);
	free(triangles);

	return returnptr;
}

Vector3f Surface::evalPolar(float r(float, float), float theta, float phi)
{
	return r(theta, phi) * Vector3f(cosf(theta) * cosf(phi), sinf(theta) * cosf(phi), sinf(phi));
}

Vector3f Surface::makePolar(Vector3f other)
{
	return other.z * Vector3f(cosf(other.y) * cosf(other.x), cosf(other.y) * sinf(other.x), sinf(other.y));
}

void Surface::addVertexsCube(_float4vector cube[8], VertexArr& vertexs, IndexArr& indexs, bool polar)
{
	struct triangle {
		unsigned short v0 = 0u;
		unsigned short v1 = 0u;
		unsigned short v2 = 0u;

		triangle() {}
		triangle(unsigned x, unsigned y, unsigned z)
		{
			v0 = unsigned short(x);
			v1 = unsigned short(y);
			v2 = unsigned short(z);
		}
	};

	struct TriangleList
	{
		unsigned size = 0;
		triangle data[4] = {};
		void push_back(triangle t)
		{
			data[3] = data[2];
			data[2] = data[1];
			data[1] = data[0];
			data[0] = t;

			if (size < 4)size++;
		}

	}triangles;

	unsigned vi = vertexs.counter;

	Vector2i cases[] = { {0 , 1} , {0 , 3} , {0 , 4} ,  {1 , 2} ,  {1 , 5} ,  {3 , 2} ,  {3 , 7} ,  {4 , 5} ,  {4 , 7} ,  {2 , 6} ,  {5 , 6} ,  {7 , 6} };

	unsigned char b = 0x0;
	for (int i = 0; i < 8; i++) {
		if (cube[i].w <= 0)
			b |= 0x1 << i;
	}

	for (Vector2i& c : cases) {
		if (((b & (0x1 << c.x)) && !(b & (0x1 << c.y))) || (!(b & (0x1 << c.x)) && (b & (0x1 << c.y))))
		{
			Vector3f p0 = { cube[c.x].x ,cube[c.x].y ,cube[c.x].z };
			Vector3f p1 = { cube[c.y].x ,cube[c.y].y ,cube[c.y].z };
			float w0 = cube[c.x].w;
			float w1 = cube[c.y].w;

			if (polar)
				vertexs.push_back(makePolar((p0 * w1 - p1 * w0) / (w1 - w0)), ((p0 * w0 - p1 * w1) / (w1 - w0)).normalize());
			else
				vertexs.push_back((p0 * w1 - p1 * w0) / (w1 - w0), ((p0 * w0 - p1 * w1) / (w1 - w0)).normalize());
		}

	}

	constexpr unsigned char p0 = 0x01;
	constexpr unsigned char p1 = 0x02;
	constexpr unsigned char p2 = 0x04;
	constexpr unsigned char p3 = 0x08;
	constexpr unsigned char p4 = 0x10;
	constexpr unsigned char p5 = 0x20;
	constexpr unsigned char p6 = 0x40;
	constexpr unsigned char p7 = 0x80;

#define CASE(p) case (p):case 0xff - (p):
#define T(v0,v1,v2) triangles.push_back(triangle(vi + v0,vi + v1,vi + v2))

	switch (b)
	{
	CASE(0x00)
		break;
	CASE(p0)
	CASE(p1)
	CASE(p2)
	CASE(p3)
	CASE(p4)
	CASE(p5)
	CASE(p6)
	CASE(p7)
		T(0,1,2);
		break;
	CASE(p0 | p1)
	CASE(p1 | p2)
	CASE(p2 | p3)
	CASE(p3 | p0)
	CASE(p4 | p5)
	CASE(p5 | p6)
	CASE(p6 | p7)
	CASE(p7 | p4)
	CASE(p0 | p4)
	CASE(p1 | p5)
	CASE(p2 | p6)
	CASE(p3 | p7)
	CASE(p0 | p1 | p2 | p3)
	CASE(p0 | p1 | p4 | p5)
	CASE(p0 | p3 | p4 | p7)
		T(0, 1, 2);
		T(1, 2, 3);
		break;
	CASE(p0 | p1 | p3)
		T(0, 2, 4);
		T(1, 2, 3);
		T(2, 3, 4);
		break;
	CASE(p0 | p4 | p1)
		T(0, 1, 4);
		T(1, 2, 4);
		T(2, 4, 3);
		break;
	CASE(p0 | p3 | p4)
		T(0, 1, 3);
		T(1, 2, 3);
		T(2, 3, 4);
		break;
	CASE(p1 | p0 | p2)
		T(1, 2, 4);
		T(0, 1, 3);
		T(1, 3, 4);
		break;
	CASE(p1 | p2 | p5)
	CASE(p3 | p2 | p7)
	CASE(p4 | p5 | p7)
		T(0, 1, 2);
		T(1, 2, 3);
		T(2, 3, 4);
		break;
	CASE(p1 | p0 | p5)
	CASE(p2 | p6 | p1)
	CASE(p3 | p7 | p0)
	CASE(p5 | p6 | p1)
	CASE(p7 | p6 | p3)
		T(0, 2, 4);
		T(0, 1, 4);
		T(1, 4, 3);
		break;
	CASE(p2 | p1 | p3)
	CASE(p5 | p1 | p4)
	CASE(p6 | p2 | p5)
	CASE(p7 | p3 | p4)
		T(2, 3, 4);
		T(0, 1, 2);
		T(1, 2, 3);
		break;
	CASE(p2 | p3 | p6)
	CASE(p5 | p4 | p6)
	CASE(p7 | p4 | p6)
		T(0, 1, 3);
		T(0, 2, 3);
		T(2, 3, 4);
		break;
	CASE(p3 | p0 | p2)
	CASE(p4 | p0 | p5)
		T(1, 3, 4);
		T(1, 4, 0);
		T(4, 0, 2);
		break;
	CASE(p4 | p7 | p0)
		T(0, 3, 4);
		T(0, 4, 1);
		T(4, 1, 2);
		break;
	CASE(p6 | p5 | p7)
		T(0, 1, 4);
		T(0, 1, 2);
		T(1, 2, 3);
		break;
	CASE(p6 | p7 | p2)
		T(0, 3, 4);
		T(0, 3, 1);
		T(3, 1, 2);
		break;
	CASE(p0 | p1 | p3 | p4)
		T(0, 1, 4);
		T(0, 4, 2);
		T(4, 2, 5);
		T(2, 5, 3);
		break;
	CASE(p1 | p2 | p0 | p5)
		T(1, 3, 5);
		T(1, 5, 0);
		T(5, 0, 4);
		T(0, 4, 2);
		break;
	CASE(p2 | p3 | p1 | p6)
		T(0, 2, 4);
		T(0, 4, 5);
		T(0, 5, 1);
		T(5, 1, 3);
		break;
	CASE(p3 | p0 | p2 | p7)
		T(0, 2, 4);
		T(0, 4, 5);
		T(0, 5, 1);
		T(5, 1, 3);
		break;

	default:
		throw INFO_EXCEPT("binggoo");
		break;
	}

#undef CASE
#undef T

	for (unsigned i = 0; i < triangles.size; i++)
	{
		triangle t = triangles.data[i];
		indexs.push_back(t.v0);
		indexs.push_back(t.v1);
		indexs.push_back(t.v2);
	}
}
