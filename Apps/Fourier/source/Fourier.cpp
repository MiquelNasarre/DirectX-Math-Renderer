#include "Fourier.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "IG_Fourier.h"

#include <string>

#define INTERPRET_FIGURE_VIEW(view, call) \
if (view == -1) harmonics.call; \
else if (view >= int(IG::NFIG)) Interpolations[view-IG::NFIG]->call; \
else if (view > -1) Figure[view]->call; \
else DataPlots[-view - 2]->call

#define INTERPRET_FIGURE_VIEW_NT(view, call) \
if (view == -1) harmonics.call; \
else if (view >= int(IG::NFIG)) Interpolations[view-IG::NFIG]->call; \
else if (view > -1) Figure[view]->call

#pragma region ImGui Data Initializations

int				IG::L = 7;
int				IG::M = -4;
float			IG::theta = 0.f;
float			IG::phi = 0.f;
bool			IG::UPDATE = false;
bool			IG::UPDATE_CURVES = false;
bool			IG::CURVES = false;
bool			IG::MENU = true;
bool			IG::COEF_VIEW = false;

bool 			IG::DOUBLE_VIEW = false;
int 			IG::VIEW1 = -1;
int 			IG::VIEW2 = -1;
Vector2i*		IG::PAIRS = NULL;
unsigned int	IG::PAIRS_SIZE = 0u;
bool			IG::ALREADY_EXISTS = false;
int				IG::COPY = -1;
bool			IG::SAVE = false;
char*			IG::SAVE_NAME = NULL;
bool			IG::DELETE_VIEW = false;

bool			IG::CALCULATE_FIGURE = false;
bool			IG::FIGURE_FILE = true;
bool			IG::LOADING = false;
char*			IG::FILENAME = (char*)calloc(100, sizeof(char));
unsigned int	IG::MAXL = 0u;
unsigned short	IG::TDEPTH = 0u;
unsigned int	IG::NFIG = 0u;
unsigned int	IG::NPLOT = 0u;
Vector2i		IG::WindowDim = { 0, 0 };

unsigned int	IG::NINT = 0u;
float			IG::TVALUE = 0.f;
bool			IG::PLAY = false;
float			IG::PLAY_SPEED = 0.01f;
int**			IG::I_DATA = NULL;
unsigned int*	IG::I_DATA_SIZE = NULL;
bool			IG::I_ADD = false;
int				IG::ADD_FIGURE = -1;
int				IG::DELETE_FIGURE = -1;

bool			IG::COMPUTE_ERROR = false;
bool			IG::ERROR_WINDOW = false;
float*			IG::COMPUTED_ERRORS = NULL;

bool			IG::UPDATE_TEXTURE = false;
_float4color	IG::TEXTURE = { -1.f,0.f,0.f,0.f };

int						IG::UPDATE_LIGHT = -1;
IG::lightsource*		IG::LIGHTS = (IG::lightsource*)calloc(sizeof(IG::lightsource), 8);

#pragma endregion

//	Coefficient window

void Fourier::createCoefWindow()
{
}

void Fourier::updateCoefWindow()
{
}

void Fourier::deleteCoefWindow()
{
}

//	Main Window

Fourier::Fourier()
	: window(960, 720, "Fourier", "", true)
{
	window.setFramerateLimit(60);

	IG::LIGHTS[0].is_on = true;
	IG::LIGHTS[0].color = { 1.f, 1.f, 1.f, 1.f };
	IG::LIGHTS[0].intensities = { 600.f, 320.f, 0.f, 0.f };
	IG::LIGHTS[0].position = { 30.f, 10.f, 20.f, 0.f };

	harmonics.create(window.graphics, &C, 1u);
	
	//createShape("test");
}

int Fourier::Run()
{
	while (window.processEvents())
		doFrame();
	return 0;
}

