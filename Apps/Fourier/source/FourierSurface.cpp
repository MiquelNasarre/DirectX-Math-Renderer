#include "FourierSurface.h"
#include "Bindable/BindableBase.h"

Vector3f*					FourierSurface::vertexsIcosphere = NULL;
unsigned int				FourierSurface::depthIcosphere = 6u;
unsigned int*				FourierSurface::trianglesIcosphere = NULL;
FourierSurface::infoVect*	FourierSurface::infoIcosphere = NULL;

unsigned int	FourierSurface::ntriangles = 0;
unsigned int	FourierSurface::nvertexs = 0;

unsigned int	FourierSurface::Functions::maxL = MAX_L;
float**			FourierSurface::Functions::Constants = NULL;
Vector3f***		FourierSurface::Functions::DatasetYlmi = (Vector3f***)calloc(MAX_L + 1, sizeof(void*));

// File Manager Functions

const void** FourierSurface::FileManager::extractFigureFromFile(const char* filename)
{
	FILE* file = fopen((FIGURES_DIR + std::string(filename) + ".dat").c_str(), "r");
	if (!file)
		throw std::exception(("Unable to find or open file: " + std::string(filename)).c_str());

	fscanf(file, "Vertexs:\n");
	char s[100];
	int nV = 0;
	while (fgets(s, 100, file)[0] != 'T')nV++;

	rewind(file);
	fscanf(file, "Vertexs:\n");

	Vector3f* vertexs = (Vector3f*)calloc(nV, sizeof(Vector3f));
	for (int i = 0; i < nV; i++)
		fscanf(file, "(%f,%f,%f)\n", &vertexs[i].x, &vertexs[i].y, &vertexs[i].z);

	fscanf(file, "Triangles:\n");

	int nT = 0;
	while (fscanf(file, "%s\n", s) != EOF)nT++;

	rewind(file);
	fscanf(file, "Vertexs:\n");
	while (fgets(s, 100, file)[0] != 'T');

	Vector3i* triangles = (Vector3i*)calloc(nT, sizeof(Vector3i));

	for (int i = 0; i < nT; i++)
		fscanf(file, "%i,%i,%i", &triangles[i].x, &triangles[i].y, &triangles[i].z);

	fclose(file);

	const void** figure = (const void**)calloc(3, sizeof(void*));
	int* nt = (int*)calloc(2, sizeof(int));
	nt[0] = nT;
	nt[1] = nV;
	figure[0] = vertexs;
	figure[1] = triangles;
	figure[2] = nt;

	return figure;
}

unsigned int FourierSurface::FileManager::ncoefFromFile(const char* filename)
{
	FILE* file = fopen((COEFFICIENTS_DIR + std::string(filename) + ".dat").c_str(), "r");
	if (!file)
		throw std::exception(("Unable to find or open file: " + std::string(filename)).c_str());

	unsigned int C = 0u;
	fscanf(file, "Number of coefficients: %u", &C);

	fclose(file);

	return C;
}

Coefficient* FourierSurface::FileManager::loadCoefficientsFromFile(const char* filename)
{
	FILE* file = fopen((COEFFICIENTS_DIR + std::string(filename) + ".dat").c_str(), "r");
	if (!file)
		throw std::exception(("Unable to find or open file: " + std::string(filename)).c_str());

	unsigned int N = 0u;
	fscanf(file, "Number of coefficients: %u", &N);

	Coefficient* coef = (Coefficient*)calloc(N, sizeof(Coefficient));
	for (unsigned int i = 0; i < N; i++)
		fscanf(file, "\nL: %u M: %i C: %f", &coef[i].L, &coef[i].M, &coef[i].C);

	return coef;
}

void FourierSurface::FileManager::saveCoefficients(Coefficient* coef, unsigned int ncoef, const char* filename)
{
	FILE* file = fopen((COEFFICIENTS_DIR + std::string(filename) + ".dat").c_str(), "w");
	
	fprintf(file, "Number of coefficients: %u", ncoef);
	
	for (unsigned int i = 0; i < ncoef; i++)
		fprintf(file, "\nL: %i M: %i C: %.10f", coef[i].L, coef[i].M, coef[i].C);

	fclose(file);
}

// Math Functions

void			FourierSurface::Functions::generateHarmonicsAsync()
{
	bool finished[4] = { true,true,true,true };

	unsigned int l = 0;
	while(l<=maxL)
	{
		while (!finished[0] && !finished[1] && !finished[2] && !finished[3])
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		for (int w = 0; w < 4; w++)
		{
			if (finished[w])
			{
				finished[w] = false;
				std::thread(generateDataAsync, DatasetYlmi, l, &finished[w]).detach();
				if (++l > maxL)
					break;
				
			}
		}
	}
	while (!finished[0] || !finished[1] || !finished[2] || !finished[3])
		std::this_thread::sleep_for(std::chrono::milliseconds(2));
}

void			FourierSurface::Functions::generateDataAsync(Vector3f*** dataset, unsigned int l, bool* done)
{
	Vector3f** data = (Vector3f**)calloc(2 * l + 1, sizeof(void*));
	for (int m = -int(l); m <= int(l); m++)
	{
		data[m + l] = (Vector3f*)calloc(nvertexs, sizeof(Vector3f));

		for (unsigned int i = 0; i < nvertexs; i++)
			data[m + l][i] = Ylmdif(i, l, m);
	}
	dataset[l] = data;
	*done = true;
}

void			FourierSurface::Functions::fillTriangleData(unsigned short depth, Vector3f V0, Vector3f V1, Vector3f V2, Vector3f* Centers, float* Distances)
{
	if (depth > 0)
	{
		unsigned int triangleDivisions = 1u;
		for (unsigned short i = 0; i < depth - 1; i++)
			triangleDivisions *= 4u;

		Vector3f V01 = 0.5f * (V0 + V1);
		Vector3f V12 = 0.5f * (V1 + V2);
		Vector3f V20 = 0.5f * (V2 + V0);

		fillTriangleData(depth - 1, V0, V01, V20, Centers, Distances);
		fillTriangleData(depth - 1, V1, V12, V01, &Centers[triangleDivisions], &Distances[triangleDivisions]);
		fillTriangleData(depth - 1, V2, V20, V12, &Centers[2 * triangleDivisions], &Distances[2 * triangleDivisions]);
		fillTriangleData(depth - 1, V20, V01, V12, &Centers[3 * triangleDivisions], &Distances[3 * triangleDivisions]);
		return;
	}

	Centers[0] = (V0 + V1 + V2) / 3.f;
	Distances[0] = Centers[0].abs();
	Centers[0] /= Distances[0];
}

