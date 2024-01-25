#include <iostream>
#include <vector>
#include <cmath>

#include "Matrix.h"
#include "Vector.h"

class Matrix {

    
    public:
    Vector va;
    Vector vb;
    Vector vc;
    float* M[12];

    Matrix();

     Matrix(float m11, float m21, float m31, float m41,
            float m12, float m22, float m32, float m42,
            float m13, float m23, float m33, float m43) {

            std::vector<float> M {m11, m21, m31, m41,
                                  m12, m22, m32, m42,
                                  m13, m23, m33, m43 };

           }

    Matrix(const Vector &A, const Vector &B, const Vector &C) {

            std::vector<float> M {A.x, B.x, C.x, 1.0f,
                                  A.y, B.y, C.y, 1.0f,
                                  A.z, B.z, C.z, 1.0f};

    }
};