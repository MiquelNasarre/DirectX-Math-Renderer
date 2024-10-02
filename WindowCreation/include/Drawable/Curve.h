#pragma once
#include "Drawable.h"

class Curve : public Drawable
{
public:
	Curve() {}
	Curve(Graphics& gfx, Vector3f F(float), Vector2f rangeT, UINT Npoints, Color color = Color::White, bool transparency = false);
	Curve(Graphics& gfx, Vector3f F(float), Vector2f rangeT, UINT Npoints, std::vector<Color> colors, bool transparency = false);

	template<typename C>
	Curve(Graphics& gfx, Vector3f F(float, const C&), const C& param, Vector2f rangeT, UINT Npoints, Color color = Color::White, bool transparency = false);
	template<typename C>
	Curve(Graphics& gfx, Vector3f F(float, const C&), const C& param, Vector2f rangeT, UINT Npoints, std::vector<Color> colors, bool transparency = false);

	void create(Graphics& gfx, Vector3f F(float), Vector2f rangeT, UINT Npoints, Color color = Color::White, bool transparency = false);
	void create(Graphics& gfx, Vector3f F(float), Vector2f rangeT, UINT Npoints, std::vector<Color> colors, bool transparency = false);

	template<typename C>
	void create(Graphics& gfx, Vector3f F(float, const C&), const C& param, Vector2f rangeT, UINT Npoints, Color color = Color::White, bool transparency = false);
	template<typename C>
	void create(Graphics& gfx, Vector3f F(float, const C&), const C& param, Vector2f rangeT, UINT Npoints, std::vector<Color> colors, bool transparency = false);

	void updateShape(Graphics& gfx, Vector3f F(float), Vector2f rangeT, UINT Npoints, Color color = Color::White);
	void updateShape(Graphics& gfx, Vector3f F(float), Vector2f rangeT, UINT Npoints, std::vector<Color> colors);

	template<typename C>
	void updateShape(Graphics& gfx, Vector3f F(float, const C&), const C& param, Vector2f rangeT, UINT Npoints, Color color = Color::White);
	template<typename C>
	void updateShape(Graphics& gfx, Vector3f F(float, const C&), const C& param, Vector2f rangeT, UINT Npoints, std::vector<Color> colors);

	void updateRotation(Graphics& gfx, float rotationX, float rotationY, float rotationZ);
	void updateRotation(Graphics& gfx, Vector3f axis, float angle, bool multiplicative = false);
	void updateRotation(Graphics& gfx, Quaternion rotation, bool multiplicative = false);
	void updatePosition(Graphics& gfx, Vector3f position, bool additive = false);

	Quaternion getRotation();
	Vector3f getPosition();

private:

	void addDefaultBinds(Graphics& gfx, bool transparency);

	struct Vertex {
		_float4vector position;
		_float4color color;
	};

	struct VSconstBuffer {
		_float4vector translation = { 0.f, 0.f, 0.f, 0.f };
		Quaternion rotation = 1.f;
	}vscBuff;

	ConstantBuffer<VSconstBuffer>* pVSCB;
};

template<typename C>
Curve::Curve(Graphics& gfx, Vector3f F(float, const C&), const C& param, Vector2f rangeT, UINT Npoints, Color color, bool transparency)
{
	create(gfx, F, param, rangeT, Npoints, color);
}

template<typename C>
Curve::Curve(Graphics& gfx, Vector3f F(float, const C&), const C& param, Vector2f rangeT, UINT Npoints, std::vector<Color> colors, bool transparency)
{
	create(gfx, F, param, rangeT, Npoints, colors);
}

template<typename C>
void Curve::create(Graphics& gfx, Vector3f F(float, const C&), const C& param, Vector2f rangeT, UINT Npoints, Color color, bool transparency)
{
	if (isInit)
		throw std::exception("You cannot create a curve over one that is already initialized");
	else
		isInit = true;

	std::vector<Vertex> vertexs;

	for (UINT i = 0; i <= Npoints; i++)
		vertexs.push_back(Vertex(F(rangeT.x + float(i) / Npoints * (rangeT.y - rangeT.x), param).getVector4(), color.getColor4()));

	std::vector<unsigned int> indexs;

	for (UINT i = 0; i <= Npoints; i++)
		indexs.push_back(i);

	AddBind(std::make_unique<VertexBuffer>(gfx, vertexs.data(), (unsigned int)vertexs.size()));
	AddBind(std::make_unique<IndexBuffer>(gfx, indexs.data(), (unsigned int)indexs.size()));

	addDefaultBinds(gfx, transparency);
}

