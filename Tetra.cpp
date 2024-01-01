#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;

class Tetra {

	float cX, cY, cZ;	

	// Constructor
	Tetra(float cX, float cY, float cZ) {

		
		this->cX = cX;
		this->cY = cY;
		this->cZ = cZ;

	    float vertex[]={
	
		};

	int indices[]={
		 	};
		
	
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
