#include "SceneElement.h"
#include "ShaderIF.h"
#include "BasicShape.h"
#include "BasicShapeRenderer.h"

class Antenna : public SceneElement
{
public:
	Antenna(ShaderIF* sIF, float position[3], const PhongMaterial& matl);
	~Antenna();

	void getMCBoundingBox(double* xyzLimits) const; // {xmin, xmax, ymin, ymax, zmin, zmax}
	void renderAntenna();
	void render();

private:
  float xyz[6];
  static const int COUNT = 6;
  BasicShape* parts[COUNT];
  BasicShapeRenderer* partsR[COUNT];

	PhongMaterial material;
  PhongMaterial sphereMaterial = PhongMaterial(.24725, .1994, .0745, 0.75164, 0.60648, 0.22648, 0.628281, 0.555802, 0.366065, 51.2, 1);
  PhongMaterial discMaterial = PhongMaterial(.2, .2, .2, .2, .2, .2, 0.508273, 0.508273, 0.508273, 51.2, 1);
};
