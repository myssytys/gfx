#ifndef TETRA_H
#define TETRA_H

class Tetra {

public:

   float edge;	
	float* vertex;
	float* normals;
	float* color;
	int* indices;
  // Tetra();
   Tetra(float edge);

};

#endif