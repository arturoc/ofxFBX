#pragma once

#define FBXSDK_NEW_API
#include <fbxsdk.h>
#include "ofxFBXCamera.h"
#include "ofxFBXMesh.h"
#include "ofxFBXLight.h"
#include <string>
#include <vector>
#include <list>

class ofxFBXScene{
public:
	bool load(std::string path);
	std::vector<ofxFBXCamera*> & getCameras();
	std::vector<ofxFBXMesh*> & getMeshes();
	std::vector<ofxFBXNode*> & getNulls();
	std::vector<ofxFBXLight*> & getLights();

	void setTime(u_long ms);
	void setFrame(u_int frame);

private:
	void parseScene(FbxNode* pNode, FbxAnimLayer * pAnimLayer, ofxFBXNode * parentNode);
	ofxFBXNode * parseMeshInfo(FbxNode* pNode, FbxAnimLayer * pAnimLayer);
	ofxFBXNode * parseCameraInfo(FbxNode* pNode, FbxAnimLayer * pAnimLayer);
	ofxFBXNode * parseLightInfo(FbxNode* pNode, FbxAnimLayer * pAnimLayer);
	void parsePositionCurve(ofxFBXNode & node, FbxAnimLayer * pAnimLayer, FbxPropertyT<FbxDouble3> &position);
	void parseScaleCurve(ofxFBXNode & node, FbxAnimLayer * pAnimLayer, FbxPropertyT<FbxDouble3> &scale);
	void parseRotationCurve(ofxFBXNode & node, FbxAnimLayer * pAnimLayer, FbxNode* fbxNode, FbxPropertyT<FbxDouble3> &rotation);
	std::list<ofxFBXCamera> camerasList;
	std::list<ofxFBXMesh> meshesList;
	std::list<ofxFBXNode> nullsList;
	std::list<ofxFBXLight> lightsList;
	std::vector<ofxFBXCamera*> cameras;
	std::vector<ofxFBXMesh*> meshes;
	std::vector<ofxFBXNode*> nulls;
	std::vector<ofxFBXLight*> lights;
	FbxManager* lSdkManager;
	FbxScene* lScene;
};
