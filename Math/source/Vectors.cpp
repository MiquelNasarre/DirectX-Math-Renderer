#include "Vectors.h"

#include <string>
#include <math.h>

//	Vector2d

Vector2d::Vector2d()
{
	x = 0.;
	y = 0.;
}

Vector2d::Vector2d(double x, double y)
{
	this->x = x;
	this->y = y;
}

Vector2d::Vector2d(float x, float y)
{
	this->x = (double)x;
	this->y = (double)y;
}

Vector2d::Vector2d(int x, int y)
{
	this->x = (double)x;
	this->y = (double)y;
}

Vector2d::Vector2d(const Vector2i& other)
{
	x = (double)other.x;
	y = (double)other.y;
}

Vector2d::Vector2d(const Vector2f& other)
{
	x = (double)other.x;
	y = (double)other.y;
}

Vector2d Vector2d::operator+(const Vector2d& other) const
{
	return Vector2d(x + other.x, y + other.y);
}

Vector2d Vector2d::operator+(const Vector2f& other) const
{
	return *this + (Vector2d)other;
}

Vector2d Vector2d::operator+(const Vector2i& other) const
{
	return *this + (Vector2d)other;
}

Vector2d Vector2d::operator-(const Vector2d& other) const
{
	return Vector2d(x - other.x, y - other.y);
}

Vector2d Vector2d::operator-(const Vector2f& other) const
{
	return *this - (Vector2d)other;
}

Vector2d Vector2d::operator-(const Vector2i& other) const
{
	return *this - (Vector2d)other;
}

Vector2d Vector2d::operator-() const
{
	return Vector2d(-x, -y);
}

Vector2d Vector2d::operator*(const int& other) const
{
	return *this * (double)other;
}

Vector2d Vector2d::operator*(const float& other) const
{
	return *this * (double)other;
}

Vector2d Vector2d::operator*(const double& other) const
{
	return Vector2d(other * x, other * y);
}

Vector2d Vector2d::operator/(const int& other) const
{
	return *this / (double)other;
}

Vector2d Vector2d::operator/(const float& other) const
{
	return *this / (double)other;
}

Vector2d Vector2d::operator/(const double& other) const
{
	return Vector2d(x / other, y / other);
}

Vector2d& Vector2d::operator+=(const Vector2d& other)
{
	x += other.x;
	y += other.y;
	return *this;
}

Vector2d& Vector2d::operator+=(const Vector2f& other)
{
	x += (double)other.x;
	y += (double)other.y;
	return *this;
}

Vector2d& Vector2d::operator+=(const Vector2i& other)
{
	x += (double)other.x;
	y += (double)other.y;
	return *this;
}

Vector2d& Vector2d::operator-=(const Vector2d& other)
{
	x -= other.x;
	y -= other.y;
	return *this;
}

Vector2d& Vector2d::operator-=(const Vector2f& other)
{
	x -= (double)other.x;
	y -= (double)other.y;
	return *this;
}

Vector2d& Vector2d::operator-=(const Vector2i& other)
{
	x -= (double)other.x;
	y -= (double)other.y;
	return *this;
}

Vector2d& Vector2d::normalize()
{
	*this = *this / abs();
	return *this;
}

double Vector2d::operator^(const Vector2d& other) const
{
	return x * other.x + y * other.y;
}

Vector2d::operator bool() const
{
	return x || y;
}

bool Vector2d::operator!=(const Vector2d& other) const
{
	return x != other.x || y != other.y;
}

bool Vector2d::operator==(const Vector2d& other) const
{
	return x == other.x && y == other.y;
}

double Vector2d::abs() const
{
	return sqrt(x * x + y * y);
}

const char* Vector2d::str() const
{
	std::string String = "( " + std::to_string(x) + " , " + std::to_string(y) + " )";
	return (const char*)memcpy(calloc(String.size() + 1, sizeof(char)), String.c_str(), String.size() + 1);
}

//	Vector2f

Vector2f::Vector2f()
{
	x = 0.f;
	y = 0.f;
}

Vector2f::Vector2f(double x, double y)
{
	this->x = (float)x;
	this->y = (float)y;
}

Vector2f::Vector2f(float x, float y)
{
	this->x = x;
	this->y = y;
}

