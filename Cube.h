#ifndef CUBE_H
#define CUBE_H

#include "glm/glm/glm.hpp">
#include <iostream>
#include <vector>

class Cube {
    
    public:

    struct Vertex {
        glm::vec3 pos;
        glm::vec3 normals;
        glm::vec4 colors;        
        glm::vec2 texCoords;        
    };
    
    //glm::vec3 vertices;
    std::vector<uint16_t> indices;
    std::vector<Vertex> Data;
    //glm::vec4 colors;
    //glm::vec2 texCoords;
    Cube();
    ~Cube();
    
};
#endif