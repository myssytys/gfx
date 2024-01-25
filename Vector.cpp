#include <cmath>

class Vector {

public:
    float x,y,z;

		// Default constructor
	Vector() {
		this->x=0.0f;
		this->y=0.0f;
		this->z=0.0f;
	}
		// Initializing constructor
    Vector(float x, float y, float z) {
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
	
Vector operator+(const Vector &a) {
	Vector res;
	res.x = a.x+x;
	res.y = a.y+y;
	res.z = a.z+z;
	return res;
}

Vector operator-(const Vector &a) {
	Vector res;
	res.x = a.x-x;
	res.y = a.y-y;
	res.z = a.z-z;
	return res;
}

Vector operator/(const Vector &a) {
	Vector res;
	res.x = x/a.x;
	res.y = y/a.y;
	res.z = z/a.z;
	return res;
}

Vector operator*(const Vector &a) {
	Vector res;
	res.x = x*a.x;
	res.y = y*a.y;
	res.z = z*a.z;
	return res;
}

Vector operator=(const Vector &a) {
	Vector res;
	res.x = a.x;
	res.y = a.y;
	res.z = a.z;
	return res;
}

};