void			FourierSurface::Functions::calculateCoefficientAsync(Coefficient* coef, const unsigned int l, const Vector3f* centerTriangles, const float* areaTriangles, const float* distanceTriangles, const unsigned int numT, bool* finished)
{
	for (int m = -int(l); m <= int(l); m++)
	{
		float C = 0.f;

		for (unsigned int i = 0; i < numT; i++)
			C += areaTriangles[i] * distanceTriangles[i] * Ylm(centerTriangles[i], l, m);

		C *= 0.25f / pi;

		coef[m + l].L = l;
		coef[m + l].M = m;
		coef[m + l].C = C;
	}
	*finished = true;
}

Coefficient*	FourierSurface::Functions::calculateCoefficients(const void** extractedFigure, const unsigned int maxL, const unsigned short triangleDepth)
{
	const Vector3f* vertexs = (Vector3f*)extractedFigure[0];
	const Vector3i* triangles = (Vector3i*)extractedFigure[1];
	const int numT = ((int*)extractedFigure[2])[0];
	const int numV = ((int*)extractedFigure[2])[1];

	// Now we have the figure
	// Lets calculate the coefficients

	unsigned int triangleDivisions = 1u;
	for (unsigned short i = 0; i < triangleDepth; i++)
		triangleDivisions *= 4u;

	Vector3f* centerTriangles = (Vector3f*)calloc(triangleDivisions * numT, sizeof(Vector3f));
	float* areaTriangles = (float*)calloc(triangleDivisions * numT, sizeof(float));
	float* distanceTriangles = (float*)calloc(triangleDivisions * numT, sizeof(float));

	for (int i = 0; i < numT; i++)
	{
		Vector3f V1 = vertexs[triangles[i].x];
		Vector3f V2 = vertexs[triangles[i].y];
		Vector3f V3 = vertexs[triangles[i].z];

		Vector3f V12 = (V1 * V2 * V1).normalize();
		Vector3f V13 = (V1 * V3 * V1).normalize();

		float angle1 = acosf(V13 ^ V12);

		Vector3f V21 = (V2 * V1 * V2).normalize();
		Vector3f V23 = (V2 * V3 * V2).normalize();

		float angle2 = acosf(V23 ^ V21);

		Vector3f V32 = (V3 * V2 * V3).normalize();
		Vector3f V31 = (V3 * V1 * V3).normalize();

		float angle3 = acosf(V31 ^ V32);

		if (triangleDepth)
		{
			fillTriangleData(triangleDepth, V1, V2, V3, &centerTriangles[i * triangleDivisions], &distanceTriangles[i * triangleDivisions]);
			float area = (angle1 + angle2 + angle3 - pi) / triangleDivisions;
			for (unsigned int j = 0; j < triangleDivisions; j++)
				areaTriangles[triangleDivisions * i + j] = area;
		}
		else
		{
			centerTriangles[i] = (V1 + V2 + V3) / 3.f;
			areaTriangles[i] = angle1 + angle2 + angle3 - pi;
			distanceTriangles[i] = centerTriangles[i].abs();
			centerTriangles[i].normalize();
		}
	}

	Coefficient* Coef = (Coefficient*)calloc((maxL + 1) * (maxL + 1), sizeof(Coefficient));
	bool finished[4] = { true,true,true,true };
	unsigned int l = 0;
	while (l <= maxL)
	{
		while (!finished[0] && !finished[1] && !finished[2] && !finished[3])
			std::this_thread::sleep_for(std::chrono::milliseconds(2));
		for (int w = 0; w < 4; w++)
		{
			if (finished[w])
			{
				finished[w] = false;
				std::thread(calculateCoefficientAsync, &Coef[l * l], l, centerTriangles, areaTriangles, distanceTriangles, triangleDivisions * numT, &finished[w]).detach();
				if (++l > maxL)
					break;
			}
		}
	}
	while (!finished[0] || !finished[1] || !finished[2] || !finished[3])
		std::this_thread::sleep_for(std::chrono::milliseconds(2));

	free(centerTriangles);
	free(areaTriangles);
	free(distanceTriangles);
	return Coef;

}

float			FourierSurface::Functions::Ylm(int l, int m, float phi, float theta)
{
	float costheta = cosf(theta);
	float sintheta = sinf(theta);
	float cosphi = cosf(phi);
	float sinphi = sinf(phi);

	if (m > 0)
		return Klm(l, m) * Functions::Tchev(m, cosphi) * Functions::Legendre(l, m, costheta);
	else if (m == 0)
		return Klm(l, m) * Functions::Legendre(l, m, costheta);
	else
		return Klm(l, m) * Functions::Uchev(-m - 1, cosphi) * sinphi * Functions::Legendre(l, -m, costheta);
}

float			FourierSurface::Functions::Ylm(Vector3f v, unsigned int l, int m)
{
	float costheta = v.z;
	float sintheta = sqrtf(1 - v.z * v.z);
	float cosphi = v.x / sintheta;
	float sinphi = v.y / sintheta;

	if (m > 0)
		return Klm(l, m) * Functions::Tchev(m, cosphi) * Functions::Legendre(l, m, costheta);
	else if (m == 0)
		return Klm(l, m) * Functions::Legendre(l, m, costheta);
	else
		return Klm(l, m) * Functions::Uchev(-m - 1, cosphi) * sinphi * Functions::Legendre(l, -m, costheta);
}