Vector2f::Vector2f(int x, int y)
{
	this->x = (float)x;
	this->y = (float)y;
}

Vector2f::Vector2f(const Vector2i& other)
{
	x = (float)other.x;
	y = (float)other.y;
}

Vector2f::Vector2f(const Vector2d& other)
{
	x = (float)other.x;
	y = (float)other.y;
}

Vector2d Vector2f::operator+(const Vector2d& other) const
{
	return (Vector2d)*this + other;
}

Vector2f Vector2f::operator+(const Vector2f& other) const
{
	return Vector2f(x + other.x, y + other.y);
}

Vector2f Vector2f::operator+(const Vector2i& other) const
{
	return *this + (Vector2f)other;
}

Vector2d Vector2f::operator-(const Vector2d& other) const
{
	return (Vector2d)*this - other;
}

Vector2f Vector2f::operator-(const Vector2f& other) const
{
	return Vector2f(x - other.x, y - other.y);
}

Vector2f Vector2f::operator-(const Vector2i& other) const
{
	return *this - (Vector2f)other;
}

Vector2f Vector2f::operator-() const
{
	return Vector2f(-x, -y);
}

Vector2f Vector2f::operator*(const int& other) const
{
	return *this * (float)other;
}

Vector2f Vector2f::operator*(const float& other) const
{
	return Vector2f(x * other, y * other);
}

Vector2d Vector2f::operator*(const double& other) const
{
	return *this * (float)other;
}

Vector2f Vector2f::operator/(const int& other) const
{
	return *this / (float)other;
}

Vector2f Vector2f::operator/(const float& other) const
{
	return Vector2f(x / other, y / other);
}

Vector2d Vector2f::operator/(const double& other) const
{
	return *this / (float)other;
}

Vector2f& Vector2f::operator+=(const Vector2f& other)
{
	x += other.x;
	y += other.y;
	return *this;
}

Vector2f& Vector2f::operator+=(const Vector2i& other)
{
	x += (float)other.x;
	y += (float)other.y;
	return *this;
}

Vector2f& Vector2f::operator-=(const Vector2f& other)
{
	x -= other.x;
	y -= other.y;
	return *this;
}

Vector2f& Vector2f::operator-=(const Vector2i& other)
{
	x -= (float)other.x;
	y -= (float)other.y;
	return *this;
}

Vector2f& Vector2f::operator*=(const float& other)
{
	x *= other;
	y *= other;
	return *this;
}

Vector2f& Vector2f::operator/=(const float& other)
{
	x /= other;
	y /= other;
	return *this;
}

Vector2f& Vector2f::normalize()
{
	*this = *this / abs();
	return *this;
}

float Vector2f::operator^(const Vector2f& other) const
{
	return x * other.x + y * other.y;
}

Vector2f::operator bool() const
{
	return x || y;
}

bool Vector2f::operator!=(const Vector2f& other) const
{
	return x != other.x || y != other.y;
}

bool Vector2f::operator==(const Vector2f& other) const
{
	return x == other.x && y == other.y;
}

float Vector2f::abs() const
{
	return sqrtf(x * x + y * y);
}

const char* Vector2f::str() const
{
	std::string String = "( " + std::to_string(x) + " , " + std::to_string(y) + " )";
	return (const char*)memcpy(calloc(String.size() + 1, sizeof(char)), String.c_str(), String.size() + 1);
}

_float4vector Vector2f::getVector4() const
{
	return _float4vector(x, y, 0.f, 0.f);
}

//	Vector2i

Vector2i::Vector2i()
{
	x = 0;
	y = 0;
}

Vector2i::Vector2i(double x, double y)
{
	this->x = (int)x;
	this->y = (int)y;
}

Vector2i::Vector2i(float x, float y)
{
	this->x = (int)x;
	this->y = (int)y;
}

Vector2i::Vector2i(int x, int y)
{
	this->x = x;
	this->y = y;
}

Vector2i::Vector2i(const Vector2f& other)
{
	x = (int)other.x;
	y = (int)other.y;
}

Vector2i::Vector2i(const Vector2d& other)
{
	x = (int)other.x;
	y = (int)other.y;
}

Vector2d Vector2i::operator+(const Vector2d& other) const
{
	return (Vector2d)*this + other;
}

