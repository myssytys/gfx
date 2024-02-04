#include <vector>

#include "Tetra.h"

	Tetra::Tetra(float edge) {

	    std::vector<float> vertex{
			0.0f, edge, 0.0f,
			edge/2, -edge/2, 0.0f,
			0.0f, 0.0f, edge,
			-edge/2, -edge/2.0f, 0.0f
		};		
		

		std::vector<int> indices{
			0,1,2,
			0,2,3,
			0,3,1,
			1,3,2
		};
	}