float			FourierSurface::Functions::Ylm(unsigned int i, unsigned int l, int m)
{
	if (DatasetYlmi[l])
		return DatasetYlmi[l][l + m][i].x;

	if (m > 0)
		return Klm(l, m) * Functions::Tchev(m, infoIcosphere[i].cosphi) * Functions::Legendre(l, m, vertexsIcosphere[i].z);
	else if (m == 0)
		return Klm(l, m) * Functions::Legendre(l, m, vertexsIcosphere[i].z);
	else
		return Klm(l, m) * Functions::Uchev(-m - 1, infoIcosphere[i].cosphi) * infoIcosphere[i].sinphi * Functions::Legendre(l, -m, vertexsIcosphere[i].z);

}

_float4vector	FourierSurface::Functions::YlmDif(unsigned int i, unsigned int l, int m)
{
	_float4vector ylm;

	float minus_sint_dty;
	float minus_dpy;

	if (DatasetYlmi[l])
	{
		ylm.w = DatasetYlmi[l][m + l][i].x;
		minus_sint_dty = -infoIcosphere[i].sintheta * DatasetYlmi[l][m + l][i].y;
		minus_dpy = -DatasetYlmi[l][m + l][i].z;
	}
	else if (m > 0)
	{
		Vector2f leg = Functions::LegendreDif(l, m, infoIcosphere[i].costheta);
		float cosmp = Functions::Tchev(m, infoIcosphere[i].cosphi);
		float sinmp = Functions::Uchev(m - 1, infoIcosphere[i].cosphi) * infoIcosphere[i].sinphi;
		float K = Klm(l, m);

		ylm.w = K * leg.x * cosmp;

		minus_sint_dty = infoIcosphere[i].sintheta * infoIcosphere[i].sintheta * K * leg.y * cosmp;
		minus_dpy = K * leg.x * m * sinmp;
	}
	else if (m == 0)
	{
		Vector2f leg = Functions::LegendreDif(l, m, infoIcosphere[i].costheta);
		float K = Klm(l, m);

		ylm.w = K * leg.x;

		minus_sint_dty = infoIcosphere[i].sintheta * infoIcosphere[i].sintheta * K * leg.y;
		minus_dpy = 0.f;
	}
	else
	{
		Vector2f leg = Functions::LegendreDif(l, -m, infoIcosphere[i].costheta);
		float cosmp = Functions::Tchev(-m, infoIcosphere[i].cosphi);
		float sinmp = Functions::Uchev(-m - 1, infoIcosphere[i].cosphi) * infoIcosphere[i].sinphi;
		float K = Klm(l, -m);

		ylm.w = K * leg.x * sinmp;

		minus_sint_dty = infoIcosphere[i].sintheta * infoIcosphere[i].sintheta * K * leg.y * sinmp;
		minus_dpy = K * leg.x * m * cosmp;
	}

	// Vectors normals

	if (i == 12)
	{
		ylm.x = 0.f;
		ylm.x = 0.f;
		ylm.z = 1.f;
		return ylm;
	}
	if (i == 17)
	{
		ylm.x = 0.f;
		ylm.x = 0.f;
		ylm.z = -1.f;
		return ylm;
	}

	ylm.x = ylm.w * (minus_sint_dty * infoIcosphere[i].costheta * infoIcosphere[i].cosphi - minus_dpy * infoIcosphere[i].sinphi + ylm.w * infoIcosphere[i].sintheta * vertexsIcosphere[i].x);
	ylm.y = ylm.w * (minus_sint_dty * infoIcosphere[i].costheta * infoIcosphere[i].sinphi + minus_dpy * infoIcosphere[i].cosphi + ylm.w * infoIcosphere[i].sintheta * vertexsIcosphere[i].y);
	ylm.z = ylm.w * infoIcosphere[i].sintheta * (-minus_sint_dty + ylm.w * vertexsIcosphere[i].z);

	return ylm;
}

Vector3f		FourierSurface::Functions::Ylmdif(unsigned int i, unsigned int l, int m)
{
	if (DatasetYlmi[l])
	{
		Vector3f ylm = DatasetYlmi[l][m + l][i];
		return ylm;
	}

	Vector3f ylm;

	if (m > 0)
	{
		Vector2f leg = Functions::LegendreDif(l, m, infoIcosphere[i].costheta);
		float cosmp = Functions::Tchev(m, infoIcosphere[i].cosphi);
		float sinmp = Functions::Uchev(m - 1, infoIcosphere[i].cosphi) * infoIcosphere[i].sinphi;
		float K = Klm(l, m);

		ylm.x = K * leg.x * cosmp;

		ylm.y = -infoIcosphere[i].sintheta * K * leg.y * cosmp;
		ylm.z = -K * leg.x * m * sinmp;

		return ylm;
	}
	if (m == 0)
	{
		Vector2f leg = Functions::LegendreDif(l, m, infoIcosphere[i].costheta);
		float K = Klm(l, m);

		ylm.x = K * leg.x;

		ylm.y = -infoIcosphere[i].sintheta * K * leg.y;
		ylm.z = 0.f;

		return ylm;
	}
	else
	{
		Vector2f leg = Functions::LegendreDif(l, -m, infoIcosphere[i].costheta);
		float cosmp = Functions::Tchev(-m, infoIcosphere[i].cosphi);
		float sinmp = Functions::Uchev(-m - 1, infoIcosphere[i].cosphi) * infoIcosphere[i].sinphi;
		float K = Klm(l, -m);

		ylm.x = K * leg.x * sinmp;
		ylm.y = -infoIcosphere[i].sintheta * K * leg.y * sinmp;
		ylm.z = -K * leg.x * m * cosmp;



		return ylm;
	}
}

float			FourierSurface::Functions::Legendre(int l, int m, float x)
{
	float Plm;
	
	if (x == 1.f || x == -1)
	{
		if (m == 0)
		{
			if (x == -1.f && l % 2)
				return -1.f;
			else
				return 1.f;
		}
		return 0.f;
	}

	if (l == 0)
	{
		return 1.f;
	}

	float x12 = 1 - x * x;
	float sq = sqrtf(x12);
	int lc = m;


	Plm = DFactorial(2 * m - 1);

	for (unsigned short i = 0; i < m; i++)
		Plm *= -sq;

	if (l == m)
		return Plm;

	float aux = x * (2 * lc + 1) * Plm;
	float Pl1m = Plm;
	Plm = aux;
	lc++;

	while (lc < l) {
		aux = ((2 * lc + 1) * x * Plm - (lc + m) * Pl1m) / (lc - m + 1);
		lc++;
		Pl1m = Plm;
		Plm = aux;
	}

	return Plm;
}

