#ifndef Floor_H
#define Floor_H
#ifdef __APPLE_CC__
#include "GLFW/glfw3.h"
#else
#include <GL/gl.h>
#endif

#include "SceneElement.h"
#include "ShaderIF.h"

class Floor : public SceneElement
{
public:
	Floor(ShaderIF* sIF, float origin[3], float length[3], const PhongMaterial& matl);
	~Floor();

	void getMCBoundingBox(double* xyzLimits) const; // {xmin, xmax, ymin, ymax, zmin, zmax}
	void renderFloor();
	void render();

private:
	GLuint vao[1];
	GLuint vbo[2];
	GLuint ebo[3];
	GLuint texID;

	float xyz[6];
	PhongMaterial material;

	GLuint indexList[3][4]= {
	{ 6, 7, 0, 1 }, // xmin face
	{ 6, 0, 4, 2 }, // ymin face
	{ 1, 7, 3, 5 }  // ymax face
	};
	void defineGeometry();
};
#endif
