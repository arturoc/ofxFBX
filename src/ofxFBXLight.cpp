/*
 * ofxFBXLight.cpp
 *
 *  Created on: May 30, 2013
 *      Author: arturo
 */

#include "ofxFBXLight.h"

const GLfloat DEFAULT_LIGHT_POSITION[] = {0.0f, 0.0f, 0.0f, 1.0f};
const GLfloat DEFAULT_DIRECTION_LIGHT_POSITION[] = {0.0f, 0.0f, 1.0f, 0.0f};
const GLfloat DEFAULT_SPOT_LIGHT_DIRECTION[] = {0.0f, 0.0f, -1.0f};
const GLfloat DEFAULT_LIGHT_COLOR[] = {1.0f, 1.0f, 1.0f, 1.0f};
const GLfloat DEFAULT_LIGHT_SPOT_CUTOFF = 180.0f;

ofxFBXLight::ofxFBXLight()
:fbxLight(NULL)
{
	// TODO Auto-generated constructor stub

}

ofxFBXLight::~ofxFBXLight() {
	// TODO Auto-generated destructor stub
}

void ofxFBXLight::initLighting(){
	glLightfv(GL_LIGHT0, GL_POSITION, DEFAULT_DIRECTION_LIGHT_POSITION);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, DEFAULT_LIGHT_COLOR);
	glLightfv(GL_LIGHT0, GL_SPECULAR, DEFAULT_LIGHT_COLOR);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, DEFAULT_LIGHT_SPOT_CUTOFF);
	glEnable(GL_LIGHT0);

	// Set ambient light.
	GLfloat lAmbientLight[] = {static_cast<GLfloat>(1), static_cast<GLfloat>(1),
		static_cast<GLfloat>(1), 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lAmbientLight);
}

void ofxFBXLight::update(){
	float lConeAngle = 0;

	FbxLight::EType mType = fbxLight->LightType.Get();
	if (mType == FbxLight::eSpot)
	{
		FbxPropertyT<FbxDouble> lConeAngleProperty = fbxLight->InnerAngle;
		lConeAngle = static_cast<GLfloat>(lConeAngleProperty.Get());
	}

	getNode().transformGL();
	const GLfloat lLightColor[4] = {fbxLight->Color.Get().mData[0],fbxLight->Color.Get().mData[1],fbxLight->Color.Get().mData[2], 1.0f};
	//const GLfloat lConeAngle = mConeAngle.Get(pTime);

	glColor3fv(lLightColor);

	/*glPushAttrib(GL_ENABLE_BIT);
    glPushAttrib(GL_POLYGON_BIT);
    // Visible for double side.
    glDisable(GL_CULL_FACE);
    // Draw wire-frame geometry.
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (mType == FbxLight::eSpot)
	{
		// Draw a cone for spot light.
		glPushMatrix();
		glScalef(1.0f, 1.0f, -1.0f);
		const double lRadians = ofDegToRad( lConeAngle );
		const double lHeight = 15.0;
		const double lBase = lHeight * tan(lRadians / 2);
		GLUquadricObj * lQuadObj = gluNewQuadric();
		gluCylinder(lQuadObj, 0.0, lBase, lHeight, 18, 1);
		gluDeleteQuadric(lQuadObj);
		glPopMatrix();
	}
	else
	{
		// Draw a sphere for other types.
		GLUquadricObj * lQuadObj = gluNewQuadric();
		gluSphere(lQuadObj, 1.0, 10, 10);
		gluDeleteQuadric(lQuadObj);
	}
    glPopAttrib();
    glPopAttrib();*/


	// The transform have been set, so set in local coordinate.
	if (mType == FbxLight::eDirectional)
	{
		glLightfv(GL_LIGHT0, GL_POSITION, DEFAULT_DIRECTION_LIGHT_POSITION);
	}
	else
	{
		glLightfv(GL_LIGHT0, GL_POSITION, DEFAULT_LIGHT_POSITION);
	}

	glLightfv(GL_LIGHT0, GL_DIFFUSE, lLightColor);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lLightColor);

	if (mType == FbxLight::eSpot ) //&& lConeAngle != 0.0
	{
		glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, DEFAULT_SPOT_LIGHT_DIRECTION);

		// If the cone angle is 0, equal to a point light.
		/*if (lConeAngle != 0.0f)
		{
			// OpenGL use cut off angle, which is half of the cone angle.
			glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, lConeAngle/2);
		}*/
	}
	glEnable(GL_LIGHT0);
	getNode().restoreTransformGL();
}

void ofxFBXLight::setFBXLight(FbxLight* light){
	fbxLight = light;
}
