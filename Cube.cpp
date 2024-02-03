#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "Cube.h"

//using namespace std;

	// Constructor
	Cube::Cube(float centerX, float centerY, float centerZ) {

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
				

};
