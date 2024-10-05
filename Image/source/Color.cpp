#include "Color.h"

const Color Color::Black		= Color(  0,  0,    0, 255);
const Color Color::White		= Color(255, 255, 255, 255);
const Color Color::Red			= Color(255,   0,   0, 255);
const Color Color::Green		= Color(  0, 255,   0, 255);
const Color Color::Blue			= Color(  0,   0, 255, 255);
const Color Color::Yellow		= Color(255, 255,   0, 255);
const Color Color::Cyan			= Color(  0, 255, 255, 255);
const Color Color::Purple		= Color(255,   0, 255, 255);
const Color Color::Gray			= Color(127, 127, 127, 255);
const Color Color::Orange		= Color(255, 127,   0, 255);
const Color Color::Transparent	= Color(  0,   0,   0,   0);

Color::Color()
{
	R = 0u;
	G = 0u;
	B = 0u;
	A = 0u;
}

Color::Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	:R{ r }, G{ g }, B{ b }, A{ a }
{
}

Color::Color(float* f)
{
	R = unsigned char(f[0] * 255);
	G = unsigned char(f[1] * 255);
	B = unsigned char(f[2] * 255);
	A = unsigned char(f[3] * 255);
}

Color Color::operator+(const Color& other) const
{
	int Ri = (int)R + (int)other.R;
	int Gi = (int)G + (int)other.G;
	int Bi = (int)B + (int)other.B;
	int Ai = (int)A + (int)other.A;
	if (Ri > 255)Ri = 255;
	if (Gi > 255)Gi = 255;
	if (Bi > 255)Bi = 255;
	if (Ai > 255)Ai = 255;
	return Color(Ri, Gi, Bi, Ai);
}

Color Color::operator-(const Color& other) const
{
	int Ri = (int)R - (int)other.R;
	int Gi = (int)G - (int)other.G;
	int Bi = (int)B - (int)other.B;
	int Ai = (int)A - (int)other.A;
	if (Ri < 0)Ri = 0;
	if (Gi < 0)Gi = 0;
	if (Bi < 0)Bi = 0;
	if (Ai < 0)Ai = 0;
	return Color(Ri, Gi, Bi, Ai);
}

Color Color::operator*(const Color& other) const
{
	int Ri = (int)R * (int)other.R;
	int Gi = (int)G * (int)other.G;
	int Bi = (int)B * (int)other.B;
	int Ai = (int)A * (int)other.A;
	Ri /= 255;
	Gi /= 255;
	Bi /= 255;
	Ai /= 255;
	return Color(Ri, Gi, Bi, Ai);
}

Color Color::operator/(const Color& other) const
{
	float Ri = (int)R / (float)other.R;
	float Gi = (int)G / (float)other.G;
	float Bi = (int)B / (float)other.B;
	float Ai = (int)A / (float)other.A;
	Ri *= 255;
	Gi *= 255;
	Bi *= 255;
	Ai *= 255;
	return Color((unsigned char)Ri, (unsigned char)Gi, (unsigned char)Bi, (unsigned char)Ai);
}

Color Color::operator*(const int& rhs) const
{
	int Ri = (int)R * rhs;
	int Gi = (int)G * rhs;
	int Bi = (int)B * rhs;
	int Ai = (int)A * rhs;
	if (Ri > 255)Ri = 255;
	if (Gi > 255)Gi = 255;
	if (Bi > 255)Bi = 255;
	if (Ai > 255)Ai = 255;
	return Color(Ri, Gi, Bi, Ai);
}

Color Color::operator/(const int& rhs) const
{
	int Ri = (int)R / rhs;
	int Gi = (int)G / rhs;
	int Bi = (int)B / rhs;
	int Ai = (int)A / rhs;
	return Color(Ri, Gi, Bi, Ai);
}

Color Color::operator*(const float& rhs) const
{
	float Ri = (int)R * rhs;
	float Gi = (int)G * rhs;
	float Bi = (int)B * rhs;
	float Ai = (int)A * rhs;
	if (Ri > 255)Ri = 255;
	if (Gi > 255)Gi = 255;
	if (Bi > 255)Bi = 255;
	if (Ai > 255)Ai = 255;
	return Color((unsigned char)Ri, (unsigned char)Gi, (unsigned char)Bi, (unsigned char)Ai);
}

Color Color::operator/(const float& rhs) const
{
	float Ri = (int)R / rhs;
	float Gi = (int)G / rhs;
	float Bi = (int)B / rhs;
	float Ai = (int)A / rhs;
	if (Ri > 255)Ri = 255;
	if (Gi > 255)Gi = 255;
	if (Bi > 255)Bi = 255;
	if (Ai > 255)Ai = 255;
	return Color((unsigned char)Ri, (unsigned char)Gi, (unsigned char)Bi, (unsigned char)Ai);
}

Color Color::operator*(const double& rhs) const
{
	float Ri = (int)R * (float)rhs;
	float Gi = (int)G * (float)rhs;
	float Bi = (int)B * (float)rhs;
	float Ai = (int)A * (float)rhs;
	if (Ri > 255)Ri = 255;
	if (Gi > 255)Gi = 255;
	if (Bi > 255)Bi = 255;
	if (Ai > 255)Ai = 255;
	return Color((unsigned char)Ri, (unsigned char)Gi, (unsigned char)Bi, (unsigned char)Ai);
}

Color Color::operator/(const double& rhs) const
{
	float Ri = (int)R / (float)rhs;
	float Gi = (int)G / (float)rhs;
	float Bi = (int)B / (float)rhs;
	float Ai = (int)A / (float)rhs;
	if (Ri > 255)Ri = 255;
	if (Gi > 255)Gi = 255;
	if (Bi > 255)Bi = 255;
	if (Ai > 255)Ai = 255;
	return Color((unsigned char)Ri, (unsigned char)Gi, (unsigned char)Bi, (unsigned char)Ai);
}

Color Color::operator-() const
{
	return Color(255 - R, 255 - G, 255 - B, A);
}

Color& Color::operator+=(const Color& other)
{
	*this = *this + other;
	return *this;
}

Color& Color::operator-=(const Color& other)
{
	*this = *this - other;
	return *this;
}

Color& Color::operator*=(const Color& other)
{
	*this = *this * other;
	return *this;
}

Color& Color::operator/=(const Color& other)
{
	*this = *this / other;
	return *this;
}

Color& Color::operator*=(const int& other)
{
	*this = *this * other;
	return *this;
}

Color& Color::operator/=(const int& other)
{
	*this = *this / other;
	return *this;
}

Color& Color::operator*=(const float& other)
{
	*this = *this * other;
	return *this;
}

Color& Color::operator/=(const float& other)
{
	*this = *this / other;
	return *this;
}

Color& Color::operator*=(const double& other)
{
	*this = *this * other;
	return *this;
}

Color& Color::operator/=(const double& other)
{
	*this = *this / other;
	return *this;
}

bool Color::operator==(const Color& other) const
{
	if (R == other.R && G == other.G && B == other.B && A == other.A)
		return true;
	return false;
}

bool Color::operator!=(const Color& other) const
{
	if (R != other.R || G != other.G || B != other.B || A != other.A)
		return true;
	return false;
}

_float4color Color::getColor4() const
{
	return _float4color((float)R / 255.f, (float)G / 255.f, (float)B / 255.f, (float)A / 255.f);
}

Color operator*(const int& rhs, const Color& lhs)
{
	return lhs * rhs;
}

Color operator*(const float& rhs, const Color& lhs)
{
	return lhs * rhs;
}

Color operator*(const double& rhs, const Color& lhs)
{
	return lhs * rhs;
}
