#ifndef VECTOR_H
#define VECTOR_H

class Vector {

    public:
    float x,y,z;

    Vector();
    Vector(float x, float y, float z);    
    float dot(const Vector &a, const Vector &b);
    Vector cross(const Vector &a, const Vector &b);
    float length(const Vector &a);
    Vector normalize(const Vector &a);
    Vector operator+(const Vector &a);
    Vector operator-(const Vector &a);
    Vector operator/(const Vector &a);
    Vector operator*(const Vector &a);
    Vector operator=(const Vector &a);
    ~Vector();


};

#endif