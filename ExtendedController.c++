// ExtendedController.c++

#include "ExtendedController.h"
#include "ModelView.h"
#include<iostream>

ExtendedController::ExtendedController(const std::string& name, int rcFlags):
	GLFWController(name, rcFlags)
{
}

void ExtendedController::handleMouseMotion(int x, int y)
{
	int dx = x - screenBaseX;
	int dy = y - screenBaseY;
	screenBaseX = x;
	screenBaseY = y;

	if(mouseMotionIsRotate)
	{
		ModelView::addToGlobalRotationDegrees(dy/3.0, dx/3.0, 0.0);
	}
	else if(mouseMotionIsTranslate)
	{
		double ldsx, ldsy;
		screenXYToLDS(dx, dy, ldsx, ldsy);
		ldsx++; ldsy--;
		ModelView::addToGlobalPan(-ldsx, -ldsy, 0);
	}
}

void ExtendedController::handleDisplay()
{
    glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw opaque objects
    glDisable(GL_BLEND);
    drawingOpaqueObjects = true; // record in instance variable so ModelView instances can query
    renderAllModels();

    // draw translucent objects
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    drawingOpaqueObjects = false; // record in instance variable so ModelView instances can query
    renderAllModels();

    swapBuffers();
}

// The following must a public method in class ExtendedController:
bool ExtendedController::drawingOpaque() const // CALLED FROM SceneElement or descendant classes
{
    return drawingOpaqueObjects;
}
