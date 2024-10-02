#include "Quaternion.h"
#include <string>

Quaternion::Quaternion(int r, int i, int j, int k)
{
	this->r = (float)r;
	this->i = (float)i;
	this->j = (float)j;
	this->k = (float)k;
}

Quaternion::Quaternion(float r, float i, float j, float k)
{
	this->r = r;
	this->i = i;
	this->j = j;
	this->k = k;
}

Quaternion::Quaternion(double r, double i, double j, double k)
{
	this->r = (float)r;
	this->i = (float)i;
	this->j = (float)j;
	this->k = (float)k;
}

Quaternion::Quaternion(Vector3f v)
{
	r = 0.f;
	i = v.x;
	j = v.y;
	k = v.z;
}

Quaternion Quaternion::operator+(const Quaternion& other) const
{
	return Quaternion(r + other.r, i + other.i, j + other.j, k + other.k);
}

Quaternion Quaternion::operator-(const Quaternion& other) const
{
	return Quaternion(r - other.r, i - other.i, j - other.j, k - other.k);
}

Quaternion Quaternion::operator*(const Quaternion& other) const
{
	return Quaternion(
		r * other.r - i * other.i - j * other.j - k * other.k,
		r * other.i + i * other.r + j * other.k - k * other.j,
		r * other.j + j * other.r + k * other.i - i * other.k,
		r * other.k + k * other.r + i * other.j - j * other.i
	);
}

Quaternion Quaternion::operator/(const Quaternion& other) const
{
	return *this * other.inv();
}

Quaternion Quaternion::operator-() const
{
	return Quaternion(-r, -i, -j, -k);
}

Quaternion Quaternion::operator+(const int& other) const
{
	return Quaternion(r + float(other), i, j, k);
}

Quaternion Quaternion::operator+(const float& other) const
{
	return Quaternion(r + other, i, j, k);
}

Quaternion Quaternion::operator+(const double& other) const
{
	return Quaternion(r + float(other), i, j, k);
}

Quaternion Quaternion::operator*(const int& other) const
{
	return Quaternion(float(other) * r, float(other) * i, float(other) * j, float(other) * k);
}

Quaternion Quaternion::operator*(const float& other) const
{
	return Quaternion(other * r, other * i, other * j, other * k);
}

Quaternion Quaternion::operator*(const double& other) const
{
	return Quaternion(float(other) * r, float(other) * i, float(other) * j, float(other) * k);
}

Quaternion Quaternion::operator/(const int& other) const
{
	return Quaternion(r / float(other), i / float(other), j / float(other), k / float(other));
}

Quaternion Quaternion::operator/(const float& other) const
{
	return Quaternion(r / other, i / other, j / other, k / other);
}

Quaternion Quaternion::operator/(const double& other) const
{
	return Quaternion(r / float(other), i / float(other), j / float(other), k / float(other));
}

Quaternion Quaternion::operator+=(const Quaternion& other)
{
	*this = *this + other;
	return *this;
}

Quaternion Quaternion::operator*=(const Quaternion& other)
{
	*this = other * *this;
	return *this;
}

Quaternion Quaternion::normalize()
{
	return *this *= 1.f / abs();
}

Quaternion Quaternion::inv() const
{
	return Quaternion(r, -i, -j, -k) / (r * r + i * i + j * j + k * k);
}

float Quaternion::abs() const
{
	return sqrtf(r * r + i * i + j * j + k * k);
}

const char* Quaternion::str(int decimals) const
{
	if (!decimals)decimals = -1;

	std::string String;
	String = std::to_string(r).erase(2 + decimals, 6 - decimals);
	if (i >= 0)
		String = String + " + " + std::to_string(i).erase(2 + decimals,6 - decimals) + " i";
	else if (i < 0)
		String = String + " - " + std::to_string(-i).erase(2 + decimals, 6 - decimals) + " i";

	if (j >= 0)
		String = String + " + " + std::to_string(j).erase(2 + decimals, 6 - decimals) + " j";
	else if (j < 0)
		String = String + " - " + std::to_string(-j).erase(2 + decimals, 6 - decimals) + " j";

	if (k >= 0)
		String = String + " + " + std::to_string(k).erase(2 + decimals, 6 - decimals) + " k";
	else if (k < 0)
		String = String + " - " + std::to_string(-k).erase(2 + decimals, 6 - decimals) + " k";

	return (const char*)memcpy(calloc(String.size() + 1, sizeof(char)), String.c_str(), String.size() + 1);
}

Vector3f Quaternion::getVector() const
{
	return Vector3f(i, j, k);
}

_float4vector Quaternion::getVector4() const
{
	return { i, j, k, 0.f };
}

Quaternion operator*(const int& lhs, const Quaternion& rhs)
{
	return rhs * lhs;
}

Quaternion operator*(const float& lhs, const Quaternion& rhs)
{
	return rhs * lhs;
}

Quaternion operator*(const double& lhs, const Quaternion& rhs)
{
	return rhs * lhs;
}

Quaternion operator+(const int& lhs, const Quaternion& rhs)
{
	return rhs + lhs;
}

Quaternion operator+(const float& lhs, const Quaternion& rhs)
{
	return rhs + lhs;
}

Quaternion operator+(const double& lhs, const Quaternion& rhs)
{
	return rhs + lhs;
}

Quaternion operator/(const int& lhs, const Quaternion& rhs)
{
	return rhs.inv() * lhs;
}

Quaternion operator/(const float& lhs, const Quaternion& rhs)
{
	return rhs.inv() * lhs;
}

Quaternion operator/(const double& lhs, const Quaternion& rhs)
{
	return rhs.inv() * lhs;
}

Quaternion operator-(const int& lhs, const Quaternion& rhs)
{
	return -rhs + lhs;
}

Quaternion operator-(const float& lhs, const Quaternion& rhs)
{
	return -rhs + lhs;
}

Quaternion operator-(const double& lhs, const Quaternion& rhs)
{
	return -rhs + lhs;
}

Quaternion rotationQuaternion(Vector3f axis, float angle)
{
	if (!axis) return 1.f;

	axis.normalize();
	float s = sinf(angle / 2.f);
	return Quaternion(cosf(angle / 2.f), s * axis.x, s * axis.y, s * axis.z);
}