Vector2f		FourierSurface::Functions::LegendreDif(int l, int m, float x)
{
	Vector2f Plm;

	if (x == 1.f || x == -1)
	{
		if (m == 0)
		{
			if (x == -1.f && l % 2)
				Plm.x = -1.f;
			else
				Plm.x = 1.f;
		}
		else
			Plm.x = 0.f;
		Plm.y = l * (l + 1) * 0.5f;
		return Plm;
	}

	if (l == 0)
	{
		Plm.x = 1.f;
		Plm.y = 0.f;
		return Plm;
	}

	float x12 = 1 - x * x;
	float sq = sqrtf(x12);
	int lc = m;


	Plm.x = DFactorial(2 * m - 1);

	for (unsigned short i = 0; i < m; i++)
		Plm.x *= -sq;

	if (l == m)
	{
		Plm.y = -x * l / x12 * Plm.x;
		return Plm;
	}


	float aux = x * (2 * lc + 1) * Plm.x;
	float Pl1m = Plm.x;
	Plm.x = aux;
	lc++;

	while (lc < l) {
		aux = ((2 * lc + 1) * x * Plm.x - (lc + m) * Pl1m) / (lc - m + 1);
		lc++;
		Pl1m = Plm.x;
		Plm.x = aux;
	}

	Plm.y = ((l + m) * Pl1m - l * x * Plm.x) / x12;

	return Plm;
}

float			FourierSurface::Functions::Tchev(int n, float x)
{
	if (!n)
		return 1.f;
	if (n == 1)
		return x;

	float Ti1 = 1.f;
	float Ti = x;

	int m = 1;
	float temp;
	while (m++ < n)
	{
		temp = 2 * x * Ti - Ti1;
		Ti1 = Ti;
		Ti = temp;
	}
	return Ti;
}

float			FourierSurface::Functions::Uchev(int n, float x)
{
	if (!n)
		return 1.f;
	if (n == 1)
		return 2 * x;

	float Ui1 = 1.f;
	float Ui = 2 * x;

	int m = 1;
	float temp;
	while (m++ < n)
	{
		temp = 2 * x * Ui - Ui1;
		Ui1 = Ui;
		Ui = temp;
	}
	return Ui;
}

int				FourierSurface::Functions::Factorial(int n)
{
	if (!n) return 1;
	int f = n;
	while (--n)
		f *= n;
	return f;
}

float			FourierSurface::Functions::DFactorial(int n)
{
	switch (n)
	{
	case 1:
		return 1.f;
	case 3:
		return 3.f;
	case 5:
		return 15.f;
	case 7:
		return 105.f;
	case 9:
		return 945.f;
	case 11:
		return 10395.f;
	case 13:
		return 135135.f;
	case 15:
		return 2027025.f;
	case 17:
		return 34459425.f;
	case 19:
		return 654729075.f;
	case 21:
		return 13749310575.f;

	default:
		break;
	}

	if (n < 1) return 1;
	float f = float(n);
	while (--n > 1) {
		--n;
		f *= n;
	}
	return f;
}
	
float			FourierSurface::Functions::DivFactorial(int n, int m)
{
	if (!n || n <= m) return 1;
	float f = float(n);

	while (--n > m)
		f *= n;

	return f;
}
	
float			FourierSurface::Functions::sqDivFactorial(int n, int m)
{
	if (!n || n <= m) return 1;
	float f = sqrtf(float(n));

	while (--n > m)
		f *= sqrtf(float(n));

	return f;
}

float			FourierSurface::Functions::Klm(unsigned int l, int m)
{	
	return m>=0 ? Constants[l][m] : Constants[l][-m];
}

void			FourierSurface::Functions::generateConstants()
{
	Constants = (float**)calloc(maxL + 1, sizeof(void*));
	for (unsigned int l = 0; l <= maxL; l++)
	{
		Constants[l] = (float*)calloc(l + 1, sizeof(float));

		Constants[l][0] = sqrtf(float(2 * l + 1));
		for (unsigned int m = 1; m <= l; m++)
			Constants[l][m] = sqrtf(float(2 * (2 * l + 1))) / Functions::sqDivFactorial(l + m, l - m);
	}
}

// Surface creation & stuff

// Private static