Vector2f Vector2i::operator+(const Vector2f& other) const
{
	return (Vector2f)*this + other;
}

Vector2i Vector2i::operator+(const Vector2i& other) const
{
	return Vector2i(x + other.x, y + other.y);
}

Vector2d Vector2i::operator-(const Vector2d& other) const
{
	return (Vector2d)*this - other;
}

Vector2f Vector2i::operator-(const Vector2f& other) const
{
	return (Vector2f)*this - other;
}

Vector2i Vector2i::operator-(const Vector2i& other) const
{
	return Vector2i(x - other.x, y - other.y);
}

Vector2i Vector2i::operator-() const
{
	return Vector2i(-x, -y);
}

Vector2i Vector2i::operator*(const int& other) const
{
	return Vector2i(x * other, y * other);
}

Vector2f Vector2i::operator*(const float& other) const
{
	return (Vector2f)*this * other;
}

Vector2d Vector2i::operator*(const double& other) const
{
	return (Vector2d)*this * other;
}

Vector2i Vector2i::operator/(const int& other) const
{
	return Vector2i(x / other, y / other);
}

Vector2f Vector2i::operator/(const float& other) const
{
	return (Vector2f)*this / other;
}

Vector2d Vector2i::operator/(const double& other) const
{
	return (Vector2d)*this / other;
}

Vector2i& Vector2i::operator+=(const Vector2i& other)
{
	x += other.x;
	y += other.y;
	return *this;
}

Vector2i& Vector2i::operator-=(const Vector2i& other)
{
	x -= other.x;
	y -= other.y;
	return *this;
}

int Vector2i::operator^(const Vector2i& other) const
{
	return x * other.x + y * other.y;
}

Vector2i::operator bool() const
{
	return x || y;
}

bool Vector2i::operator!=(const Vector2i& other) const
{
	return x != other.x || y != other.y;
}

bool Vector2i::operator==(const Vector2i& other) const
{
	return x == other.x && y == other.y;
}

float Vector2i::abs() const
{
	return sqrtf(float(x * x + y * y));
}

const char* Vector2i::str() const
{
	std::string String = "( " + std::to_string(x) + " , " + std::to_string(y) + " )";
	return (const char*)memcpy(calloc(String.size() + 1, sizeof(char)), String.c_str(), String.size() + 1);
}

//	Rest of operators

Vector2i operator*(const int& lhs, const Vector2i& rhs)
{
	return Vector2i(lhs * rhs.x, lhs * rhs.y);
}

Vector2f operator*(const float& lhs, const Vector2i& rhs)
{
	return Vector2f(lhs * (float)rhs.x, lhs * (float)rhs.y);
}

Vector2d operator*(const double& lhs, const Vector2i& rhs)
{
	return Vector2d(lhs * (double)rhs.x, lhs * (double)rhs.y);
}

Vector2f operator*(const int& lhs, const Vector2f& rhs)
{
	return Vector2f((float)lhs * rhs.x, (float)lhs * rhs.y);
}

Vector2f operator*(const float& lhs, const Vector2f& rhs)
{
	return Vector2f(lhs * rhs.x, lhs * rhs.y);
}

Vector2d operator*(const double& lhs, const Vector2f& rhs)
{
	return Vector2d(lhs * (double)rhs.x, lhs * (double)rhs.y);
}

Vector2d operator*(const int& lhs, const Vector2d& rhs)
{
	return Vector2d((double)lhs * rhs.x, (double)lhs * rhs.y);
}

Vector2d operator*(const float& lhs, const Vector2d& rhs)
{
	return Vector2d((double)lhs * rhs.x, (double)lhs * rhs.y);
}

Vector2d operator*(const double& lhs, const Vector2d& rhs)
{
	return Vector2d(lhs * rhs.x, lhs * rhs.y);
}

//	Vector3d

Vector3d::Vector3d()
{
	x = 0.;
	y = 0.;
	z = 0.;
}

