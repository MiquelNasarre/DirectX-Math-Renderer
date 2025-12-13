#pragma once

struct Vector2i;
struct Vector2f;
struct Vector2d;

struct _float4vector {
	float x;
	float y;
	float z;
	float w;
};

struct Vector2d {
	double x;
	double y;

	Vector2d();
	Vector2d(double x, double y);
	Vector2d(float x, float y);
	Vector2d(int x, int y);
	Vector2d(const Vector2i& other);
	Vector2d(const Vector2f& other);

	Vector2d operator+(const Vector2d& other) const;
	Vector2d operator+(const Vector2f& other) const;
	Vector2d operator+(const Vector2i& other) const;
	Vector2d operator-(const Vector2d& other) const;
	Vector2d operator-(const Vector2f& other) const;
	Vector2d operator-(const Vector2i& other) const;
	Vector2d operator-() const;
	Vector2d operator*(const int& other) const;
	Vector2d operator*(const float& other) const;
	Vector2d operator*(const double& other) const;
	Vector2d operator/(const int& other) const;
	Vector2d operator/(const float& other) const;
	Vector2d operator/(const double& other) const;
	Vector2d& operator+=(const Vector2d& other);
	Vector2d& operator+=(const Vector2f& other);
	Vector2d& operator+=(const Vector2i& other);
	Vector2d& operator-=(const Vector2d& other);
	Vector2d& operator-=(const Vector2f& other);
	Vector2d& operator-=(const Vector2i& other);

	Vector2d& normalize();
	double operator^(const Vector2d& other) const;
	operator bool() const;
	bool operator!=(const Vector2d& other) const;
	bool operator==(const Vector2d& other) const;

	double		abs() const;
	const char* str() const;
};

struct Vector2f {
	float x;
	float y;

	Vector2f();
	Vector2f(double x, double y);
	Vector2f(float x, float y);
	Vector2f(int x, int y);
	Vector2f(const Vector2i& other);
	Vector2f(const Vector2d& other);

	Vector2d operator+(const Vector2d& other) const;
	Vector2f operator+(const Vector2f& other) const;
	Vector2f operator+(const Vector2i& other) const;
	Vector2d operator-(const Vector2d& other) const;
	Vector2f operator-(const Vector2f& other) const;
	Vector2f operator-(const Vector2i& other) const;
	Vector2f operator-() const;
	Vector2f operator*(const int& other) const;
	Vector2f operator*(const float& other) const;
	Vector2d operator*(const double& other) const;
	Vector2f operator/(const int& other) const;
	Vector2f operator/(const float& other) const;
	Vector2d operator/(const double& other) const;
	Vector2f& operator+=(const Vector2f& other);
	Vector2f& operator+=(const Vector2i& other);
	Vector2f& operator-=(const Vector2f& other);
	Vector2f& operator-=(const Vector2i& other);
	Vector2f& operator*=(const float& other);
	Vector2f& operator/=(const float& other);

	Vector2f& normalize();
	float operator^(const Vector2f& other) const;
	operator bool() const;
	bool operator!=(const Vector2f& other) const;
	bool operator==(const Vector2f& other) const;

	float		abs() const;
	const char* str() const;
	_float4vector getVector4() const;
};

struct Vector2i {
	int x;
	int y;

	Vector2i();
	Vector2i(double x, double y);
	Vector2i(float x, float y);
	Vector2i(int x, int y);
	Vector2i(unsigned x, unsigned y) { this->x = x; this->y = y; }
	Vector2i(const Vector2f& other);
	Vector2i(const Vector2d& other);

	Vector2d operator+(const Vector2d& other) const;
	Vector2f operator+(const Vector2f& other) const;
	Vector2i operator+(const Vector2i& other) const;
	Vector2d operator-(const Vector2d& other) const;
	Vector2f operator-(const Vector2f& other) const;
	Vector2i operator-(const Vector2i& other) const;
	Vector2i operator-() const;
	Vector2i operator*(const int& other) const;
	Vector2f operator*(const float& other) const;
	Vector2d operator*(const double& other) const;
	Vector2i operator/(const int& other) const;
	Vector2f operator/(const float& other) const;
	Vector2d operator/(const double& other) const;
	Vector2i& operator+=(const Vector2i& other);
	Vector2i& operator-=(const Vector2i& other);

	int operator^(const Vector2i& other) const;
	operator bool() const;
	bool operator!=(const Vector2i& other) const;
	bool operator==(const Vector2i& other) const;

	float		abs() const;
	const char* str() const;
};

Vector2i operator*(const int& lhs, const Vector2i& rhs);
Vector2f operator*(const float& lhs, const Vector2i& rhs);
Vector2d operator*(const double& lhs, const Vector2i& rhs);

Vector2f operator*(const int& lhs, const Vector2f& rhs);
Vector2f operator*(const float& lhs, const Vector2f& rhs);
Vector2d operator*(const double& lhs, const Vector2f& rhs);

Vector2d operator*(const int& lhs, const Vector2d& rhs);
Vector2d operator*(const float& lhs, const Vector2d& rhs);
Vector2d operator*(const double& lhs, const Vector2d& rhs);

struct Vector3i;
struct Vector3f;
struct Vector3d;

