#include <cmath>

#include "Vector.h"

		// Default constructor
Vector::Vector() {
		this->x=0.0f;
		this->y=0.0f;
		this->z=0.0f;	}
		// Initializing constructor
Vector::Vector(float x, float y, float z) {
        this->x=x;
        this->y=y;
        this->z=z;
    }
		// Dot product of 2 vectors
float dot(const Vector &a, const Vector &b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}
		// Cross product of 2 vectors
Vector cross(const Vector &a, const Vector &b)
{
	return Vector(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x);
}
	// Return length of vector
float length(const Vector &a)
{
	return std::sqrt(dot(a, a));
}
	// Get vector normal
Vector normalize(const Vector &a)
{
	const float lrcp = 1.0f / std::sqrt(dot(a, a));
	return Vector(a.x * lrcp, a.y * lrcp, a.z * lrcp);
}
	
Vector operator+(const Vector &a, const Vector &b) {
	Vector res;
	res.x = a.x+b.x;
	res.y = a.y+b.y;
	res.z = a.z+b.z;
	return res;
}

Vector operator-(const Vector &a, const Vector &b) {
	Vector res;
	res.x = b.x-a.x;
	res.y = b.y-a.y;
	res.z = b.z-a.z;
	return res;
}

Vector operator/(const Vector &a, const Vector &b) {
	Vector res;
	res.x = b.x/a.x;
	res.y = b.y/a.y;
	res.z = b.z/a.z;
	return res;
}

Vector operator*(const Vector &a, const Vector &b) {
	Vector res;
	res.x = b.x*a.x;
	res.y = b.y*a.y;
	res.z = b.z*a.z;
	return res;
}

Vector operator=(const Vector &a, Vector &b) {
	Vector res;
	res.x = a.x;
	res.y = a.y;
	res.z = a.z;
	return res;
}
