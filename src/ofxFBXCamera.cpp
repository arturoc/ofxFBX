
#include "ofxFBX.h"
#include "ofxFBXCamera.h"

void ofxFBXCamera::updateLookAt(){
	float  length = (target - getGlobalPosition()).length();

	ofVec3f rotationVector(1.0,0,0);
	ofVec3f center = getGlobalOrientation() * rotationVector;
	center *= length;
	center += getPosition();


	rotationVector.set(0,1.0,0);
    ofVec3f up = getGlobalOrientation() * rotationVector;

    ofVec3f forward = center - getGlobalPosition();
    forward.normalize();
    ofVec3f right = up.cross(forward);
    right.normalize();
    up = forward.cross(right);
    up.normalize();

	lookAt(center,up);
}

void ofxFBXCamera::setTime(u_long ms){
	ofxFBXNode::setTime(ms);
	((ofNode&)*this) = getNode();

	updateLookAt();
}


void ofxFBXCamera::setFrame(u_int frame){
	ofxFBXNode::setFrame(frame);
	((ofNode&)*this) = getNode();

	updateLookAt();
}