void		FourierSurface::generateIcosphere()
{
	unsigned int depth = depthIcosphere;

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
	triangle* triangles = (triangle*)calloc(C,sizeof(triangle));

	float gold = (1.f + sqrtf(5) / 2.f);

	vertexs[ 0] = { 0.f, 1.f, gold };
	vertexs[ 1] = { 0.f, 1.f,-gold };
	vertexs[ 2] = { 0.f,-1.f, gold };
	vertexs[ 3] = { 0.f,-1.f,-gold };
	vertexs[ 4] = { 1.f, gold, 0.f };
	vertexs[ 5] = { 1.f,-gold, 0.f };
	vertexs[ 6] = {-1.f, gold, 0.f };
	vertexs[ 7] = {-1.f,-gold, 0.f };
	vertexs[ 8] = { gold, 0.f, 1.f };
	vertexs[ 9] = {-gold, 0.f, 1.f };
	vertexs[10] = { gold, 0.f,-1.f };
	vertexs[11] = {-gold, 0.f,-1.f };


	aristas[ 0] = { 0, 2 };
	aristas[ 1] = { 0, 4 };
	aristas[ 2] = { 0, 6 };
	aristas[ 3] = { 0, 8 };
	aristas[ 4] = { 0, 9 };
	aristas[ 5] = { 1, 3 };
	aristas[ 6] = { 1, 4 };
	aristas[ 7] = { 1, 6 };
	aristas[ 8] = { 1,10 };
	aristas[ 9] = { 1,11 };
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


	triangles[ 0] = {  3,-19, -2 };
	triangles[ 1] = {  7, 19, -8 };
	triangles[ 2] = { 11, 22,-12 };
	triangles[ 3] = { 16,-22,-15 };
	triangles[ 4] = { 21,-29,-20 };
	triangles[ 5] = { 23, 29,-24 };
	triangles[ 6] = { 25, 30,-26 };
	triangles[ 7] = { 28,-30,-27 };
	triangles[ 8] = {-13, -1,  4 };
	triangles[ 9] = { -5,  1, 14 };
	triangles[10] = { -9,  6, 17 };
	triangles[11] = {-18, -6, 10 };
	triangles[12] = {  2, 20, -4 };
	triangles[13] = {  5,-25, -3 };
	triangles[14] = { -7,  9,-21 };
	triangles[15] = { 26,-10,  8 };
	triangles[16] = { 18,-28,-16 };
	triangles[17] = { 15, 24,-17 };
	triangles[18] = { 27,-14, 12 };
	triangles[19] = {-11, 13,-23 };

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

		vertexs =	(Vector3f*)calloc(V, sizeof(Vector3f));
		aristas =	(arista*)calloc(A, sizeof(arista));
		triangles =	(triangle*)calloc(C, sizeof(triangle));

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

	infoIcosphere = (infoVect*)calloc(V, sizeof(infoVect));

	trianglesIcosphere = (unsigned int*)calloc(3 * C, sizeof(unsigned int));
	for (unsigned int i = 0; i < C; i++)
	{
		unsigned int aris0 = abs(triangles[i].a0) - 1;
		bool ornt0 = (triangles[i].a0 > 0) ? true : false;
		unsigned int aris1 = abs(triangles[i].a1) - 1;
		bool ornt1 = (triangles[i].a1 > 0) ? true : false;
		unsigned int aris2 = abs(triangles[i].a2) - 1;
		bool ornt2 = (triangles[i].a2 > 0) ? true : false;
		
		trianglesIcosphere[3 * i]		= ornt0 ? aristas[aris0].v0 : aristas[aris0].v1;
		trianglesIcosphere[3 * i + 1]	= ornt1 ? aristas[aris1].v0 : aristas[aris1].v1;
		trianglesIcosphere[3 * i + 2]	= ornt2 ? aristas[aris2].v0 : aristas[aris2].v1;

	}
	vertexsIcosphere = vertexs;

	nvertexs = V;
	ntriangles = C;

	for (unsigned int i = 0; i < nvertexs; i++)
	{
		vertexsIcosphere[i].normalize();
		infoIcosphere[i].costheta = vertexsIcosphere[i].z;
		infoIcosphere[i].sintheta = sqrtf(1 - vertexsIcosphere[i].z * vertexsIcosphere[i].z);
		infoIcosphere[i].cosphi = vertexsIcosphere[i].x / infoIcosphere[i].sintheta;
		infoIcosphere[i].sinphi = vertexsIcosphere[i].y / infoIcosphere[i].sintheta;
	}
	infoIcosphere[12].sinphi = 0.f;
	infoIcosphere[12].cosphi = 1.f;
	infoIcosphere[17].sinphi = 0.f;
	infoIcosphere[17].cosphi = 1.f;

	free(aristas);
	free(triangles);
}

void		FourierSurface::calculateVertexsAsync(const unsigned int ti, const unsigned int tf, const Coefficient* coef, const unsigned int ncoef, Vertex* V)
{
	for (unsigned int i = ti; i < tf; i++)
	{
		if (ncoef == 1)
		{
			if (Functions::DatasetYlmi[coef[0].L])
				V[i].dYlm = coef[0].C * Functions::DatasetYlmi[coef[0].L][coef[0].M + coef[0].L][i];
			else
				V[i].dYlm = coef[0].C * Functions::Ylmdif(i, coef[0].L, coef[0].M);

			if (V[i].dYlm.x > 0)
				V[i].color = Color::Blue;
			else
				V[i].color = Color::Yellow;

			V[i].vector = vertexsIcosphere[i];
		}
		else
		{
			Vector3f ylm(0.f, 0.f, 0.f);
			for (unsigned int j = 0; j < ncoef; j++)
			{
				if (Functions::DatasetYlmi[coef[j].L])
					ylm += coef[j].C * Functions::DatasetYlmi[coef[j].L][coef[j].M + coef[j].L][i];
				else
					ylm += coef[j].C * Functions::Ylmdif(i, coef[j].L, coef[j].M);
			}

			V[i].vector = vertexsIcosphere[i];
			V[i].dYlm = ylm;
			V[i].color = Color::White;

		}
	}
}

// Construction

FourierSurface::~FourierSurface()
{
	if (Coef)
		free(Coef);
	if (Vertexs)
		free(Vertexs);
}

FourierSurface::FourierSurface(Graphics& gfx, const Coefficient* coef, const unsigned int ncoef)
{
	create(gfx, coef, ncoef);
}

