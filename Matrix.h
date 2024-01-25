#ifndef MATRIX_H
#define MATRIX_H

#include "Vector.h";

class Matrix {

    public:
    Vector* M;

    Matrix(float m11, float m21, float m31, 
           float m12, float m22, float m32,
           float m13, float m23, float m33);

    Matrix(const Vector &A, const Vector &B, const Vector &C);

    ~Matrix();

};

#endif;