#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

#include "Vector.cpp"

using namespace std;

#define PI 3.14159265358979323846

class Sphere {

	struct Vertex {
		float x,y,z;
		float r,g,b;
		float nx,ny,nz;
	};

	struct Edge {
		float a,b;
	};

	float radius;
	float centerX, centerY, centerZ;
	float phi, theta, dtheta, dphi;
	int hseg, vseg;
	Vertex* vertex;
	Vertex* icovertex;
	Edge* edge;
	//Vector* vertex;
	int* indices;
	int* icoindices;


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
		indices = new int[vseg*hseg];
		icovertex = new Vertex[36];
		icoindices = new int[120];
		edge = new Edge[60];

	}

	void DrawSphere() {

		int i,j;

		dtheta = 2.0 * PI / (vseg - 1);	
		dphi = PI / (hseg - 1);

		for(i=0; i<hseg; i++) {

			theta += dtheta;

			for(j=0;j<vseg;j++) {

				phi += dphi;

				vertex[i*j].x = sin(phi) * cos(theta);
				vertex[i*j].y = sin(phi) * sin(theta);
				vertex[i*j].z = cos(phi);


			}



		}

	}

	void generateIcosahedron() {

  	float phi = (1.0 + sqrt(5.0)) / 2.0;
	float du = 1.0 / sqrt(phi * phi + 1.0);
    float dv = phi * du;
	
	icovertex[0].x=0.0f; icovertex[0].y=+dv; icovertex[0].z=+du;
	icovertex[1].x=0.0f; icovertex[1].y=+dv; icovertex[1].z=-du;
	icovertex[2].x=0.0f; icovertex[2].y=-dv; icovertex[2].z=+du;
	icovertex[3].x=0.0f; icovertex[3].y=-dv; icovertex[3].z=-du;
	icovertex[4].x=+du; icovertex[4].y=0.0f; icovertex[4].z=+dv;
	icovertex[5].x=-du; icovertex[5].y=0.0f; icovertex[5].z=+dv;
	icovertex[6].x=+du; icovertex[6].y=0.0f; icovertex[6].z=-dv;
	icovertex[7].x=-du;; icovertex[7].y=0.0f; icovertex[7].z=-dv;
	icovertex[8].x=+dv; icovertex[8].y=+du; icovertex[8].z=0.0f;
	icovertex[9].x=+dv; icovertex[9].y=-du; icovertex[9].z=0.0f;
	icovertex[10].x=-dv; icovertex[10].y=+du; icovertex[10].z=0.0f;
	icovertex[11].x=-dv; icovertex[11].y=-du; icovertex[11].z=0.0f;

    edge[0].a=0; edge[0].b=1;
	edge[1].a=0; edge[1].b=4;
	edge[2].a=0; edge[2].b=5;
	edge[3].a=0; edge[3].b=8;
	edge[4].a=0; edge[4].b=10;
	edge[5].a=1; edge[5].b=6;
	edge[6].a=1; edge[6].b=7;
	edge[7].a=1; edge[7].b=8;
	edge[8].a=1; edge[8].b=10;
	edge[9].a=2; edge[9].b=3;
	edge[10].a=2; edge[10].b=4;
	edge[11].a=2; edge[11].b=5;
	edge[12].a=2; edge[12].b=9;
	edge[13].a=2; edge[13].b=11;
	edge[14].a=3; edge[14].b=6;
	edge[15].a=3; edge[15].b=7;
	edge[16].a=3; edge[16].b=9;
	edge[17].a=3; edge[17].b=11;
	edge[18].a=4; edge[18].b=5;
	edge[19].a=4; edge[19].b=8;
	edge[20].a=4; edge[20].b=9;
	edge[21].a=5; edge[21].b=10;
	edge[22].a=5; edge[22].b=11;
	edge[23].a=6; edge[23].b=7;
	edge[24].a=6; edge[24].b=8;
	edge[25].a=6; edge[25].b=9;
	edge[26].a=7; edge[26].b=10;
	edge[27].a=7; edge[27].b=11;
	edge[28].a=8; edge[28].b=9;
	edge[29].a=10; edge[29].b=11;
	
    icoindices[0] = 0; icoindices[1] = 1; icoindices[2] = 8;
	icoindices[3] = 0; icoindices[4] = 7; icoindices[5] = 3;
    icoindices[6] = 0; icoindices[7] = 4; icoindices[8] = 5;
	icoindices[9] = 1; icoindices[10]=18; icoindices[11]= 2;
	icoindices[12]= 0; icoindices[13]= 5; icoindices[14]=10;
	icoindices[15]= 2; icoindices[16]=21; icoindices[17]= 4;
    icoindices[18]= 0; icoindices[19]= 8; icoindices[20]= 4;
    icoindices[21]= 3; icoindices[22]=19; icoindices[23]= 1;
	icoindices[24]= 0; icoindices[25]=10; icoindices[26]= 1;
    icoindices[27]= 4; icoindices[28]= 8; icoindices[29]= 0;
	icoindices[30]= 1; icoindices[31]= 6; icoindices[32]= 8;
	icoindices[33]= 5; icoindices[34]=24; icoindices[35]= 7;
	icoindices[36]= 1; icoindices[37]= 7; icoindices[38]= 6;
	icoindices[39]= 6; icoindices[40]=23; icoindices[41]= 5;
    icoindices[42]= 1; icoindices[43]=10; icoindices[44]= 7;
	icoindices[45]= 8; icoindices[46]=26; icoindices[47]= 6;
    icoindices[48]= 2; icoindices[49]= 3; icoindices[50]=11;
	icoindices[51]= 9; icoindices[52]=17; icoindices[53]=13;
    icoindices[54]= 2; icoindices[55]= 4; icoindices[56]= 9;
	icoindices[57]=10; icoindices[58]=20; icoindices[59]=12;
    icoindices[60]= 2; icoindices[61]= 5; icoindices[62]= 4;
	icoindices[63]=11; icoindices[64]=18; icoindices[65]=10;
    icoindices[66]= 2; icoindices[67]= 9; icoindices[68]= 3;
	icoindices[69]=12; icoindices[70]=16; icoindices[71]= 9;
    icoindices[72]= 2; icoindices[73]=11; icoindices[74]= 5;
	icoindices[75]=13; icoindices[76]=22; icoindices[77]=11;
    icoindices[78]= 3; icoindices[79]= 6; icoindices[80]= 7;
	icoindices[81]=14; icoindices[82]=23; icoindices[83]=15;
    icoindices[84]= 3; icoindices[85]= 7; icoindices[86]=11;
	icoindices[87]=15; icoindices[88]=27; icoindices[89]=17;
    icoindices[90]= 3; icoindices[91]= 9; icoindices[92]= 6;
	icoindices[93]=16; icoindices[94]=25; icoindices[95]=14;
    icoindices[96]= 4; icoindices[97]= 8; icoindices[98]= 9;
	icoindices[99]=19; icoindices[100]=28;icoindices[101]=20;
	icoindices[102]=5; icoindices[103]=11;icoindices[104]=10;
	icoindices[105]=22;icoindices[106]=29;icoindices[108]=21;
    icoindices[109]=6; icoindices[110]= 9;icoindices[111]= 8;
	icoindices[112]=25;icoindices[113]=28;icoindices[114]=24;
    icoindices[115]=7; icoindices[116]=10;icoindices[117]=11;
	icoindices[118]=26;icoindices[119]=29;icoindices[120]=27;
	

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