void		FourierSurface::create(Graphics& gfx, const Coefficient* coef, const unsigned int ncoef, std::mutex* mtx)
{
	if (isInit)
		throw std::exception("You cannot create a surface over one that is already initialized");
	else
		isInit = true;

	if (!Functions::Constants)
		Functions::generateConstants();

	Coef = (Coefficient*)calloc(ncoef, sizeof(Coefficient));
	for (unsigned int i = 0; i < ncoef; i++)
		Coef[i] = coef[i];

	Ncoef = ncoef;

	if (!vertexsIcosphere)
		generateIcosphere();

	Vertexs = (Vertex*)calloc(nvertexs, sizeof(Vertex));
	const unsigned int t0 = 0u;
	const unsigned int t1 = nvertexs / 4u;
	const unsigned int t2 = nvertexs / 2u;
	const unsigned int t3 = 3u * nvertexs / 4u;
	const unsigned int t4 = nvertexs;

	std::thread worker0 = std::thread(calculateVertexsAsync, t0, t1, coef, ncoef, Vertexs);
	std::thread worker1 = std::thread(calculateVertexsAsync, t1, t2, coef, ncoef, Vertexs);
	std::thread worker2 = std::thread(calculateVertexsAsync, t2, t3, coef, ncoef, Vertexs);
	std::thread worker3 = std::thread(calculateVertexsAsync, t3, t4, coef, ncoef, Vertexs);

	curves.create(gfx, coef, ncoef, 0.f, 0.f, mtx);

	worker0.join();
	worker1.join();
	worker2.join();
	worker3.join();

	if(mtx)
		mtx->lock();

	AddBind(std::make_unique<VertexBuffer>(gfx, Vertexs, nvertexs));

	AddBind(std::make_unique<IndexBuffer>(gfx, trianglesIcosphere, 3 * ntriangles));

	auto pvs = (VertexShader*)AddBind(std::move(std::make_unique<VertexShader>(gfx, SHADERS_DIR L"VertexShader.cso")));

	AddBind(std::make_unique<PixelShader>(gfx, SHADERS_DIR L"PixelShader.cso"));

	D3D11_INPUT_ELEMENT_DESC ied[3] =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Dylm",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Color",0,DXGI_FORMAT_B8G8R8A8_UNORM,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};

	AddBind(std::make_unique<InputLayout>(gfx, ied, 3u, pvs->GetBytecode()));

	AddBind(std::make_unique<Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_unique<Rasterizer>(gfx, true));

	pVSCB = (ConstantBuffer<VSconstBuffer>*)AddBind(std::make_unique<ConstantBuffer<VSconstBuffer>>(gfx, VERTEX_CONSTANT_BUFFER_TYPE));

	pscBuff.lightsource[0] = { { 600.f, 320.f, 0.f, 0.f } , { 1.f, 1.f, 1.f, 1.f } , { 30.f, 10.f, 20.f, 0.f } };

	pPSCB = (ConstantBuffer<PSconstBuffer>*)AddBind(std::make_unique<ConstantBuffer<PSconstBuffer>>(gfx, pscBuff, PIXEL_CONSTANT_BUFFER_TYPE));

	if (mtx)
		mtx->unlock();
}

void		FourierSurface::updateShape(Graphics& gfx, const Coefficient* coef, const unsigned int ncoef)
{
	if (!isInit)
		throw std::exception("You cannot update shape to an uninitialized surface");

	free(Coef);
	Coef = (Coefficient*)calloc(ncoef, sizeof(Coefficient));
	for (unsigned int i = 0; i < ncoef; i++)
		Coef[i] = coef[i];

	Ncoef = ncoef;

	if (!vertexsIcosphere)
		generateIcosphere();

	const unsigned int t0 = 0u;
	const unsigned int t1 = nvertexs / 4u;
	const unsigned int t2 = nvertexs / 2u;
	const unsigned int t3 = 3u * nvertexs / 4u;
	const unsigned int t4 = nvertexs;

	std::thread worker0 = std::thread(calculateVertexsAsync, t0, t1, coef, ncoef, Vertexs);
	std::thread worker1 = std::thread(calculateVertexsAsync, t1, t2, coef, ncoef, Vertexs);
	std::thread worker2 = std::thread(calculateVertexsAsync, t2, t3, coef, ncoef, Vertexs);
	std::thread worker3 = std::thread(calculateVertexsAsync, t3, t4, coef, ncoef, Vertexs);

	curves.updateShape(gfx, coef, ncoef, 0.f, 0.f);

	worker0.join();
	worker1.join();
	worker2.join();
	worker3.join();

	changeBind(std::make_unique<VertexBuffer>(gfx, Vertexs, nvertexs), 0u);

}

// User friendly

void		FourierSurface::saveCoefficients(const char* filename)
{
	FileManager::saveCoefficients(Coef, Ncoef, filename);
}

void		FourierSurface::updateLight(Graphics& gfx, UINT id, Vector2f intensity, Color color, Vector3f position)
{
	if (!isInit)
		throw std::exception("You cannot update light to an uninitialized surface");

	pscBuff.lightsource[id] = { intensity.getVector4() , color.getColor4() , position.getVector4() };
	pPSCB->Update(gfx, pscBuff);
}

void		FourierSurface::updateLight(Graphics& gfx, UINT id, _float4vector intensity, _float4color color, _float4vector position)
{
	if (!isInit)
		throw std::exception("You cannot update light to an uninitialized surface");

	pscBuff.lightsource[id] = { intensity , color , position };
	pPSCB->Update(gfx, pscBuff);
}

void		FourierSurface::clearLights(Graphics& gfx)
{
	if (!isInit)
		throw std::exception("You cannot update clear lights to an uninitialized surface");

	pscBuff = {};
	pPSCB->Update(gfx, pscBuff);
}

void		FourierSurface::updateTexture(Graphics& gfx, Color color, bool def, bool random)
{
	if (!isInit)
		throw std::exception("You cannot update texture to an uninitialized surface");

	for (unsigned int i = 0; i < nvertexs; i++)
	{
		if (def)
		{
			if (Ncoef > 1)
				Vertexs[i].color = Color::White;
			else
				Vertexs[i].color = Functions::Ylm(i, Coef[0].L, Coef[0].M) > 0.f ? Color::Blue : Color::Yellow;
		}
		else if (random)
			Vertexs[i].color = Color(rand() % 256, rand() % 256, rand() % 256);
		else
			Vertexs[i].color = color;
	}

	changeBind(std::make_unique<VertexBuffer>(gfx, Vertexs, nvertexs), 0u);
}

void		FourierSurface::updateRotation(Graphics& gfx, Vector3f axis, float angle, bool multiplicative)
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

void		FourierSurface::updateRotation(Graphics& gfx, Quaternion rotation, bool multiplicative)
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

void		FourierSurface::updateScreenPosition(Graphics& gfx, Vector2f screenDisplacement)
{
	if (!isInit)
		throw std::exception("You cannot update screen position to an uninitialized surface");

	vscBuff.screenDisplacement = screenDisplacement.getVector4();

	pVSCB->Update(gfx, vscBuff);

	curves.updateScreenPosition(gfx, screenDisplacement);
}

