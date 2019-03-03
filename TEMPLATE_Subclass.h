// TEMPLATE_Subclass.h

#ifndef TEMPLATE_SUBCLASS_H
#define TEMPLATE_SUBCLASS_H

#include "SceneElement.h"
#include "ShaderIF.h"

class TEMPLATE_Subclass : public SceneElement
{
public:
	TEMPLATE_Subclass(ShaderIF* sIF, PhongMaterial& matlIn);
	virtual ~TEMPLATE_Subclass();

	// xyzLimits: {mcXmin, mcXmax, mcYmin, mcYmax, mcZmin, mcZmax}
	void getMCBoundingBox(double* xyzLimitsF) const;
	void render();
private:
	// IMPORTANT NOTE:
	// The ShaderIF will be stored with the SceneElement piece of
	// this object instance. You only need add instance variables here
	// that are unique to the new subclass you are creating.
	PhongMaterial matl;
};

#endif
