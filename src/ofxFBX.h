/*
 * ofxFBX.h
 *
 *  Created on: Sep 11, 2012
 *      Author: arturo
 */

#ifndef OFXFBX_H_
#define OFXFBX_H_

#define FBXSDK_NEW_API
#include <fbxsdk.h>
#include "ofxFBXScene.h"
#include "ofxFBXNode.h"
#include "ofxFBXCamera.h"
#include "ofxFBXMesh.h"

inline ofVec3f ofxFBXNodePosition(FbxNode* pNode){
	return ofVec3f(pNode->LclTranslation.Get().mData[0],pNode->LclTranslation.Get().mData[1],pNode->LclTranslation.Get().mData[2]);
}


inline ofQuaternion ofxFBXNodeRotation(FbxNode* pNode){
	return ofQuaternion( pNode->LclRotation.Get().mData[0], ofVec3f(1, 0, 0), pNode->LclRotation.Get().mData[1], ofVec3f(0, 1, 0), pNode->LclRotation.Get().mData[2], ofVec3f(0, 0, 1));
}

inline ofVec4f toOf(const FbxVector4 & vec4){
	return ofVec4f(vec4[0],vec4[1],vec4[2],vec4[3]);
}

inline FbxVector4 toFbx(const ofVec4f & vec4){
	return FbxVector4(vec4.x,vec4.y,vec4.z,vec4.w);
}

inline ofMatrix4x4 toOf(const FbxAMatrix & matrix){
	ofMatrix4x4 m;
	for(int i=0;i<4;i++){
		m._mat[i] = toOf(matrix.GetRow(i));
	}
	return m;
}

#endif /* OFXFBX_H_ */
