#include "Antenna.h"

Antenna::Antenna(ShaderIF* sIF, float position[3], const PhongMaterial& matl) : SceneElement(sIF), material(matl)
{
  float height = 7 * 20;
  float x = position[0], y = position[1], z = position[2];
  parts[0] = BasicShape::makeBoundedCylinder(cryph::AffPoint(x, y, z), cryph::AffPoint(x, y + height, z), 6, 20, 2);
  for(int i = 0; i < 4; i++)
  {
    y = height / 5 * (i + 1);
    parts[i + 1] = BasicShape::makeBoundedCylinder(cryph::AffPoint(x, y, z), cryph::AffPoint(x, y + 4, z), 35 - i* 5, 20, 2);
  }

  y = position[1] + height;
  parts[5] = BasicShape::makeSphere(cryph::AffPoint(x, y, z), 10, 20, 20);

  xyz[0] = position[0] - 35; xyz[1] = position[0] + 35;
  xyz[2] = position[1]; xyz[3] = position[1] + height + 10;
  xyz[4] = xyz[0]; xyz[5] = xyz[1];
  for(int i = 0; i < COUNT; i++)
    partsR[i] = new BasicShapeRenderer(sIF, parts[i]);
}

Antenna::~Antenna()
{
  for(int i = 0; i < COUNT; i++)
  {
    delete parts[i];
    delete partsR[i];
  }
}

void Antenna::getMCBoundingBox(double* xyzLimits) const
{
  for(int i = 0; i < 6; i++)
    xyzLimits[i] = xyz[i];
}

void Antenna::renderAntenna()
{
  establishMaterial(material);
  bool disc = false;
  for(int i = 0; i < COUNT - 1; i++)
  {
    if(i > 0 && !disc)
    {
      establishMaterial(discMaterial);
      disc = true;
    }
    partsR[i]->drawShape();
  }

  establishMaterial(sphereMaterial);
  glUniform1i(shaderIF->ppuLoc("processGeo"), 1);
  partsR[COUNT - 1]->drawShape();
  glUniform1i(shaderIF->ppuLoc("processGeo"), 0);
}

void Antenna::render()
{
	GLint pgm;
	glGetIntegerv(GL_CURRENT_PROGRAM, &pgm);
	glUseProgram(shaderIF->getShaderPgmID());

  establishView();
  establishLightingEnvironment();

	renderAntenna();

	glUseProgram(pgm);
}
