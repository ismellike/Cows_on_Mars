// SceneElement.c++

#include "SceneElement.h"
#include "OpenGLImageReader.h"
#include "ExtendedController.h"

float SceneElement::lightPos[4*MAX_NUM_LIGHTS];
bool SceneElement::posInModelCoordinates[MAX_NUM_LIGHTS];
float posToGLSL[4*MAX_NUM_LIGHTS];
float SceneElement::lightStrength[3*MAX_NUM_LIGHTS];
float SceneElement::globalAmbient[] = { 0.2, 0.2, 0.2 };

SceneElement::SceneElement(ShaderIF* sIF) : shaderIF(sIF)
{
}

SceneElement::~SceneElement()
{
}

void SceneElement::establishLightingEnvironment()
{
	float ecLightPosition[4*MAX_NUM_LIGHTS];
	cryph::Matrix4x4 mc_ec, ec_lds;
	getMatrices(mc_ec,ec_lds);

	for(int i = 0; i < MAX_NUM_LIGHTS; i++)
	{
		if(posInModelCoordinates[i])
		{
			if(lightPos[4*i+3] == 1)
			{
				cryph::AffPoint point(lightPos[4*i], lightPos[4*i+1], lightPos[4*i+2]);
				point = mc_ec*point;
				ecLightPosition[4*i] = point.x;
				ecLightPosition[4*i+1] = point.y;
				ecLightPosition[4*i+2] = point.z;
				ecLightPosition[4*i+3] = lightPos[4*i+3];
			}
			else
			{
				cryph::AffVector vec(lightPos[4*i], lightPos[4*i+1], lightPos[4*i+2]);
				vec = mc_ec*vec;
				ecLightPosition[4*i] = vec.dx;
				ecLightPosition[4*i+1] = vec.dy;
				ecLightPosition[4*i+2] = vec.dz;
				ecLightPosition[4*i+3] = lightPos[4*i+3];
			}
		}
		else
		{
			ecLightPosition[4*i] = lightPos[4*i];
			ecLightPosition[4*i+1] = lightPos[4*i+1];
			ecLightPosition[4*i+2] = lightPos[4*i+2];
			ecLightPosition[4*i+3] = lightPos[4*i+3];
		}
	}

	glUniform1i(shaderIF->ppuLoc("actualNumLights"), MAX_NUM_LIGHTS);
	glUniform3fv(shaderIF->ppuLoc("lightStrength"), MAX_NUM_LIGHTS, lightStrength);
	glUniform3fv(shaderIF->ppuLoc("globalAmbient"), 1, globalAmbient);
	glUniform4fv(shaderIF->ppuLoc("ecLightPosition"), MAX_NUM_LIGHTS, ecLightPosition);
}

void SceneElement::establishMaterial(const PhongMaterial& matl)
{
	glUniform3fv(shaderIF->ppuLoc("kd"), 1, matl.kd);
	glUniform3fv(shaderIF->ppuLoc("ka"), 1, matl.ka);
	glUniform3fv(shaderIF->ppuLoc("ks"), 1, matl.ks);
	glUniform1f(shaderIF->ppuLoc("m"), matl.shininess);
	glUniform1f(shaderIF->ppuLoc("alpha"), matl.alpha);
}

// NOTE: You may want to modify the interface to this method so that you
//       can pass parameters to help you specify some of the parameters
//       mentioned in the comments that follow.
void SceneElement::establishTexture(GLuint texID)
{
	// Set texture-related parameters:
	// 1. OpenGL ones including glActiveTexture, glBindTexture, glTexParameter*
	// 2. Parameters communicated via per-primitive uniform variables defined in
	//    your shader program that allow you to control whether a texture is to
	//    to be used, and, if so, where the texture comes from (e.g., procedural,
	//    a sampler2D, etc.) and how to combine its color with with the color
	//    computed by your Phong local lighting model color.
	//
	// (If you are texture-mapping onto faces of BasicShape instances,
	// see that documentation for specific additional details.)
	int texture = 0;
	glActiveTexture(GL_TEXTURE0 + texture);

	glUniform1i(shaderIF->ppuLoc("textureMap"), texture);
	glUniform1i(shaderIF->ppuLoc("usingTexture"), 1);

	glBindTexture(GL_TEXTURE_2D, texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
}

void SceneElement::establishView()
{
	// Line of sight, dynamic view controls, 3D-2D projection, & mapping to LDS:
	cryph::Matrix4x4 mc_ec, ec_lds;
	ModelView::getMatrices(mc_ec, ec_lds);
	float m[16];
	glUniformMatrix4fv(shaderIF->ppuLoc("mc_ec"), 1, false, mc_ec.extractColMajor(m));
	glUniformMatrix4fv(shaderIF->ppuLoc("ec_lds"), 1, false, ec_lds.extractColMajor(m));

	ExtendedController* ec = dynamic_cast<ExtendedController*>(Controller::getCurrentController());
	glUniform1i(shaderIF->ppuLoc("sceneHasTranslucentObjects"), 1);
	if (ec->drawingOpaque())
	    glUniform1i(shaderIF->ppuLoc("drawingOpaqueObjects"), 1);
	else
	    glUniform1i(shaderIF->ppuLoc("drawingOpaqueObjects"), 0);
}

GLuint SceneElement::readTextureImage(const std::string& imgFileName)
{
	OpenGLImageReader* oglIR = OpenGLImageReader::create(imgFileName.c_str());
	if (oglIR == nullptr)
	{
		std::cerr << "Could not open '" << imgFileName << "' for texture map.\n";
		return 0;
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	GLuint texID[1];
	glGenTextures(1, texID);
	glBindTexture(GL_TEXTURE_2D, texID[0]);
	float white[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, white);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	GLint level = 0;
	int pw = oglIR->getWidth(), ph = oglIR->getHeight();
	GLint iFormat = oglIR->getInternalFormat();
	GLenum format = oglIR->getFormat();
	GLenum type = oglIR->getType();
	const GLint border = 0; // must be zero (only present for backwards compatibility)
	const void* pixelData = oglIR->getTexture();
	glTexImage2D(GL_TEXTURE_2D, level, iFormat, pw, ph, border, format, type, pixelData);
	delete oglIR;
	return texID[0];
}
