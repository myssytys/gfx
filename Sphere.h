#ifndef SPHERE_H
#define SPHERE_H

class Sphere {

public:

    typedef struct {
        int a;
        int b;
    } Edge ;
    Edge* edge;
    float* vertices;
    int* indices;
    float phi;
    float dphi;
    float theta;
    float dtheta;
    int hseg;
    int vseg;
    float radius;

    Sphere(float centerX, float centerY, float centerZ, float radius, int hseg, int vseg);
    void UVSphere();
    void generateIcosahedron(float size);
    float getRadius();
    void setRadius(float r);


};

#endif