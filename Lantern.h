#include "SceneElement.h"
#include "ShaderIF.h"
#include "BasicShape.h"
#include "BasicShapeRenderer.h"

class Lantern : public SceneElement
{
	typedef float vec3[3];
public:
	Lantern(ShaderIF* sIF, float position[3], const PhongMaterial& matl);
	~Lantern();

	void getMCBoundingBox(double* xyzLimits) const; // {xmin, xmax, ymin, ymax, zmin, zmax}
	void GetLightPosition(float* position);
	void renderLantern();
	void render();

private:
  float xyz[6];
  static const int COUNT = 13;
  BasicShape* parts[COUNT];
  BasicShapeRenderer* partsR[COUNT];
	float lightPos[3];

	PhongMaterial material;
  PhongMaterial barMaterial = PhongMaterial(.19225, .19225, .19225, 0.50754, 0.50754, 0.50754, 0.508273, 0.508273, 0.508273, 51.2, 1);
	PhongMaterial lightMaterial = PhongMaterial(.1745, .1175, .1175, 0.61424, 0.04136, 0.04136, 0.727811, 0.626959, 0.626959, 76.8, .55);
};