void		FourierSurface::updateCurves(Graphics& gfx, float phi, float theta)
{
	curves.updateShape(gfx, Coef, Ncoef, phi, theta);
}

float		FourierSurface::computeError(Polihedron* Poli, bool* cancel)
{
	const Vector3f* vertexs = Poli->getVertexs();
	const Vector3i* triangles = Poli->getTriangles();
	const unsigned int numTriangles = Poli->getNumTriangles();

	float absPoli = 0.f;
	float absDif = 0.f;

	for (unsigned int i = 0u; i < numTriangles; i++)
	{
		if (cancel && *cancel)
			break;

		Vector3f V1 = vertexs[triangles[i].x];
		Vector3f V2 = vertexs[triangles[i].y];
		Vector3f V3 = vertexs[triangles[i].z];

		Vector3f V12 = (V1 * V2 * V1).normalize();
		Vector3f V13 = (V1 * V3 * V1).normalize();

		float angle1 = acosf(V13 ^ V12);

		Vector3f V21 = (V2 * V1 * V2).normalize();
		Vector3f V23 = (V2 * V3 * V2).normalize();

		float angle2 = acosf(V23 ^ V21);

		Vector3f V32 = (V3 * V2 * V3).normalize();
		Vector3f V31 = (V3 * V1 * V3).normalize();

		float angle3 = acosf(V31 ^ V32);

		Vector3f center = (V1 + V2 + V3) / 3.f;
		float area = angle1 + angle2 + angle3 - pi;
		float distance = center.abs();
		center.normalize();

		float value = 0.f;
		for (unsigned int j = 0u; j < Ncoef; j++)
			value += Coef[j].C * Functions::Ylm(center, Coef[j].L, Coef[j].M);

		absPoli += area * distance * distance;
		absDif += area * (distance - value) * (distance - value);
	}

	return absDif / absPoli;
}

Quaternion	FourierSurface::getRotation()
{
	return vscBuff.rotation;
}

FourierSurface::Vertex* FourierSurface::getVertexPtr()
{
	return Vertexs;
}

Coefficient* FourierSurface::getCoefficients()
{
	return Coef;
}

unsigned int	FourierSurface::getNcoefficients()
{
	return Ncoef;
}

unsigned int	FourierSurface::getNvertexs()
{
	return nvertexs;
}

unsigned int* FourierSurface::getTrianglesIcosphere()
{
	return trianglesIcosphere;
}

unsigned int	FourierSurface::getNtriangles()
{
	return ntriangles;
}

void			FourierSurface::DrawCurves(Graphics& gfx)
{
	curves.Draw(gfx);
}

void			FourierSurface::generateDataSet()
{
	std::thread(Functions::generateHarmonicsAsync).detach();
}

unsigned int	FourierSurface::depthDataset()
{
	if (!Functions::DatasetYlmi[0]) return 0;
	int l = -1;
	while (++l <= (int)Functions::maxL && Functions::DatasetYlmi[l]);
	return l - 1;
}

//	Curves

//	Private static

void FourierSurface::Curves::generatePhiCurveAsync(const unsigned int t0, const unsigned int t1, const Coefficient* coef, const unsigned int ncoef, const float phi, const float theta, Vertex* V)
{
	constexpr float error = 1.0035f;

	for (unsigned int j = t0; j <= t1; j++)
	{
		float iphi = j * 2 * pi / Npoints + phi;
		float Z = 0.f;
		for (unsigned int k = 0; k < ncoef; k++)
			Z += coef[k].C * Functions::Ylm(coef[k].L, coef[k].M, iphi, theta);

		V[unsigned int(theta * Npoints / pi) + j + 1] = { (error * Z * Vector3f(sinf(theta) * cosf(iphi), sinf(theta) * sinf(iphi), cosf(theta))).getVector4(), { 1.f, 1.f, 1.f, 1.f } };
	}
}

void FourierSurface::Curves::generateThetaCurveAsync(const unsigned int t0, const unsigned int t1, const Coefficient* coef, const unsigned int ncoef, const float phi, const float theta, Vertex* V)
{
	constexpr float error = 1.0035f;

	for (unsigned int i = t0; i <= t1; i++)
	{
		float itheta = i * pi / Npoints;
		float Y = 0.f;

		for (unsigned int k = 0; k < ncoef; k++)
			Y += coef[k].C * Functions::Ylm(coef[k].L, coef[k].M, phi, itheta);

		if (i <= unsigned int(theta * Npoints / pi))
			V[i] = { (error * Y * Vector3f(sinf(itheta) * cosf(phi), sinf(itheta) * sinf(phi), cosf(itheta))).getVector4(), { 1.f, 1.f, 1.f, 1.f } };
		else
			V[i + Npoints + 1] = { (error * Y * Vector3f(sinf(itheta) * cosf(phi), sinf(itheta) * sinf(phi), cosf(itheta))).getVector4(), { 1.f, 1.f, 1.f, 1.f } };
	}
}

//	Construction

void FourierSurface::Curves::create(Graphics& gfx, const Coefficient* coef, const unsigned int ncoef, const float phi, const float theta, std::mutex* mtx)
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

	if(mtx)
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

void FourierSurface::Curves::updateShape(Graphics& gfx, const Coefficient* coef, const unsigned int ncoef, const float phi, const float theta)
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

void FourierSurface::Curves::updateRotation(Graphics& gfx, Quaternion rotation, bool multiplicative)
{
	if (!multiplicative)
		vscBuff.rotation = rotation;
	else
		vscBuff.rotation *= rotation;

	vscBuff.rotation.normalize();
	pVSCB->Update(gfx, vscBuff);
}

void FourierSurface::Curves::updateScreenPosition(Graphics& gfx, Vector2f screenDisplacement)
{
	vscBuff.screenDisplacement = screenDisplacement.getVector4();

	pVSCB->Update(gfx, vscBuff);
}

//	Creation Functions