struct Vector3d {
	double x;
	double y;
	double z;

	Vector3d();
	Vector3d(double x, double y, double z);
	Vector3d(float x, float y, float z);
	Vector3d(int x, int y, int z);
	Vector3d(const Vector3i& other);
	Vector3d(const Vector3f& other);

	Vector3d operator+(const Vector3d& other) const;
	Vector3d operator+(const Vector3f& other) const;
	Vector3d operator+(const Vector3i& other) const;
	Vector3d operator-(const Vector3d& other) const;
	Vector3d operator-(const Vector3f& other) const;
	Vector3d operator-(const Vector3i& other) const;
	Vector3d operator-() const;
	Vector3d operator*(const int& other) const;
	Vector3d operator*(const float& other) const;
	Vector3d operator*(const double& other) const;
	Vector3d operator/(const int& other) const;
	Vector3d operator/(const float& other) const;
	Vector3d operator/(const double& other) const;
	Vector3d& operator+=(const Vector3d& other);
	Vector3d& operator+=(const Vector3f& other);
	Vector3d& operator+=(const Vector3i& other);
	Vector3d& operator-=(const Vector3d& other);
	Vector3d& operator-=(const Vector3f& other);
	Vector3d& operator-=(const Vector3i& other);

	Vector3d operator*(const Vector3d& other) const;
	Vector3d& normalize();
	double operator^(const Vector3d& other) const;
	operator bool() const;
	bool operator!=(const Vector3d& other) const;
	bool operator==(const Vector3d& other) const;

	double		abs() const;
	const char* str() const;
};

struct Vector3f {
	float x;
	float y;
	float z;

	Vector3f();
	Vector3f(double x, double y, double z);
	Vector3f(float x, float y, float z);
	Vector3f(int x, int y, int z);
	Vector3f(const Vector3i& other);
	Vector3f(const Vector3d& other);

	Vector3d operator+(const Vector3d& other) const;
	Vector3f operator+(const Vector3f& other) const;
	Vector3f operator+(const Vector3i& other) const;
	Vector3d operator-(const Vector3d& other) const;
	Vector3f operator-(const Vector3f& other) const;
	Vector3f operator-(const Vector3i& other) const;
	Vector3f operator-() const;
	Vector3f operator*(const int& other) const;
	Vector3f operator*(const float& other) const;
	Vector3d operator*(const double& other) const;
	Vector3f operator/(const int& other) const;
	Vector3f operator/(const float& other) const;
	Vector3d operator/(const double& other) const;
	Vector3f& operator+=(const Vector3f& other);
	Vector3f& operator+=(const Vector3i& other);
	Vector3f& operator-=(const Vector3f& other);
	Vector3f& operator-=(const Vector3i& other);
	Vector3f& operator*=(const float& other);
	Vector3f& operator/=(const float& other);

	Vector3f operator*(const Vector3f& other) const;
	Vector3f& normalize();
	float operator^(const Vector3f& other) const;
	operator bool() const;
	bool operator!=(const Vector3f& other) const;
	bool operator==(const Vector3f& other) const;

	float		abs() const;
	const char* str() const;
	_float4vector getVector4() const;
};

struct Vector3i {
	int x;
	int y;
	int z;

	Vector3i();
	Vector3i(double x, double y, double z);
	Vector3i(float x, float y, float z);
	Vector3i(int x, int y, int z);
	Vector3i(unsigned x, unsigned y, unsigned z) { this->x = x; this->y = y; this->z = z; }
	Vector3i(const Vector3f& other);
	Vector3i(const Vector3d& other);

	Vector3d operator+(const Vector3d& other) const;
	Vector3f operator+(const Vector3f& other) const;
	Vector3i operator+(const Vector3i& other) const;
	Vector3d operator-(const Vector3d& other) const;
	Vector3f operator-(const Vector3f& other) const;
	Vector3i operator-(const Vector3i& other) const;
	Vector3i operator-() const;
	Vector3i operator*(const int& other) const;
	Vector3f operator*(const float& other) const;
	Vector3d operator*(const double& other) const;
	Vector3i operator/(const int& other) const;
	Vector3f operator/(const float& other) const;
	Vector3d operator/(const double& other) const;
	Vector3i& operator+=(const Vector3i& other);
	Vector3i& operator-=(const Vector3i& other);

	Vector3i operator*(const Vector3i& other) const;
	int operator^(const Vector3i& other) const;
	operator bool() const;
	bool operator!=(const Vector3i& other) const;
	bool operator==(const Vector3i& other) const;

	float		abs() const;
	const char* str() const;
};

Vector3i operator*(const int& lhs, const Vector3i& rhs);
Vector3f operator*(const float& lhs, const Vector3i& rhs);
Vector3d operator*(const double& lhs, const Vector3i& rhs);

Vector3f operator*(const int& lhs, const Vector3f& rhs);
Vector3f operator*(const float& lhs, const Vector3f& rhs);
Vector3d operator*(const double& lhs, const Vector3f& rhs);

Vector3d operator*(const int& lhs, const Vector3d& rhs);
Vector3d operator*(const float& lhs, const Vector3d& rhs);
Vector3d operator*(const double& lhs, const Vector3d& rhs);