void Fourier::drag_dynamic_space()
{
	Vector3f obs = window.graphics.getObserver();
	Vector3f ex = -(obs * Vector3f(0.f, 0.f, 1.f)).normalize();
	Vector3f ey = ex * obs;

	Vector3f lastMouse = (-obs + (ex * (lastPos.x - window.getDimensions().x / 2) + ey * (lastPos.y - window.getDimensions().y / 2)) / scale).normalize();
	lastPos = Mouse::getPosition();
	Vector3f newMouse = (-obs + (ex * (lastPos.x - window.getDimensions().x / 2) + ey * (lastPos.y - window.getDimensions().y / 2)) / scale).normalize();

	Vector3f axis0 = lastMouse * newMouse;
	if (!axis0) axis0 = newMouse;
	axis0.normalize();
	float dangle0 = -acosf(lastMouse ^ newMouse);
	if ((lastMouse ^ newMouse) > 1.f)dangle0 = 0.f;
	if (lastMouse == newMouse) dangle0 = 0.f;

	constexpr float s = 1.f / 1.5f;

	Quaternion newRot = rotationQuaternion(newMouse, Mouse::getWheel() / 18000.f) * rotationQuaternion(axis0, dangle0) * rotationQuaternion(axis, (dangle * (1 - s + fabs(axis ^ newMouse) * s)));

	dangle = 2 * acosf(newRot.r);
	axis = newRot.getVector();
	if (!axis)axis = newMouse;
	axis.normalize();
}

void Fourier::drag_dynamic_plane()
{
	Vector3f obs = window.graphics.getObserver();
	Vector3f ex = -(obs * Vector3f(0.f, 0.f, 1.f)).normalize();
	Vector3f ey = ex * obs;

	Vector2i movement = Mouse::getPosition() - lastPos;
	lastPos = Mouse::getPosition();

	Vector3f desiredAxis;
	float desiredAngle;

	if (!movement)
	{
		desiredAxis = obs;
		desiredAngle = Mouse::getWheel() / 18000.f;
		dangle *= 0.90f;
	}
	else
	{
		desiredAxis = (movement.y * ex - movement.x * ey).normalize();
		desiredAngle = 2 * movement.abs() / scale;
	}

	Vector3f preAxis = axis;
	axis = (axis * dangle + desiredAxis * desiredAngle);
	if (axis)axis.normalize();
	else axis = desiredAxis;
	dangle *= axis ^ preAxis;
	dangle += (desiredAxis ^ axis) * (desiredAngle - dangle) / 20.f;

	if (dangle > 0.2f)dangle = 0.2f;
	if (dangle < -0.2f)dangle = -0.2f;
}

void Fourier::magneticReturn()
{
	if (rotation.r < 0)rotation = -rotation;
	constexpr float pull = 0.03f;

	float angle = 2 * acosf(rotation.r);
	Vector3f axisRot = rotation.getVector();

	Quaternion newRot;
	if (angle < pull)
		newRot = rotationQuaternion(axisRot, -pull / 40.f) * rotationQuaternion(axis, dangle);
	else
		newRot = rotationQuaternion(axisRot, -angle / 40.f) * rotationQuaternion(axis, dangle);
	
	axis = newRot.getVector();
	dangle = 2 * acos(newRot.r);

	dangle *= 0.95f;

	if ((fabs(angle) < 0.01f && fabs(dangle) < 0.005f))
	{
		rotation = 1.f;
		axis = { 1.f,0.f,0.f };
		dangle = 0.f;
		returning = false;

	}
}

void Fourier::strictReturn()
{
	dangle = 0.f;

	if (rotation.r < 0)rotation = -rotation;

	float angle = 2 * acosf(rotation.r);
	Vector3f axisRot = rotation.getVector();

	axis = axisRot;
	dangle = -powf(angle, 0.65f) / 40.f;

	if (fabs(angle) < 0.01f)
	{
		rotation = 1.f;
		strict = false;
		dangle = 0.f;
	}
}