Vector3d::Vector3d(double x, double y, double z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3d::Vector3d(float x, float y, float z)
{
	this->x = (double)x;
	this->y = (double)y;
	this->z = (double)z;
}

Vector3d::Vector3d(int x, int y, int z)
{
	this->x = (double)x;
	this->y = (double)y;
	this->z = (double)z;
}

Vector3d::Vector3d(const Vector3i& other)
{
	x = (double)other.x;
	y = (double)other.y;
	z = (double)other.z;
}

Vector3d::Vector3d(const Vector3f& other)
{
	x = (double)other.x;
	y = (double)other.y;
	z = (double)other.z;
}

Vector3d Vector3d::operator+(const Vector3d& other) const
{
	return Vector3d(x + other.x, y + other.y, z + other.z);
}

Vector3d Vector3d::operator+(const Vector3f& other) const
{
	return *this + (Vector3d)other;
}

Vector3d Vector3d::operator+(const Vector3i& other) const
{
	return *this + (Vector3d)other;
}

Vector3d Vector3d::operator-(const Vector3d& other) const
{
	return Vector3d(x - other.x, y - other.y, z - other.z);
}

Vector3d Vector3d::operator-(const Vector3f& other) const
{
	return *this - (Vector3d)other;
}

Vector3d Vector3d::operator-(const Vector3i& other) const
{
	return *this - (Vector3d)other;
}

Vector3d Vector3d::operator-() const
{
	return Vector3d(-x, -y, -z);
}

Vector3d Vector3d::operator*(const int& other) const
{
	return *this * (double)other;
}

Vector3d Vector3d::operator*(const float& other) const
{
	return *this * (double)other;
}

Vector3d Vector3d::operator*(const double& other) const
{
	return Vector3d(other * x, other * y, other * z);
}

Vector3d Vector3d::operator/(const int& other) const
{
	return *this / (double)other;
}

Vector3d Vector3d::operator/(const float& other) const
{
	return *this / (double)other;
}

Vector3d Vector3d::operator/(const double& other) const
{
	return Vector3d(x / other, y / other, z / other);
}

Vector3d& Vector3d::operator+=(const Vector3d& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}

Vector3d& Vector3d::operator+=(const Vector3f& other)
{
	x += (double)other.x;
	y += (double)other.y;
	z += (double)other.z;
	return *this;
}

Vector3d& Vector3d::operator+=(const Vector3i& other)
{
	x += (double)other.x;
	y += (double)other.y;
	z += (double)other.z;
	return *this;
}

Vector3d& Vector3d::operator-=(const Vector3d& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}

Vector3d& Vector3d::operator-=(const Vector3f& other)
{
	x -= (double)other.x;
	y -= (double)other.y;
	z -= (double)other.z;
	return *this;
}

Vector3d& Vector3d::operator-=(const Vector3i& other)
{
	x -= (double)other.x;
	y -= (double)other.y;
	z -= (double)other.z;
	return *this;
}

Vector3d Vector3d::operator*(const Vector3d& other) const
{
	return -Vector3d(y * other.z - other.y * z, z * other.x - other.z * x, x * other.y - other.x * y);
}

Vector3d& Vector3d::normalize()
{
	*this = *this / abs();
	return *this;
}

double Vector3d::operator^(const Vector3d& other) const
{
	return x * other.x + y * other.y + z * other.z;
}

Vector3d::operator bool() const
{
	return x || y || z;
}

bool Vector3d::operator!=(const Vector3d& other) const
{
	return x != other.x || y != other.y || z != other.z;
}

bool Vector3d::operator==(const Vector3d& other) const
{
	return x == other.x && y == other.y && z == other.z;
}

double Vector3d::abs() const
{
	return sqrt(x * x + y * y + z * z);
}

const char* Vector3d::str() const
{
	std::string String = "( " + std::to_string(x) + " , " + std::to_string(y) + " , " + std::to_string(z) + " )";
	return (const char*)memcpy(calloc(String.size() + 1, sizeof(char)), String.c_str(), String.size() + 1);
}

//	Vector3f

Vector3f::Vector3f()
{
	x = 0.f;
	y = 0.f;
	z = 0.f;
}

Vector3f::Vector3f(double x, double y, double z)
{
	this->x = (float)x;
	this->y = (float)y;
	this->z = (float)z;
}

Vector3f::Vector3f(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3f::Vector3f(int x, int y, int z)
{
	this->x = (float)x;
	this->y = (float)y;
	this->z = (float)z;
}

Vector3f::Vector3f(const Vector3i& other)
{
	x = (float)other.x;
	y = (float)other.y;
	z = (float)other.z;
}

Vector3f::Vector3f(const Vector3d& other)
{
	x = (float)other.x;
	y = (float)other.y;
	z = (float)other.z;
}

Vector3d Vector3f::operator+(const Vector3d& other) const
{
	return (Vector3d)*this + other;
}

Vector3f Vector3f::operator+(const Vector3f& other) const
{
	return Vector3f(x + other.x, y + other.y, z + other.z);
}

Vector3f Vector3f::operator+(const Vector3i& other) const
{
	return *this + (Vector3f)other;
}

Vector3d Vector3f::operator-(const Vector3d& other) const
{
	return (Vector3d)*this - other;
}

Vector3f Vector3f::operator-(const Vector3f& other) const
{
	return Vector3f(x - other.x, y - other.y, z - other.z);
}

Vector3f Vector3f::operator-(const Vector3i& other) const
{
	return *this - (Vector3f)other;
}

Vector3f Vector3f::operator-() const
{
	return Vector3f(-x, -y, -z);
}

Vector3f Vector3f::operator*(const int& other) const
{
	return *this * (float)other;
}

Vector3f Vector3f::operator*(const float& other) const
{
	return Vector3f(x * other, y * other, z * other);
}

Vector3d Vector3f::operator*(const double& other) const
{
	return *this * (float)other;
}

Vector3f Vector3f::operator/(const int& other) const
{
	return *this / (float)other;
}

Vector3f Vector3f::operator/(const float& other) const
{
	return Vector3f(x / other, y / other, z / other);
}

Vector3d Vector3f::operator/(const double& other) const
{
	return *this / (float)other;
}

Vector3f& Vector3f::operator+=(const Vector3f& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}

Vector3f& Vector3f::operator+=(const Vector3i& other)
{
	x += (float)other.x;
	y += (float)other.y;
	z += (float)other.z;
	return *this;
}

Vector3f& Vector3f::operator-=(const Vector3f& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}

Vector3f& Vector3f::operator-=(const Vector3i& other)
{
	x -= (float)other.x;
	y -= (float)other.y;
	z -= (float)other.z;
	return *this;
}

Vector3f& Vector3f::operator*=(const float& other)
{
	x *= other;
	y *= other;
	z *= other;
	return *this;
}

Vector3f& Vector3f::operator/=(const float& other)
{
	x /= other;
	y /= other;
	z /= other;
	return *this;
}

Vector3f Vector3f::operator*(const Vector3f& other) const
{
	return Vector3f(z * other.y - other.z * y, x * other.z - other.x * z, y * other.x - other.y * x);
}

Vector3f& Vector3f::normalize()
{
	if (!*this) return *this;
	return *this /= sqrtf(x * x + y * y + z * z);
}

float Vector3f::operator^(const Vector3f& other) const
{
	return x * other.x + y * other.y + z * other.z;
}

Vector3f::operator bool() const
{
	return x || y || z;
}

bool Vector3f::operator!=(const Vector3f& other) const
{
	return x != other.x || y != other.y || z != other.z;
}

bool Vector3f::operator==(const Vector3f& other) const
{
	return x == other.x && y == other.y && z == other.z;
}

float Vector3f::abs() const
{
	return sqrtf(x * x + y * y + z * z);
}

const char* Vector3f::str() const
{
	std::string String = "( " + std::to_string(x) + " , " + std::to_string(y) + " , " + std::to_string(z) + " )";
	return (const char*)memcpy(calloc(String.size() + 1, sizeof(char)), String.c_str(), String.size() + 1);
}

_float4vector Vector3f::getVector4() const
{
	return _float4vector(x,y,z,0.f);
}

//	Vector3i

Vector3i::Vector3i()
{
	x = 0;
	y = 0;
	z = 0;
}

Vector3i::Vector3i(double x, double y, double z)
{
	this->x = (int)x;
	this->y = (int)y;
	this->z = (int)z;
}

Vector3i::Vector3i(float x, float y, float z)
{
	this->x = (int)x;
	this->y = (int)y;
	this->z = (int)z;
}

Vector3i::Vector3i(int x, int y, int z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3i::Vector3i(const Vector3f& other)
{
	x = (int)other.x;
	y = (int)other.y;
	z = (int)other.z;
}

Vector3i::Vector3i(const Vector3d& other)
{
	x = (int)other.x;
	y = (int)other.y;
	z = (int)other.z;
}

Vector3d Vector3i::operator+(const Vector3d& other) const
{
	return (Vector3d)*this + other;
}

Vector3f Vector3i::operator+(const Vector3f& other) const
{
	return (Vector3f)*this + other;
}

Vector3i Vector3i::operator+(const Vector3i& other) const
{
	return Vector3i(x + other.x, y + other.y, z + other.z);
}

Vector3d Vector3i::operator-(const Vector3d& other) const
{
	return (Vector3d)*this - other;
}

Vector3f Vector3i::operator-(const Vector3f& other) const
{
	return (Vector3f)*this - other;
}

Vector3i Vector3i::operator-(const Vector3i& other) const
{
	return Vector3i(x - other.x, y - other.y, z - other.z);
}

Vector3i Vector3i::operator-() const
{
	return Vector3i(-x, -y, -z);
}

Vector3i Vector3i::operator*(const int& other) const
{
	return Vector3i(x * other, y * other, z * other);
}

Vector3f Vector3i::operator*(const float& other) const
{
	return (Vector3f)*this * other;
}

Vector3d Vector3i::operator*(const double& other) const
{
	return (Vector3d)*this * other;
}

Vector3i Vector3i::operator/(const int& other) const
{
	return Vector3i(x / other, y / other, z / other);
}

Vector3f Vector3i::operator/(const float& other) const
{
	return (Vector3f)*this / other;
}

Vector3d Vector3i::operator/(const double& other) const
{
	return (Vector3d)*this / other;
}

Vector3i& Vector3i::operator+=(const Vector3i& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}

Vector3i& Vector3i::operator-=(const Vector3i& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}

Vector3i Vector3i::operator*(const Vector3i& other) const
{
	return -Vector3d(y * other.z - other.y * z, z * other.x - other.z * x, x * other.y - other.x * y);
}

int Vector3i::operator^(const Vector3i& other) const
{
	return x * other.x + y * other.y + z * other.z;
}

Vector3i::operator bool() const
{
	return x || y || z;
}

bool Vector3i::operator!=(const Vector3i& other) const
{
	return x != other.x || y != other.y || z != other.z;
}

bool Vector3i::operator==(const Vector3i& other) const
{
	return x == other.x && y == other.y && z == other.z;
}

float Vector3i::abs() const
{
	return sqrtf(float(x * x + y * y + z * z));
}

const char* Vector3i::str() const
{
	std::string String = "( " + std::to_string(x) + " , " + std::to_string(y) + " , " + std::to_string(z) + " )";
	return (const char*)memcpy(calloc(String.size() + 1, sizeof(char)), String.c_str(), String.size() + 1);
}

//	Rest of operators

Vector3i operator*(const int& lhs, const Vector3i& rhs)
{
	return Vector3i(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
}

Vector3f operator*(const float& lhs, const Vector3i& rhs)
{
	return Vector3f(lhs * (float)rhs.x, lhs * (float)rhs.y, lhs * (float)rhs.z);
}

Vector3d operator*(const double& lhs, const Vector3i& rhs)
{
	return Vector3d(lhs * (double)rhs.x, lhs * (double)rhs.y, lhs * (double)rhs.z);
}

Vector3f operator*(const int& lhs, const Vector3f& rhs)
{
	return Vector3f((float)lhs * rhs.x, (float)lhs * rhs.y, (float)lhs * rhs.z);
}

Vector3f operator*(const float& lhs, const Vector3f& rhs)
{
	return Vector3f(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
}

Vector3d operator*(const double& lhs, const Vector3f& rhs)
{
	return Vector3d(lhs * (double)rhs.x, lhs * (double)rhs.y, lhs * (double)rhs.z);
}

Vector3d operator*(const int& lhs, const Vector3d& rhs)
{
	return Vector3d((double)lhs * rhs.x, (double)lhs * rhs.y, (double)lhs * rhs.z);
}

Vector3d operator*(const float& lhs, const Vector3d& rhs)
{
	return Vector3d((double)lhs * rhs.x, (double)lhs * rhs.y, (double)lhs * rhs.z);
}

Vector3d operator*(const double& lhs, const Vector3d& rhs)
{
	return Vector3d(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
}
