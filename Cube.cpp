#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

//using namespace std;

class Cube {

	float centerX, centerY, centerZ;	
	float* vertex;
	float* indices;

	// Constructor
	Cube(float centerX, float centerY, float centerZ) {

		
		this->centerX = centerX;
		this->centerY = centerY;
		this->centerZ = centerZ;

	    std::vector<float> vertex{
		/*   X     Y     Z 
		/* front face */
		-1.0, -1.0,  1.0,
		 1.0, -1.0,  1.0,
		-1.0,  1.0,  1.0,
		 1.0,  1.0,  1.0,
		 /* back face */
		 1.0, -1.0, -1.0,
		-1.0, -1.0, -1.0,
		 1.0,  1.0, -1.0,
		-1.0,  1.0, -1.0,
		/* left  face */
		-1.0, -1.0, -1.0,
		-1.0, -1.0,  1.0,
		-1.0,  1.0, -1.0,
		-1.0,  1.0,  1.0,
		/* right face */
		 1.0, -1.0,  1.0,
		 1.0, -1.0, -1.0,
		 1.0,  1.0,  1.0,
		 1.0,  1.0, -1.0,
		/* top face */
		-1.0,  1.0,  1.0,
		 1.0,  1.0,  1.0,
		-1.0,  1.0, -1.0,
		 1.0,  1.0, -1.0,
		/* bottom face */
		 1.0, -1.0,  1.0,
		-1.0, -1.0,  1.0,
		 1.0, -1.0, -1.0,
		-1.0, -1.0, -1.0,
	};


	std::vector<int> indices={
		 0, 1, 2,  2, 1, 3,	/* front */
		 4, 5, 6,  6, 5, 7,	/* back */
		 8, 9,10, 10, 9,11,	/* left */
		12,13,14, 14,13,15,	/* right */
		16,17,18, 18,17,19,	/* top */
		20,21,22, 22,21,23	/* bottom */
	};
	}

	~Cube() {
		//delete[] vertex;
		//delete[] indices;

	}

	float getCenterX() {
		return centerX;
	}
	float getCenterY() {
		return centerY;
	}
	float getCenterZ() {
		return centerZ;
	}

	void setCenterX(float cx) {
		centerX = cx;
	}
	void setCenterY(float cy) {
		centerY = cy;
	}
	void setCenterZ(float cz) {
		centerZ = cz;
	}
	
	
	

};
