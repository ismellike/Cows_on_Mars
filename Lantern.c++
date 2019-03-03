#include "Lantern.h"
#include <math.h>

Lantern::Lantern(ShaderIF* sIF, float position[3], const PhongMaterial& matl) : SceneElement(sIF), material(matl)
{
  float baseR = 7;
  float barR = .5;
  float barLength = 8;
  float baseThickness = 2;
  float coneHeight = 3.5;

  float x = position[0], y = position[1], z = position[2];
  lightPos[0] = x; lightPos[1] = y + coneHeight + baseThickness + .05; lightPos[2] = z;
  //BASE
  parts[0] = BasicShape::makeBoundedCylinder(cryph::AffPoint(x, y, z), cryph::AffPoint(x, y + baseThickness, z), baseR, 20, 2);
  //LIGHTSOURCE
  parts[2] = BasicShape::makeBoundedCone(cryph::AffPoint(x, y+baseThickness, z), cryph::AffPoint(x, y+baseThickness+coneHeight, z), 2.5, .01, 20, 20);
  //TOP DISC
  parts[1] = BasicShape::makeBoundedCylinder(cryph::AffPoint(x, y + baseThickness + barLength, z), cryph::AffPoint(x, y + 2 * baseThickness + barLength, z), baseR, 20, 2);

  //BARS
  for(int i = 0; i < 10; i++)
  {
    float xB = (baseR - barR) * cos(2*M_PI/10.0 * i) + x;
    float zB = (baseR - barR) * sin(2*M_PI/10.0 * i) + z;
    parts[i + 3] = BasicShape::makeBoundedCylinder(cryph::AffPoint(xB, y + baseThickness, zB), cryph::AffPoint(xB, y + baseThickness + barLength, zB), barR, 20, 2);
  }

 xyz[0] = x - baseR; xyz[1] = x + baseR;
  xyz[2] = y; xyz[3] = y + 2 * baseThickness + barLength;
  xyz[4] = z - baseR; xyz[5] = z + baseR;

  for(int i = 0; i < COUNT; i++)
    partsR[i] = new BasicShapeRenderer(sIF, parts[i]);
}

void Lantern::GetLightPosition(float* position)
{
  for(int i = 0; i < 3; i++)
    position[i] = lightPos[i];
}

Lantern::~Lantern()
{
  for(int i = 0; i < COUNT; i++)
  {
    delete parts[i];
    delete partsR[i];
  }
}

void Lantern::getMCBoundingBox(double* xyzLimits) const
{
  for(int i = 0; i < 6; i++)
    xyzLimits[i] = xyz[i];
}

void Lantern::renderLantern()
{
  establishMaterial(material);
  partsR[0]->drawShape();
  partsR[1]->drawShape();
  establishMaterial(lightMaterial);
  partsR[2]->drawShape();
  establishMaterial(barMaterial);
  for(int i = 3; i<13; i++)
  {
    partsR[i]->drawShape();
  }
}

void Lantern::render()
{
	GLint pgm;
	glGetIntegerv(GL_CURRENT_PROGRAM, &pgm);
	glUseProgram(shaderIF->getShaderPgmID());

  establishView();
  establishLightingEnvironment();

	renderLantern();

	glUseProgram(pgm);
}
