/*
 * ofxFBXNode.h
 *
 *  Created on: Sep 10, 2012
 *      Author: arturo
 */

#ifndef OFXFBXNODE_H_
#define OFXFBXNODE_H_

#include "ofNode.h"

template<class T>
struct ofxFBXKey{
	T value;
	u_long timeMillis;
};

class ofxFBXNode{
public:
	virtual ~ofxFBXNode(){}
	string getName();

	ofVec3f getPositionAt(u_long ms);
	ofVec3f getPositionAtFrame(u_int frame);

	ofVec3f getScaleAt(u_long ms);
	ofVec3f getScaleAtFrame(u_int frame);

	ofQuaternion getRotationAt(u_long ms);
	ofQuaternion getRotationAtFrame(u_int frame);

	virtual void setTime(u_long ms);
	virtual void setFrame(u_int frame);

	ofNode & getNode();
private:
	friend class ofxFBXScene;
	vector<ofxFBXKey<float> > xKeys,yKeys,zKeys;
	vector<ofxFBXKey<float> > xScaleKeys,yScaleKeys,zScaleKeys;
	vector<ofxFBXKey<ofQuaternion> > rotationKeys;
	ofVec3f originalPosition;
	ofQuaternion originalRotation;
	ofVec3f originalScale;
	ofNode node;
protected:
	string nodeName;
	ofVec3f target;
};

#endif /* OFXFBXNODE_H_ */
