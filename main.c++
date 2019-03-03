// main.c++: Starter for EECS 672 Projects 2-4

#include "ExtendedController.h"
#include "TEMPLATE_Subclass.h"
#include <fstream>
#include <string>
#include <iostream>
#include <exception>
#include <unordered_map>
#include "Floor.h"
#include "Robot.h"
#include "Antenna.h"
#include "Lantern.h"
using namespace std;
unordered_map<std::string, PhongMaterial> materials {
	{"Gold", PhongMaterial(.24725, .1994, .0745, 0.75164, 0.60648, 0.22648, 0.628281, 0.555802, 0.366065, 51.2, 1) },
	{"Silver", PhongMaterial(.19225, .19225, .19225, 0.50754, 0.50754, 0.50754, 0.508273, 0.508273, 0.508273, 51.2, 1) },
	{"Clay", PhongMaterial(.55, .3, 0, .1, .1, .1, 0, 0, 0, .01, 1) },
	{"Plastic", PhongMaterial(0, 0, 0, .01, .01, .01, .5, .5, .5, 1, 1) },
	{"Chrome", PhongMaterial(.25, .25, .25, .4, .4, .4, 0.774597, 0.774597, 0.774597, 76.8, 1)},
	{"Glass", PhongMaterial(0, 0, 0, .5,.5,.5, 1, 1, 1, 100, .3)}
};

void setLight(float* position, int* number)
{
	int x = *number;
	float mcLightStrength = 1;
	for(int i = 0; i < MAX_NUM_LIGHTS; i++)
	{
		SceneElement::lightPos[x + i] = position[i];
		SceneElement::lightStrength[x + i - x / 4] = mcLightStrength;
	}
	SceneElement::posInModelCoordinates[x / 4] = true;
	SceneElement::lightPos[x + 3] = 1;
	*number+=4;
}

void CreateModel(ExtendedController& c, ShaderIF* sIF, string object, string matl, float position[3], int* number)
{
	PhongMaterial material = materials.at(matl);

	if(object == "Robot")
	{
		c.addModel(new Robot(sIF, position, material));
		return;
	}
	if(object == "Antenna")
	{
		c.addModel(new Antenna(sIF, position, material));
		return;
	}
	if(object == "Lantern")
	{
		Lantern* lantern = new Lantern(sIF, position, material);
		c.addModel(new Lantern(sIF, position, material));
		float* p = new float[3];
		lantern->GetLightPosition(p);
		setLight(p, number);
		return;
	}
}

void createScene(ExtendedController& c, ShaderIF* sIF)
{
//ADD EC lightPos
SceneElement::lightPos[0] = SceneElement::lightPos[1] = SceneElement::lightPos[2] = 0;
SceneElement::lightPos[3] = 0;
SceneElement::lightStrength[0] = SceneElement::lightStrength[1] = SceneElement::lightStrength[2] = 1;
SceneElement::posInModelCoordinates[0] = false;
//

	//ADD Floor
	float origin[3] = {0, -100, 0}, length[3] = {300, 100, 300};
	c.addModel(new Floor(sIF, origin, length, materials.at("Clay")));
	/*	SCENE INPUT FORMAT
	*	[OBJECT] [MATERIAL] [POSITION]
	* [POSITION]-> x y z
	*/
	string path = "scene.txt";
	ifstream inFile(path);
	if(!inFile.is_open())
	{
		cout<<"Create a scene file."<<endl;
		return;
	}

	try{
		//STATE: 0 = object, 1 = material, 2 = position
		int state = 0;
		int number = 4;
		string input;
		string object, material;
		while(inFile >> input)
		{
			if(state == 0)
				object = input;
			else if (state == 1)
				material = input;
			else if(state == 2)
			{
				 float position[3];
				 position[0] = stoi(input);
				 inFile>> position[1];
				 inFile >> position[2];

				 CreateModel(c, sIF, object, material, position,&number);
			}
			state = (state+1)%3;
		}
	}
	catch(exception& e)
	{
		cout<<"Error parsing file"<<endl;
	}
	inFile.close();
}

void set3DViewingInformation(double overallBB[])
{
	ModelView::setMCRegionOfInterest(overallBB);
	double a = 3.5, r = 0, d;
	for(int i = 0; i < 3; i++)
	{
		double t = overallBB[i + 1] - overallBB[i];
		if(t > r)
			r = t;
	}
	d = a * r;

	double mid[3] = {
		(overallBB[0] + overallBB[1]) * .5,
		(overallBB[2] + overallBB[3]) * .5,
		(overallBB[4] + overallBB[5]) * .5};

	cryph::AffPoint eye, center(mid[0], mid[1], mid[2]), dir(0, 0, -1);
	eye = center + d*dir;
	cryph::AffVector up(0,1,0);
	ModelView::setEyeCenterUp(eye, center, up);
	// EC -> LDS:
	ModelView::setProjection(PERSPECTIVE);
	double ecZmin = -d - r, ecZmax = -d + r;
	ModelView::setECZminZmax(ecZmin, ecZmax);
	double ecZpp = ecZmax;
	ModelView::setProjectionPlaneZ(ecZpp);
}

int main(int argc, char* argv[])
{
	//make robot eyes light up
	ExtendedController c("Robot world", MVC_USE_DEPTH_BIT);
	c.reportVersions(std::cout);

	ShaderIF::ShaderSpec glslProg[] =
	{
	    { "shaders/basic.vsh", GL_VERTEX_SHADER },
			{ "shaders/phong.fsh", GL_FRAGMENT_SHADER },
	    { "shaders/geo.gsh", GL_GEOMETRY_SHADER }
	};

	ShaderIF* sIF = new ShaderIF(glslProg, 3);

	createScene(c, sIF);

	glClearColor(.1, .1, .1, 1.0);

	double xyz[6];
	c.getOverallMCBoundingBox(xyz);
	std::cout << "Bounding box: " << xyz[0] << " <= x <= " << xyz[1] << '\n';
	std::cout << "              " << xyz[2] << " <= y <= " << xyz[3] << '\n';
	std::cout << "              " << xyz[4] << " <= z <= " << xyz[5] << "\n\n";
	set3DViewingInformation(xyz);

	c.run();

	delete sIF;

	return 0;
}
