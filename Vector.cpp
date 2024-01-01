#include <cmath>

class Vector {

    float x,y,z;

    Vector(float x, float y, float z) {
        this->x=x;
        this->y=y;
        this->z=z;
    }

float dot(const Vector &a, const Vector &b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector cross(const Vector &a, const Vector &b)
{
	return Vector(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x);
}

float length(const Vector &a)
{
	return std::sqrt(dot(a, a));
}

Vector normalize(const Vector &a)
{
	const double lrcp = 1.0 / std::sqrt(dot(a, a));
	return Vector(a.x * lrcp, a.y * lrcp, a.z * lrcp);
}

};