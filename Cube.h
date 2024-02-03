#ifndef CUBE_H
#define CUBE_H

class Cube {
    
    public:
    Cube(float centerX, float centerY, float centerZ);

    float getCenterX();
    float getCenterY();
    float getCenterZ();

    void setCenterX(float cx);
    void setCenterY(float cy);
    void setCenterZ(float cz);
};

#endif