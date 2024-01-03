#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;

class Tetra {

	struct Vertex {
		float x, y, z;
		float r, g ,b;
		float nx, ny, nz;
	};

	float cX, cY, cZ;
	float edge;
	float* vertex;
	int* indices;	

	// Constructor
	Tetra(float cX, float cY, float cZ, float edge) {

		
		this->cX = cX;
		this->cY = cY;
		this->cZ = cZ;
		this->edge = edge;

		vertex = new float[12];
		indices = new int[12];

	    vertex[0] = 0.0f;    vertex[1] = edge;        vertex[2] = 0.0f;
		vertex[3] =	edge/2;  vertex[4] = -edge/2;     vertex[5] = 0.0f;
		vertex[6] = 0.0f;    vertex[7] = 0.0f;        vertex[8] = edge;
		vertex[9] = -edge/2; vertex[10] = -edge/2.0f; vertex[11] = 0.0f;
			

		
	   indices[0] = 0; indices[1]  = 1; indices[2] = 2;
	   indices[3] = 0; indices[4]  = 2; indices[5] = 3;
	   indices[6] = 0; indices[7]  = 3; indices[8] = 1;
	   indices[9] = 1; indices[10] = 3; indices[11] = 2;
	
}
	

	~Tetra() {
		delete[] vertex;
		delete[] indices;

	}

	float getCenterX() {
		return cX;
	}
	float getCenterY() {
		return cY;
	}
	float getCenterZ() {
		return cZ;
	}

	void setCenterX(float cx) {
		cX = cx;
	}
	void setCenterY(float cy) {
		cY = cy;
	}
	void setCenterZ(float cz) {
		cZ = cz;
	}
	
	
	

};
