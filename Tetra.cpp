#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Tetra {

public:
	float edge;	
	float* vertex;
	float* normals;
	float* color;
	int* indices;	

	// Constructor
	Tetra();

	Tetra(float edge) {

		this->edge = edge;

		//vertex = new Vertex[12];
		//indices = new int[12];

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

	~Tetra() {
		delete[] vertex;
		delete[] indices;

	}

		
	

};