void Fourier::eventManager()
{
	//	Enable disable ImGui

	static bool persists = false;
	if (Keyboard::isKeyPressed('M'))
	{
		if (persists == false)
		{
			if (IG::MENU)
				IG::MENU = false;
			else
				IG::MENU = true;
		}
		persists = true;
	}
	else
		persists = false;

	//	Cowfficient View
	
	static bool exists = false;
	if (IG::COEF_VIEW)
	{
		if(!exists)
			createCoefWindow();
		exists = true;
	}
	else
	{
		if (exists)
			deleteCoefWindow();
		exists = false;
	}

	// Full screen

	if (Keyboard::isKeyPressed(VK_F11))
		window.setFullScreen(true);

	if (Keyboard::isKeyPressed(VK_ESCAPE))
		window.setFullScreen(false);

	// returning motion

	if (Keyboard::isKeyPressed('R'))
		magneticReturn();

	if (Keyboard::isKeyPressed('T'))
		strictReturn();

	if (Keyboard::isKeyPressed('S'))
		dangle = 0.f;

	// Quaternion motion

	if (Mouse::isButtonPressed(Mouse::Left) && !dragging)
	{
		dragging = true;
		lastPos = Mouse::getPosition();
	}

	if (dragging && !Mouse::isButtonPressed(Mouse::Left))
		dragging = false;

	if (dragging)
	{
		if (IG::DOUBLE_VIEW)
			drag_dynamic_plane();
		else
			drag_dynamic_space();
	}

	else
		scale *= powf(1.1f, Mouse::getWheel() / 120.f);

	rotation *= rotationQuaternion(axis, dangle * window.getFrameTime() * 60.f);

	IG::WindowDim = window.getDimensions();

	if (!(rotation.abs() < 10.f))
	{
		rotation = 1.f;
		axis = { 1.f,1.f,1.f };
		dangle = 0.f;
	}

	// Multithread

	if (IG::CALCULATE_FIGURE)
	{
		IG::CALCULATE_FIGURE = false;
		IG::LOADING = true;

		FourierSurface** tFigure = (FourierSurface**)calloc(IG::NFIG + 1, sizeof(void*));
		for (unsigned int i = 0; i < IG::NFIG; i++)
			tFigure[i] = Figure[i];
		if(IG::NFIG)
			free(Figure);
		Figure = tFigure;
		Figure[IG::NFIG] = new(FourierSurface);

		if (IG::FIGURE_FILE)
		{
			extractedFigure = FourierSurface::FileManager::extractFigureFromFile(IG::FILENAME);

			if (!IG::ALREADY_EXISTS)
			{
				Polihedron** tPlots = (Polihedron**)calloc(IG::NPLOT + 1, sizeof(void*));
				for (unsigned int i = 0; i < IG::NPLOT; i++)
					tPlots[i] = DataPlots[i];
				if (IG::NPLOT)
					free(DataPlots);
				DataPlots = tPlots;
				DataPlots[IG::NPLOT] = new(Polihedron);
				std::thread(createPlotAsync, &window.graphics, DataPlots[IG::NPLOT], extractedFigure, &Fplot, &mtx).detach();
			}
			else
			{
				Fplot = true;
			}

			std::thread(calculateCoefficientsAsync, &coef, extractedFigure, IG::MAXL, &Fcoef).detach();
			std::thread(createFigureAsync, &window.graphics, Figure[IG::NFIG], &coef, (IG::MAXL + 1) * (IG::MAXL + 1), &Ffigu, &mtx, &Fcoef).detach();
		}
		else
		{
			coef = FourierSurface::FileManager::loadCoefficientsFromFile(IG::FILENAME);
			Fcoef = true;
			std::thread(createFigureAsync, &window.graphics, Figure[IG::NFIG], &coef, FourierSurface::FileManager::ncoefFromFile(IG::FILENAME), &Ffigu, &mtx, &Fcoef).detach();
		}
	}

	if (Ffigu && !IG::FIGURE_FILE)
	{
		Ffigu = false;
		IG::NFIG++;
		IG::LOADING = false;
		IG::VIEW1 = IG::NFIG - 1;
		IG::UPDATE_CURVES = true;
		IG::UPDATE_LIGHT = -2;
		IG::DOUBLE_VIEW = false;

		free(coef);
	}

	if (Ffigu && Fplot)
	{
		Ffigu = false;
		Fplot = false;

		IG::NFIG++;
		IG::DOUBLE_VIEW = true;
		IG::LOADING = false;
		IG::VIEW1 = IG::NFIG - 1;
		IG::UPDATE_CURVES = true;
		IG::UPDATE_LIGHT = -2;

		if (IG::ALREADY_EXISTS)
		{
			IG::VIEW2 = IG::COPY;
			IG::ALREADY_EXISTS = false;
			free((void*)extractedFigure[0]);
			free((void*)extractedFigure[1]);
		}
		else
		{
			IG::NPLOT++;
			IG::VIEW2 = -int(IG::NPLOT) - 1;
		}

		free((void*)extractedFigure[2]);
		free(extractedFigure);
		free(coef);
	}

	//	Interpolation

	if (IG::I_ADD)
	{
		InterpolatedString** temp = (InterpolatedString**)calloc(IG::NINT + 1, sizeof(void*));
		for (unsigned int i = 0; i < IG::NINT; i++)
			temp[i] = Interpolations[i];
		if (Interpolations)
			free(Interpolations);
		Interpolations = temp;

		Interpolations[IG::NINT] = new(InterpolatedString);

		IG::DOUBLE_VIEW = false;
		IG::I_ADD = false;
		IG::VIEW1 = IG::NINT + IG::NFIG;
		IG::NINT++;
		IG::TEXTURE = { -1.f,0.f,0.f,0.f };
		IG::UPDATE_LIGHT = -2;
	}

	if (IG::ADD_FIGURE != -1)
	{
		Interpolations[IG::VIEW1 - IG::NFIG]->addSurface(window.graphics, Figure[IG::ADD_FIGURE]);
		IG::ADD_FIGURE = -1;
		IG::UPDATE_LIGHT = -2;
		IG::UPDATE_TEXTURE = true;
		IG::UPDATE_CURVES = true;
	}

	if (IG::DELETE_FIGURE != -1)
	{
		Interpolations[IG::VIEW1 - IG::NFIG]->deleteSurface(window.graphics, IG::DELETE_FIGURE);
		IG::DELETE_FIGURE = -1;
		IG::UPDATE_LIGHT = -2;
		IG::UPDATE_TEXTURE = true;
		IG::UPDATE_CURVES = true;
	}

	//	Delete view

	if (IG::DELETE_VIEW)
	{
		IG::DELETE_VIEW = false;

		if (IG::VIEW1 >= int(IG::NFIG))
		{
			Interpolations[IG::VIEW1 - IG::NFIG]->~InterpolatedString();
			free(Interpolations[IG::VIEW1 - IG::NFIG]);

			for (unsigned int i = IG::VIEW1 - IG::NFIG; i < IG::NINT - 1; i++)
				Interpolations[i] = Interpolations[i + 1];

			IG::VIEW1 = -1;
			IG::NINT--;

			if (IG::NINT == 0)
			{
				free(Interpolations);
				Interpolations = NULL;
			}
		}
		else if (IG::VIEW1 > -1)
		{
			Figure[IG::VIEW1]->~FourierSurface();
			free(Figure[IG::VIEW1]);
			for (unsigned int i = IG::VIEW1; i < IG::NFIG - 1; i++)
				Figure[i] = Figure[i + 1];

			IG::NFIG--;
			IG::VIEW1 = -1;
			IG::DOUBLE_VIEW = false;

			if (IG::NFIG == 0)
			{
				free(Figure);
				Figure = NULL;
			}
		}
		else
		{
			unsigned int p = -IG::VIEW1 - 2;
			DataPlots[p]->~Polihedron();
			free(DataPlots[p]);
			for (unsigned int i = p; i < IG::NPLOT - 1; i++)
				DataPlots[i] = DataPlots[i + 1];

			IG::NPLOT--;
			IG::VIEW1 = -1;
			IG::DOUBLE_VIEW = false;

			if (IG::NPLOT == 0)
			{
				free(DataPlots);
				DataPlots = NULL;
			}
		}
	}

	//	Interpolation T update

	if (IG::VIEW1 >= int(IG::NFIG))
	{
		if (IG::I_DATA_SIZE[IG::VIEW1 - IG::NFIG] < 2)
		{
			IG::TVALUE = 0.f;
			IG::PLAY = false;
		}
		else
		{
			if (IG::PLAY)
			{
				if (IG::CURVES)
					IG::UPDATE_CURVES = true;
				IG::TVALUE += IG::PLAY_SPEED;
			}
			while (IG::TVALUE > float(IG::I_DATA_SIZE[IG::VIEW1 - IG::NFIG]) - 1.f)IG::TVALUE -= IG::I_DATA_SIZE[IG::VIEW1 - IG::NFIG] - 1;
			while (IG::TVALUE < 0.f) IG::TVALUE += IG::I_DATA_SIZE[IG::VIEW1 - IG::NFIG] - 1;
		}
		Interpolations[IG::VIEW1 - IG::NFIG]->updateInterpolation(window.graphics, IG::TVALUE);
	}

	//	Shape updates

	if (IG::UPDATE)
	{
		IG::UPDATE = false;

		C.L = (unsigned int)IG::L;
		C.M = IG::M;

		harmonics.updateShape(window.graphics, &C, 1u);

		if (IG::CURVES)
			harmonics.updateCurves(window.graphics, IG::phi, IG::theta);
	}

	if (IG::UPDATE_CURVES)
	{
		IG::UPDATE_CURVES = false;

		INTERPRET_FIGURE_VIEW_NT(IG::VIEW1, updateCurves(window.graphics, IG::phi, IG::theta));

		if (IG::DOUBLE_VIEW)
			{INTERPRET_FIGURE_VIEW_NT(IG::VIEW2, updateCurves(window.graphics, IG::phi, IG::theta));}
	}

	//	Set lights & textures

	int l = IG::UPDATE_LIGHT;
	if (l == -2) {
		for (int i = 0; i < 8; i++) {
			harmonics.updateLight(window.graphics, i, IG::LIGHTS[i].intensities, IG::LIGHTS[i].color, IG::LIGHTS[i].position);
			for (unsigned int j = 0; j < IG::NFIG; j++)
				Figure[j]->updateLight(window.graphics, i, IG::LIGHTS[i].intensities, IG::LIGHTS[i].color, IG::LIGHTS[i].position);
			for (unsigned int j = 0; j < IG::NINT; j++)
				Interpolations[j]->updateLight(window.graphics, i, IG::LIGHTS[i].intensities, IG::LIGHTS[i].color, IG::LIGHTS[i].position);
		}
		IG::UPDATE_LIGHT = -1;
	}
	if (l > -1) {
		harmonics.updateLight(window.graphics, l, IG::LIGHTS[l].intensities, IG::LIGHTS[l].color, IG::LIGHTS[l].position);
		for (unsigned int j = 0; j < IG::NFIG; j++)
			Figure[j]->updateLight(window.graphics, l, IG::LIGHTS[l].intensities, IG::LIGHTS[l].color, IG::LIGHTS[l].position);
		for (unsigned int j = 0; j < IG::NINT; j++)
			Interpolations[j]->updateLight(window.graphics, l, IG::LIGHTS[l].intensities, IG::LIGHTS[l].color, IG::LIGHTS[l].position);
		IG::UPDATE_LIGHT = -1;
	}

	if (IG::UPDATE_TEXTURE)
	{
		IG::UPDATE_TEXTURE = false;
		if (IG::TEXTURE.r == -1.f)
		{
			INTERPRET_FIGURE_VIEW_NT(IG::VIEW1, updateTexture(window.graphics, Color::White, true));
			if(IG::DOUBLE_VIEW)
				{INTERPRET_FIGURE_VIEW_NT(IG::VIEW2, updateTexture(window.graphics, Color::White, true));}
		}
		else if (IG::TEXTURE.g == -1.f)
		{
			INTERPRET_FIGURE_VIEW_NT(IG::VIEW1, updateTexture(window.graphics, Color::White, false, true));
			if(IG::DOUBLE_VIEW)
				{INTERPRET_FIGURE_VIEW_NT(IG::VIEW2, updateTexture(window.graphics, Color::White, false, true));}
		}
		else
		{
			INTERPRET_FIGURE_VIEW_NT(IG::VIEW1, updateTexture(window.graphics, Color((float*)&IG::TEXTURE)));
			if (IG::DOUBLE_VIEW)
				{INTERPRET_FIGURE_VIEW_NT(IG::VIEW2, updateTexture(window.graphics, Color((float*)&IG::TEXTURE)));}
		}

	}

	//	Double view

	if (IG::DOUBLE_VIEW)
	{
		INTERPRET_FIGURE_VIEW(IG::VIEW1, updateScreenPosition(window.graphics, {-0.5f, 0.f }));
		INTERPRET_FIGURE_VIEW(IG::VIEW2, updateScreenPosition(window.graphics, { 0.5f, 0.f }));
	}
	else
		{INTERPRET_FIGURE_VIEW(IG::VIEW1, updateScreenPosition(window.graphics, { 0.f, 0.f }));}

	// Error Computing

	static bool* computations = NULL;
	static bool* cancelations = NULL;
	if (IG::ERROR_WINDOW)
	{
		if (IG::COMPUTE_ERROR)
		{
			IG::COMPUTE_ERROR = false;
			IG::COMPUTED_ERRORS = (float*)calloc(IG::PAIRS_SIZE, sizeof(float));
			computations = (bool*)calloc(IG::PAIRS_SIZE, sizeof(bool));
			cancelations = (bool*)calloc(IG::PAIRS_SIZE, sizeof(bool));

			for (unsigned int i = 0u; i < IG::PAIRS_SIZE; i++)
				std::thread(computeErrorsAsync, Figure[IG::PAIRS[i].x], DataPlots[-IG::PAIRS[i].y - 2], &IG::COMPUTED_ERRORS[i], &computations[i], &cancelations[i]).detach();
		}
	}
	else if (cancelations)
	{
		bool done = true;
		for (unsigned int i = 0; i < IG::PAIRS_SIZE; i++)
		{
			cancelations[i] = true;
			if (computations[i] == false)
				done = false;
		}
		if (done)
		{
			free(cancelations);
			free(computations);
			free(IG::COMPUTED_ERRORS);
			IG::COMPUTED_ERRORS = NULL;
			cancelations = NULL;
			computations = NULL;
		}

	}

	//	Save file

	if (IG::SAVE)
	{
		IG::SAVE = false;
		if (IG::VIEW1 >= int(IG::NFIG))
			Interpolations[IG::VIEW1 - IG::NFIG]->saveCoefficients(IG::SAVE_NAME);
		else
			Figure[IG::VIEW1]->saveCoefficients(IG::SAVE_NAME);
	}

}

