#include "Robot.h"
typedef float vec3[3];
typedef float vec2[2];
Robot::Robot(ShaderIF* sIF, float position[3], const PhongMaterial& matl) : SceneElement(sIF), material(matl)
{
  texID = readTextureImage("textures/face.jpg");
  int length = 20, width = 15, height = 30;
  int limbRadius = 2.6;
  int legLength = 18;
  int shoulderDim = 7;
  int headDim = 11;

  float x = position[0], y = position[1], z = position[2];
//BODY
  parts[0] = BasicShape::makeBlock(cryph::AffPoint(x, y + legLength, z), cryph::AffVector(1, 0, 0), length, cryph::AffVector(0,1,0), height, cryph::AffVector(0, 0, 1), width);
//LEGS
  x = (2 * position[0] + length) * .5; z = (2 * position[2] + width) * .5;
  parts[1] = BasicShape::makeBoundedCylinder(cryph::AffPoint(x - 4, y, z), cryph::AffPoint(x - 4, y + legLength, z), limbRadius, 20, 2);
  parts[2] = BasicShape::makeBoundedCylinder(cryph::AffPoint(x + 4, y, z), cryph::AffPoint(x + 4, y + legLength, z), limbRadius, 20, 2);
//SHOULDERS
  x = position[0] - shoulderDim; y = (2 * position[1] + height + legLength) * .7; z = (2 * position[2] + width - shoulderDim) * .5;
  parts[3] = BasicShape::makeBlock(cryph::AffPoint(x, y, z), cryph::AffVector(1, 0, 0), shoulderDim, cryph::AffVector(0,1,0), shoulderDim, cryph::AffVector(0, 0, 1), shoulderDim);
  x = position[0] + length;
  parts[4] = BasicShape::makeBlock(cryph::AffPoint(x, y, z), cryph::AffVector(1, 0, 0), shoulderDim, cryph::AffVector(0,1,0), shoulderDim, cryph::AffVector(0, 0, 1), shoulderDim);
//ARMS
  parts[5] = BasicShape::makeBoundedCylinder(cryph::AffPoint((2*x+shoulderDim)*.5, y-legLength, (2*z+shoulderDim)*.5), cryph::AffPoint((2*x+shoulderDim)*.5, y, (2*z+shoulderDim)*.5), limbRadius, 20, 2);
  x = position[0] - shoulderDim;
  parts[6] = BasicShape::makeBoundedCylinder(cryph::AffPoint((2*x+shoulderDim)*.5, y-legLength, (2*z+shoulderDim)*.5), cryph::AffPoint((2*x+shoulderDim)*.5, y, (2*z+shoulderDim)*.5), limbRadius, 20, 2);
//HEAD
  x = (2* position[0] + length - headDim) * .5; y =position[1] + height + legLength; z = (2*position[2] +width- headDim) * .5;
  float params[6] = { x, x + headDim, y, y+headDim, z, z+headDim };
  defineHead(params);



 xyz[0] = position[0] - shoulderDim; xyz[1] = position[0] + length + shoulderDim;
 xyz[2] = position[1]; xyz[3] = position[1] + height + legLength + headDim + 3;
 xyz[4] = position[2]; xyz[5] = position[2] + width;
  for(int i = 0; i < COUNT; i++)
    partsR[i] = new BasicShapeRenderer(sIF, parts[i]);
}

void Robot::defineHead(float xyz[6])
{
  vec3 vtx[] = { // The 8 unique vertices (Note the order)
		{xyz[0] ,xyz[2], xyz[5]}, {xyz[0], xyz[3], xyz[5]},
		{xyz[1], xyz[2], xyz[5]}, {xyz[1], xyz[3], xyz[5]},
		{xyz[1], xyz[2], xyz[4]}, {xyz[1], xyz[3], xyz[4]},
		{xyz[0], xyz[2], xyz[4]}, {xyz[0], xyz[3], xyz[4]}
	};
  vec2 textureCoords[] = { {0,0}, {0, 0}, {0,0}, {1, 0}, {1,0}, {1,1}, {0,0}, {0,1}};

	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);

	glGenBuffers(2, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 8*sizeof(vec3), vtx, GL_STATIC_DRAW);
	glVertexAttribPointer(shaderIF->pvaLoc("mcPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(shaderIF->pvaLoc("mcPosition"));

	glGenBuffers(3, ebo);
	for (int i=0 ; i<3 ; i++)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4*sizeof(GLuint), indexList[i], GL_STATIC_DRAW);
	}

	glDisableVertexAttribArray(shaderIF->pvaLoc("mcNormal"));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 8*sizeof(vec2), textureCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(shaderIF->pvaLoc("textureCoords"), 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(shaderIF->pvaLoc("textureCoords"));
}

Robot::~Robot()
{
  for(int i = 0; i < COUNT; i++)
  {
    delete parts[i];
    delete partsR[i];
  }
  glDeleteBuffers(3, ebo);
	glDeleteBuffers(2, vbo);
	glDeleteVertexArrays(1, vao);
}

void Robot::getMCBoundingBox(double* xyzLimits) const
{
  for(int i = 0; i < 6; i++)
    xyzLimits[i] = xyz[i];
}

void Robot::renderRobot()
{
  establishLightingEnvironment();
  establishMaterial(material);
  for(int i = 0; i < COUNT; i++)
    partsR[i]->drawShape();

    //establishMaterial(brown);
    glBindVertexArray(vao[0]);

  	// The three faces that can be drawn with glDrawArrays
  	glVertexAttrib3f(shaderIF->pvaLoc("mcNormal"), 0.0, 0.0, 1.0);
  	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  	glVertexAttrib3f(shaderIF->pvaLoc("mcNormal"), 1.0, 0.0, 0.0);
  	glDrawArrays(GL_TRIANGLE_STRIP, 2, 4);
    establishTexture(texID);
  	glVertexAttrib3f(shaderIF->pvaLoc("mcNormal"), 0.0, 0.0, -1.0);
  	glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
    glUniform1i(shaderIF->ppuLoc("usingTexture"), 0);
  	// The three faces that are drawn with glDrawElements
  	glVertexAttrib3f(shaderIF->pvaLoc("mcNormal"), -1.0, 0.0, 0.0);
  	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
  	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, nullptr);
  	glVertexAttrib3f(shaderIF->pvaLoc("mcNormal"), 0.0, -1.0, 0.0);
  	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[1]);
  	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, nullptr);
  	glVertexAttrib3f(shaderIF->pvaLoc("mcNormal"), 0.0, 1.0, 0.0);
  	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[2]);
  	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, nullptr);

}

void Robot::render()
{
	GLint pgm;
	glGetIntegerv(GL_CURRENT_PROGRAM, &pgm);
	glUseProgram(shaderIF->getShaderPgmID());

	cryph::Matrix4x4 mc_ec, ec_lds;
	getMatrices(mc_ec, ec_lds);
	float mat[16];
	glUniformMatrix4fv(shaderIF->ppuLoc("mc_ec"), 1, false, mc_ec.extractColMajor(mat));
	glUniformMatrix4fv(shaderIF->ppuLoc("ec_lds"), 1, false, ec_lds.extractColMajor(mat));

	renderRobot();

	glUseProgram(pgm);
}
