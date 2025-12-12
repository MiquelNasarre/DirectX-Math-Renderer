#pragma once
#include "Vectors.h"

#define QUATERNION_ADDED

struct Quaternion
{
	float r, i, j, k;

	Quaternion(int r, int i = 0, int j = 0, int k = 0);
	Quaternion(float r = 0.f, float i = 0.f, float j = 0.f, float k = 0.f);
	Quaternion(double r, double i = 0., double j = 0., double k = 0.);
	Quaternion(Vector3f v);

	Quaternion operator+(const Quaternion& other) const;
	Quaternion operator-(const Quaternion& other) const;
	Quaternion operator*(const Quaternion& other) const;
	Quaternion operator/(const Quaternion& other) const;

	Quaternion operator-() const;
	Quaternion operator+(const int& other) const;
	Quaternion operator+(const float& other) const;
	Quaternion operator+(const double& other) const;
	Quaternion operator*(const int& other) const;
	Quaternion operator*(const float& other) const;
	Quaternion operator*(const double& other) const;
	Quaternion operator/(const int& other) const;
	Quaternion operator/(const float& other) const;
	Quaternion operator/(const double& other) const;
	Quaternion operator+=(const Quaternion& other);
	Quaternion operator*=(const Quaternion& other);

	Quaternion	normalize();
	Quaternion	inv() const;
	float		abs() const;
	const char* str(int decimals = 2u) const;
	Vector3f	getVector() const;
	_float4vector getVector4() const;

	constexpr operator bool() const { return i || j || k || r; }
};

Quaternion operator*(const int& lhs, const Quaternion& rhs);
Quaternion operator*(const float& lhs, const Quaternion& rhs);
Quaternion operator*(const double& lhs, const Quaternion& rhs);
Quaternion operator+(const int& lhs, const Quaternion& rhs);
Quaternion operator+(const float& lhs, const Quaternion& rhs);
Quaternion operator+(const double& lhs, const Quaternion& rhs);
Quaternion operator/(const int& lhs, const Quaternion& rhs);
Quaternion operator/(const float& lhs, const Quaternion& rhs);
Quaternion operator/(const double& lhs, const Quaternion& rhs);
Quaternion operator-(const int& lhs, const Quaternion& rhs);
Quaternion operator-(const float& lhs, const Quaternion& rhs);
Quaternion operator-(const double& lhs, const Quaternion& rhs);

constexpr bool operator!=(const Quaternion& q0, const Quaternion& q1) { return (q0.i != q1.i) || (q0.j != q1.j) || (q0.k != q1.k) || (q0.r != q1.r); }
constexpr bool operator==(const Quaternion& q0, const Quaternion& q1) { return !(q0 != q1); }

Quaternion rotationQuaternion(Vector3f axis, float angle);