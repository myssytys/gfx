#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;

#define PI 3.14159265358979323846

class Sphere {

	struct Vertex {
		float x,y,z;
		float r,g,b;
		float nx,ny,nz;
	};
	float radius;
	float centerX, centerY, centerZ;
	float phi, theta, dtheta, dphi;
	float *x,*y,*z;
	int hseg, vseg;
	Vertex* vertex;


	// Constructor
	Sphere(float centerX, float centerY, float centerZ, float radius, int hseg, int vseg) {

		this->radius = radius;
		this->centerX = centerX;
		this->centerY = centerY;
		this->centerZ = centerZ;
		this->vseg = vseg;
		this->hseg = hseg;

		this->phi = 0.0f;
		this->theta = 0.0f;
		this->dphi = 2*PI/(vseg-1);
		this->dtheta = PI/(hseg-1);

		vertex = new Vertex[vseg*hseg];

	}

	void DrawSphere() {

		int i,j;

		dtheta = 2.0 * PI / (vseg - 1);	
		dphi = PI / (hseg - 1);

		for(i=0; i<hseg; i++) {

			theta += dtheta;

			for(j=0;j<vseg;j++) {

				phi += dphi;

				x[i*j] = sin(phi) * cos(theta);
				y[i*j] = sin(phi) * sin(theta);
				z[i*j] = cos(phi);

			}



		}





	}



	float getRadius() {
		return radius;
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

	void setRadius(float r) {
		radius = r;
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
