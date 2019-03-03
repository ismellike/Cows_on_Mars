// Floor.c++

#include "Floor.h"
typedef float vec3[3];
typedef float vec2[2];

Floor::Floor(ShaderIF* sIF, float origin[3], float length[3], const PhongMaterial& matl) : SceneElement(sIF), material(matl)
{
	xyz[0] = origin[0]; xyz[1] = origin[0] + length[0];
	xyz[2] = origin[1]; xyz[3] = origin[1] + length[1];
	xyz[4] = origin[2]; xyz[5] = origin[2] + length[2];
	texID = readTextureImage("textures/mars_floor.jpg");
	defineGeometry();
}

Floor::~Floor()
{
	glDeleteBuffers(3, ebo);
	glDeleteBuffers(2, vbo);
	glDeleteVertexArrays(1, vao);
}

void Floor::defineGeometry()
{
	vec3 vtx[] = { // The 8 unique vertices (Note the order)
		{xyz[0] ,xyz[2], xyz[5]}, {xyz[0], xyz[3], xyz[5]},
		{xyz[1], xyz[2], xyz[5]}, {xyz[1], xyz[3], xyz[5]},
		{xyz[1], xyz[2], xyz[4]}, {xyz[1], xyz[3], xyz[4]},
		{xyz[0], xyz[2], xyz[4]}, {xyz[0], xyz[3], xyz[4]}
	};
	vec2 textureCoords[] = { {0,0}, {0, 0}, {0,1}, {1, 0}, {0,0}, {1,1}, {0,0}, {0,1}};

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

void Floor::getMCBoundingBox(double* xyzLimits) const
{
	for (int i=0 ; i<6 ; i++)
		xyzLimits[i] = xyz[i];
}

void Floor::renderFloor()
{
	glBindVertexArray(vao[0]);
	establishMaterial(material);
	// The three faces that can be drawn with glDrawArrays
	glVertexAttrib3f(shaderIF->pvaLoc("mcNormal"), 0.0, 0.0, 1.0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glVertexAttrib3f(shaderIF->pvaLoc("mcNormal"), 1.0, 0.0, 0.0);
	glDrawArrays(GL_TRIANGLE_STRIP, 2, 4);
	glVertexAttrib3f(shaderIF->pvaLoc("mcNormal"), 0.0, 0.0, -1.0);
	glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);

	// The three faces that are drawn with glDrawElements
	glVertexAttrib3f(shaderIF->pvaLoc("mcNormal"), -1.0, 0.0, 0.0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, nullptr);
	glVertexAttrib3f(shaderIF->pvaLoc("mcNormal"), 0.0, -1.0, 0.0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[1]);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, nullptr);
	establishTexture(texID);
	glVertexAttrib3f(shaderIF->pvaLoc("mcNormal"), 0.0, 1.0, 0.0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[2]);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, nullptr);
	glUniform1i(shaderIF->ppuLoc("usingTexture"), 0);

}

void Floor::render()
{
	GLint pgm;
	glGetIntegerv(GL_CURRENT_PROGRAM, &pgm);
	glUseProgram(shaderIF->getShaderPgmID());
	establishView();
	establishLightingEnvironment();

	renderFloor();

	glUseProgram(pgm);
}
