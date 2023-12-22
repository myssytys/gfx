#include <cmath>

#define PI 3.14159265358979323846

class Sphere {

	float radius;
	float centerX, centerY, centerZ;
	float phi, theta, dtheta, dphi;
	int hseg, vseg;


	// Constructor
	Sphere(float radius, float centerX, float centerY, float centerZ, int hseg, int vseg) {

		this->radius = radius;
		this->centerX = centerX;
		this->centerY = centerY;
		this->centerZ = centerZ;
		this->vseg = vseg;
		this->hseg = hseg;

		this->phi = 0.0f;
		this->theta = 0.0f;
		this->dphi = 0.0f;
		this->dtheta = 0.0f;		

	}

	void DrawSphere() {

		int i,j;

		dtheta = 2.0 * PI / (vseg - 1);	
		dphi = PI / (hseg - 1);


		float x[hseg*vseg];
		float y[hseg*vseg];
		float z[hseg*vseg];


		for(i=0; i<hseg; i++) {

			for(j=0;j<vseg;j++) {


				x[i*j] = sin(phi) * cos(theta);
				y[i*j] = sin(phi) * sin(theta);
				z[i*j] = cos(phi);
				
				x[i*j+1] = x+sin(phi) * cos(theta);
				y[i*j+1] = y+sin(phi) * cos(theta);
				z[i*j+1] = z+cos(phi);

				x[i*j+2] = x+sin(phi) * cos(theta);
				y[i*j+2] = y+sin(phi) * cos(theta);
				z[i*j+2] = z+cos(phi);


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
