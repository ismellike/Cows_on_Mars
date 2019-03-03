#include "SceneElement.h"
#include "ShaderIF.h"
#include "BasicShape.h"
#include "BasicShapeRenderer.h"

class Robot : public SceneElement
{
public:
	Robot(ShaderIF* sIF, float position[3], const PhongMaterial& matl);
	~Robot();

	void getMCBoundingBox(double* xyzLimits) const; // {xmin, xmax, ymin, ymax, zmin, zmax}
	void renderRobot();
	void render();
	void defineHead(float xyz[6]);

private:
	GLuint vao[1];
	GLuint vbo[2];
	GLuint ebo[3];
	GLuint indexList[3][4]= {
	{ 6, 7, 0, 1 }, // xmin face
	{ 6, 0, 4, 2 }, // ymin face
	{ 1, 7, 3, 5 }  // ymax face
	};
  float xyz[6];
  static const int COUNT = 7;
  BasicShape* parts[COUNT];
  BasicShapeRenderer* partsR[COUNT];
	GLuint texID;

	PhongMaterial material;
};