void createShape(const char* filename)
{
	/*int n = 21;
	Vector3f* vertexs = (Vector3f*)calloc(n * n * 6, sizeof(Vector3f));

	for (int m = 0; m < 6; m++)
	{
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				float x = -1.f + 2.f * i / (n - 1.f);
				float y = -1.f + 2.f * j / (n - 1.f);
				switch (m)
				{
				case 0:
					vertexs[n * n * m + n * i + j] = Vector3f(1.f, x, y);
					break;
				case 1:
					vertexs[n * n * m + n * i + j] = Vector3f(-1.f, x, y);
					break;
				case 2:
					vertexs[n * n * m + n * i + j] = Vector3f(x, 1.f, y);
					break;
				case 3:
					vertexs[n * n * m + n * i + j] = Vector3f(x, -1.f, y);
					break;
				case 4:
					vertexs[n * n * m + n * i + j] = Vector3f(x, y, 1.f);
					break;
				case 5:
					vertexs[n * n * m + n * i + j] = Vector3f(x, y, -1.f);
					break;
				default:
					break;
				}
			}
		}
	}

	Vector3i* triangles = (Vector3i*)calloc((n - 1) * (n - 1) * 12, sizeof(Vector3i));

	for (int m = 0; m < 6; m++)
	{
		for (int i = 0; i < n - 1; i++)
		{
			for (int j = 0; j < n - 1; j++)
			{
				triangles[2 * m * (n - 1) * (n - 1) + 2 * i * (n - 1) + 2 * j] = Vector3i(n * n * m + n * i + j, n * n * m + n * (i + 1) + j, n * n * m + n * i + (j + 1));
				triangles[2 * m * (n - 1) * (n - 1) + 2 * i * (n - 1) + 2 * j + 1] = Vector3i(n * n * m + n * (i + 1) + (j + 1), n * n * m + n * (i + 1) + j, n * n * m + n * i + (j + 1));
			}
		}
	}*/

	constexpr unsigned int depth = 4;

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

#pragma region create icosahedron

	float gold = (1.f + sqrtf(5) / 2.f);

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

#pragma endregion

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

	Vector3i* trianglesIcosphere = (Vector3i*)calloc(C, sizeof(Vector3i));
	for (unsigned int i = 0; i < C; i++)
	{
		unsigned int aris0 = abs(triangles[i].a0) - 1;
		bool ornt0 = (triangles[i].a0 > 0) ? true : false;
		unsigned int aris1 = abs(triangles[i].a1) - 1;
		bool ornt1 = (triangles[i].a1 > 0) ? true : false;
		unsigned int aris2 = abs(triangles[i].a2) - 1;
		bool ornt2 = (triangles[i].a2 > 0) ? true : false;

		trianglesIcosphere[i].x = ornt0 ? aristas[aris0].v0 : aristas[aris0].v1;
		trianglesIcosphere[i].y = ornt1 ? aristas[aris1].v0 : aristas[aris1].v1;
		trianglesIcosphere[i].z = ornt2 ? aristas[aris2].v0 : aristas[aris2].v1;

	}

	for (unsigned int i = 0; i < V; i++)
	{
		vertexs[i].normalize();
		vertexs[i] = figureFunction(vertexs[i]);
	}

	FILE* file = fopen((FIGURES_DIR + std::string(filename) + ".dat").c_str(), "w");
	if (!file)
		throw std::exception("Couldn't create the file");

	fprintf(file, "Vertexs:\n");

	for (unsigned int i = 0; i < V; i++)
		fprintf(file, "(%f,%f,%f)\n", vertexs[i].x, vertexs[i].y, vertexs[i].z);

	fprintf(file, "Triangles:");
	for (unsigned int i = 0; i < C; i++)
		fprintf(file, "\n%i,%i,%i", trianglesIcosphere[i].x, trianglesIcosphere[i].y, trianglesIcosphere[i].z);

	fclose(file);
}

Vector3f figureFunction(Vector3f s2vec)
{
	if (s2vec.z == 1.f || s2vec.z == -1.f)
		return s2vec * 0.65f;

	float costheta = s2vec.z;
	float sintheta = sqrtf(1 - s2vec.z * s2vec.z);
	float cosphi = s2vec.x / sintheta;
	float sinphi = s2vec.y / sintheta;

	s2vec *= 0.65f;

	float sin5phi = FourierSurface::Functions::Uchev(4, cosphi) * sinphi;
	float sin15phi = FourierSurface::Functions::Uchev(14, cosphi) * sinphi;
	float sin25phi = FourierSurface::Functions::Uchev(24, cosphi) * sinphi;

	float factor = (sin5phi - 0.111f * sin15phi + 0.04f * sin25phi + 3.f);

	s2vec.x *= factor;
	s2vec.y *= factor;

	return s2vec;
}

void formatFile(const char* srcFile, const char* newFile)
{
	FILE* file = fopen((FIGURES_DIR + std::string(srcFile) + ".txt").c_str(), "r");
	if (!file)
		throw std::exception(("Couldn't open the file " + std::string(srcFile) + ".txt").c_str());

	Vector3f* V = (Vector3f*)calloc(3000, sizeof(Vector3f));
	Vector3f deviation = { 0.f,0.f,0.f };
	float meanradius = 0.f;
	int i = 0;
	while (fscanf(file, "%f %f %f\n\n", &V[i].x, &V[i].y, &V[i].z) != EOF)
	{
		deviation += V[i];
		meanradius += V[i].abs();
		i++;
	}
	deviation /= float(i);
	meanradius /= float(i);

	for (int j = 0; j < i; j++)
	{
		V[j] -= deviation;
		V[j] /= meanradius / 2.f;
	}

	//Vector3i* V = (Vector3i*)calloc(5000, sizeof(Vector3i));
	//int i = 0;
	//while (fscanf(file, "%i %i %i\n\n", &V[i].x, &V[i].y, &V[i].z) != EOF)i++;

	fclose(file);

	file = fopen((FIGURES_DIR + std::string(newFile) + ".dat").c_str(), "w");

	for (int j = 0; j < i; j++)
		fprintf(file, "\n(%f,%f,%f)", V[j].x, V[j].y, V[j].z);

	//for (int j = 0; j < i; j++)
	//	fprintf(file, "\n%i,%i,%i", V[j].x, V[j].y, V[j].z);

	fclose(file);
}
