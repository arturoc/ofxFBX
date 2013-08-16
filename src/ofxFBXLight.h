/*
 * ofxFBXLight.h
 *
 *  Created on: May 30, 2013
 *      Author: arturo
 */

#ifndef OFXFBXLIGHT_H_
#define OFXFBXLIGHT_H_

#define FBXSDK_NEW_API
#include <fbxsdk.h>

#include "ofxFBXNode.h"
#include "ofLight.h"

class ofxFBXLight: public ofxFBXNode {
public:
	ofxFBXLight();
	virtual ~ofxFBXLight();

	static void initLighting();

	void update();

	void setFBXLight(FbxLight* light);

private:
	FbxLight* fbxLight;
	ofColor color;
};

#endif /* OFXFBXLIGHT_H_ */