template<typename C>
void Curve::create(Graphics& gfx, Vector3f F(float, const C&), const C& param, Vector2f rangeT, UINT Npoints, std::vector<Color> colors, bool transparency)
{
	if (isInit)
		throw std::exception("You cannot create a curve over one that is already initialized");
	else
		isInit = true;

	if (!colors.size())
		throw std::exception("The color vector for a curve must have at least one color!!");

	std::vector<Vertex> vertexs;

	for (UINT i = 0; i <= Npoints; i++) {
		float c = (colors.size() - 1) * float(i) / Npoints;
		UINT C0 = UINT(c);
		UINT C1 = (C0 + 1) % colors.size();
		c -= C0;

		vertexs.push_back(Vertex(
			F(rangeT.x + float(i) / Npoints * (rangeT.y - rangeT.x), param).getVector4(),
			(colors[C0] * (1 - c) + colors[C1] * c).getColor4()
		));
	}


	std::vector<unsigned short> indexs;

	for (UINT i = 0; i <= Npoints; i++)
		indexs.push_back(i);

	AddBind(std::make_unique<VertexBuffer>(gfx, vertexs.data(), (unsigned int)vertexs.size()));
	AddBind(std::make_unique<IndexBuffer>(gfx, indexs.data(), (unsigned int)indexs.size()));

	addDefaultBinds(gfx, transparency);
}

template<typename C>
void Curve::updateShape(Graphics& gfx, Vector3f F(float, const C&), const C& param, Vector2f rangeT, UINT Npoints, Color color)
{
	if (!isInit)
		throw std::exception("You cannot update the shape of a curve if you havent initialized it first");

	std::vector<Vertex> vertexs;

	for (UINT i = 0; i <= Npoints; i++)
		vertexs.push_back(Vertex(F(rangeT.x + float(i) / Npoints * (rangeT.y - rangeT.x), param).getVector4(), color.getColor4()));

	std::vector<unsigned short> indexs;

	for (UINT i = 0; i <= Npoints; i++)
		indexs.push_back(i);

	changeBind(std::make_unique<VertexBuffer>(gfx, vertexs.data(), (unsigned int)vertexs.size()), 0u);
	changeBind(std::make_unique<IndexBuffer>(gfx, indexs.data(), (unsigned int)indexs.size()), 1u);
}

template<typename C>
void Curve::updateShape(Graphics& gfx, Vector3f F(float, const C&), const C& param, Vector2f rangeT, UINT Npoints, std::vector<Color> colors)
{
	if (!isInit)
		throw std::exception("You cannot update the shape of a curve if you havent initialized it first");

	if (!colors.size())
		throw std::exception("The color vector for a curve must have at least one color!!");

	std::vector<Vertex> vertexs;

	for (UINT i = 0; i <= Npoints; i++) {
		float c = (colors.size() - 1) * float(i) / Npoints;
		UINT C0 = UINT(c);
		UINT C1 = (C0 + 1) % colors.size();
		c -= C0;

		vertexs.push_back(Vertex(
			F(rangeT.x + float(i) / Npoints * (rangeT.y - rangeT.x), param).getVector4(),
			(colors[C0] * (1 - c) + colors[C1] * c).getColor4()
		));
	}


	std::vector<unsigned short> indexs;

	for (UINT i = 0; i <= Npoints; i++)
		indexs.push_back(i);

	changeBind(std::make_unique<VertexBuffer>(gfx, vertexs.data(), (unsigned int)vertexs.size()), 0u);
	changeBind(std::make_unique<IndexBuffer>(gfx, indexs.data(), (unsigned int)indexs.size()), 1u);
}