void Fourier::doFrame()
{
	eventManager();

	//	Update objects

	window.setTitle("Fourier  -  " + std::to_string(int(std::round(window.getFramerate()))) + "fps");

	window.graphics.updatePerspective(observer, center, scale);

	INTERPRET_FIGURE_VIEW(IG::VIEW1, updateRotation(window.graphics, rotation));

	if(IG::DOUBLE_VIEW)
		{INTERPRET_FIGURE_VIEW(IG::VIEW2, updateRotation(window.graphics, rotation));}

	//	Rendering

	window.graphics.clearBuffer(Color::Black);

	INTERPRET_FIGURE_VIEW(IG::VIEW1, Draw(window.graphics));

	if(IG::DOUBLE_VIEW)
		{INTERPRET_FIGURE_VIEW(IG::VIEW2, Draw(window.graphics));}

	if (IG::CURVES)
		{INTERPRET_FIGURE_VIEW_NT(IG::VIEW1, DrawCurves(window.graphics));}

	if(IG::CURVES && IG::DOUBLE_VIEW)
		{INTERPRET_FIGURE_VIEW_NT(IG::VIEW2, DrawCurves(window.graphics));}


	//	ImGui crap

	IG_Fourier::render();

	//	Push the frame to the scriin

	window.graphics.pushFrame();
}

