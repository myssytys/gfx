#ifndef SPHERE_H
#define SPHERE_H

class Sphere {

    Sphere(float centerX, float centerY, float centerZ, float radius, int hseg, int vseg);
    void UVSphere();
    void generateIcosahedron(float size);

};

#endif