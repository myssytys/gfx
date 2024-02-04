#include <iostream>
#include <vector>
#include <cmath>

#include "Matrix.h"

        // Default constructor
    Matrix::Matrix() {
            std::vector<float> iM {1.0f, 0.0f, 0.0f, 0.0f,
                                   0.0f, 1.0f, 0.0f, 0.0f,
                                   0.0f, 0.0f, 1.0f, 0.0f};
            
            std::vector<float> M {0.0f, 0.0f, 0.0f, 0.0f,
                                  0.0f, 0.0f, 0.0f, 0.0f,
                                  0.0f, 0.0f, 0.0f, 0.0f};
    };

    Matrix::Matrix(float m11, float m21, float m31, float m41,
            float m12, float m22, float m32, float m42,
            float m13, float m23, float m33, float m43) {

                // Initialize 4x4 matrix
            std::vector<float> M {m11, m21, m31, m41,
                                  m12, m22, m32, m42,
                                  m13, m23, m33, m43 };

                // Make Identity Matrix
            std::vector<float> iM {1.0f, 0.0f, 0.0f, 0.0f,
                                   0.0f, 1.0f, 0.0f, 0.0f,
                                   0.0f, 0.0f, 1.0f, 0.0f};


           }
    // Matrix init with 3 vectors
    Matrix::Matrix(const Vector &A, const Vector &B, const Vector &C) {

            std::vector<float> M {A.x, B.x, C.x, 1.0f,
                                  A.y, B.y, C.y, 1.0f,
                                  A.z, B.z, C.z, 1.0f};

            std::vector<float> iM {1.0f, 0.0f, 0.0f, 0.0f,
                                   0.0f, 1.0f, 0.0f, 0.0f,
                                   0.0f, 0.0f, 1.0f, 0.0f};

    }

    Matrix Matrix::operator+(const Matrix &A) {
        Matrix res;
        res.M[0] = M[0]+A.M[0];
        res.M[1] = M[1]+A.M[1];
        res.M[2] = M[2]+A.M[2];
        res.M[3] = M[3]+A.M[3];
        res.M[4] = M[4]+A.M[4];
        res.M[5] = M[5]+A.M[5];
        res.M[6] = M[6]+A.M[6];
        res.M[7] = M[7]+A.M[7];
        res.M[8] = M[8]+A.M[8];
        res.M[9] = M[9]+A.M[9];
        res.M[10] = M[10]+A.M[10];
        res.M[11] = M[11]+A.M[11];        

        return res;

    }
    Matrix Matrix::operator-(const Matrix &A) {

        Matrix res;

        res.M[0] = M[0]-A.M[0];
        res.M[1] = M[1]-A.M[1];
        res.M[2] = M[2]-A.M[2];
        res.M[3] = M[3]-A.M[3];
        res.M[4] = M[4]-A.M[4];
        res.M[5] = M[5]-A.M[5];
        res.M[6] = M[6]-A.M[6];
        res.M[7] = M[7]-A.M[7];
        res.M[8] = M[8]-A.M[8];
        res.M[9] = M[9]-A.M[9];
        res.M[10] = M[10]-A.M[10];
        res.M[11] = M[11]-A.M[11];

        return res;      

    }
    Matrix Matrix::operator/(const Matrix &A) {

        Matrix res;

        res.M[0] = M[0]/A.M[0];
        res.M[1] = M[1]/A.M[1];
        res.M[2] = M[2]/A.M[2];
        res.M[3] = M[3]/A.M[3];
        res.M[4] = M[4]/A.M[4];
        res.M[5] = M[5]/A.M[5];
        res.M[6] = M[6]/A.M[6];
        res.M[7] = M[7]/A.M[7];
        res.M[8] = M[8]/A.M[8];
        res.M[9] = M[9]/A.M[9];
        res.M[10] = M[10]/A.M[10];
        res.M[11] = M[11]/A.M[11];

        return res;      

    }
     Matrix Matrix::operator*(const Matrix& A) {

        Matrix res;

        res.M[0] = M[0]*A.M[0];
        res.M[1] = M[1]*A.M[1];
        res.M[2] = M[2]*A.M[2];
        res.M[3] = M[3]*A.M[3];
        res.M[4] = M[4]*A.M[4];
        res.M[5] = M[5]*A.M[5];
        res.M[6] = M[6]*A.M[6];
        res.M[7] = M[7]*A.M[7];
        res.M[8] = M[8]*A.M[8];
        res.M[9] = M[9]*A.M[9];
        res.M[10] = M[10]*A.M[10];
        res.M[11] = M[11]*A.M[11];

        return res;      
    }
    Matrix Matrix::operator=(const Matrix &A) {

        Matrix res;

        res.M[0] = A.M[0];
        res.M[1] = A.M[1];
        res.M[2] = A.M[2];
        res.M[3] = A.M[3];
        res.M[4] = A.M[4];
        res.M[5] = A.M[5];
        res.M[6] = A.M[6];
        res.M[7] = A.M[7];
        res.M[8] = A.M[8];
        res.M[9] = A.M[9];
        res.M[10] = A.M[10];
        res.M[11] = A.M[11];

        return res;      

    }


