/*
 * ofxFBXMesh.h
 *
 *  Created on: Sep 10, 2012
 *      Author: arturo
 */

#ifndef OFXFBXMESH_H_
#define OFXFBXMESH_H_


#define FBXSDK_NEW_API
#include <fbxsdk.h>

#include "ofVboMesh.h"
#include "ofxFBXNode.h"
#include "ofParameter.h"
#include "ofParameterGroup.h"
#include <map>

class ofxFBXMesh: public ofxFBXNode {
public:
	ofxFBXMesh();
	virtual ~ofxFBXMesh();

	void setFBXMesh(FbxMesh* lMesh);

	int getNumBlendShapes();

	void draw();
	void drawWireframe();
	ofMesh & getMesh();

	ofParameterGroup & getBlendParameters();

private:

	void blendChanged(const void * sender, float & pct);
	void updateBlend();
	map<string,float> blendPct;
	map<string,ofMesh> blendShapes;
	ofVboMesh mesh;
	ofMesh original;
	FbxMesh* fbxMesh;
	ofParameterGroup blendParameters;
};

#endif /* OFXFBXMESH_H_ */
