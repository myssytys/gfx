#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;

class Tetra {

	float cX, cY, cZ;
	float *vertex;
	int *indices;	

	// Constructor
	Tetra(float cX, float cY, float cZ) {

		
		this->cX = cX;
		this->cY = cY;
		this->cZ = cZ;

		vertex = (float*)malloc(12*3);
		indices = (int*)malloc(12);

	    vertex={

			
	
		};

	   indices={
		 
		    0, 1, 2,  // Face 0
            0, 2, 3,  // Face 1
            0, 3, 1,  // Face 2
            1, 3, 2   // Face 3
	};
	
	}
	

	~Tetra() {
		delete vertex;
		delete indices;

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