//  Threading utilities

void Fourier::createPlotAsync(Graphics* gfx, Polihedron* dataplot, const void** extractedFigure, bool* done, std::mutex* mtx)
{

	const Vector3f* vertexs = (Vector3f*)extractedFigure[0];
	const Vector3i* triangles = (Vector3i*)extractedFigure[1];
	int numT = ((int*)extractedFigure[2])[0];
	int numV = ((int*)extractedFigure[2])[1];

	dataplot->create(*gfx, vertexs, triangles, numT, nullptr, false, true, true, mtx);
	*done = true;
}

void Fourier::createFigureAsync(Graphics* gfx, FourierSurface* figure, Coefficient** coef, unsigned int ncoef, bool* done, std::mutex* mtx, bool* begin)
{
	while (begin && !(*begin))
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	*begin = false;

	// Crea la figura
	figure->create(*gfx, *coef, ncoef, mtx);
	*done = true;
}

void Fourier::calculateCoefficientsAsync(Coefficient** coef, const void** extractedFigure, unsigned int maxL, bool* done)
{
	*coef = FourierSurface::Functions::calculateCoefficients(extractedFigure, maxL, IG::TDEPTH);
	*done = true;
}

void Fourier::computeErrorsAsync(FourierSurface* surface, Polihedron* poli, float* result, bool* finished, bool* cancel)
{
	float temp = surface->computeError(poli, cancel);
	*result = temp;
	*finished = true;
}
