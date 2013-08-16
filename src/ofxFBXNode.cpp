/*
 * ofxFBXCamera.cpp
 *
 *  Created on: Sep 10, 2012
 *      Author: arturo
 */


#include "ofxFBXNode.h"


string ofxFBXNode::getName(){
	return nodeName;
}

float getValueAt(std::vector<ofxFBXKey<float> >& keys,float defaultValue,u_long ms){
	for(u_int i=0;i<keys.size();i++){
		if(keys[i].timeMillis==ms){
			return keys[i].value;
		}else if(keys[i].timeMillis>ms){
			if(i>0){
				u_long delta = ms - keys[i-1].timeMillis;
				float pct = double(delta) / double(keys[i].timeMillis - keys[i-1].timeMillis);
				return  ofLerp(keys[i-1].value,keys[i].value,pct);
			}else{
				u_long delta = ms;
				float pct = double(delta) / double(keys[i].timeMillis);
				return ofLerp(defaultValue,keys[i].value,pct);
			}
		}
	}
	if(keys.empty()){
		return defaultValue;
	}else{
		return keys.back().value;
	}
}

ofVec3f ofxFBXNode::getPositionAt(u_long ms){
	ofVec3f position;
	position.x = getValueAt(xKeys,originalPosition.x,ms);
	position.y = getValueAt(yKeys,originalPosition.y,ms);
	position.z = getValueAt(zKeys,originalPosition.z,ms);
	return position;
}


ofVec3f ofxFBXNode::getPositionAtFrame(u_int frame){
	ofVec3f position;
	if(xKeys.empty()){
		position.x = originalPosition.x;
	}else{
		position.x = xKeys[MIN(frame,xKeys.size()-1)].value;
	}
	if(yKeys.empty()){
		position.y = originalPosition.y;
	}else{
		position.y = yKeys[MIN(frame,yKeys.size()-1)].value;
	}
	if(zKeys.empty()){
		position.z = originalPosition.z;
	}else{
		position.z = zKeys[MIN(frame,zKeys.size()-1)].value;
	}
	return position;
}

ofVec3f ofxFBXNode::getScaleAt(u_long ms){
	ofVec3f scale;
	scale.x = getValueAt(xScaleKeys,originalScale.x,ms);
	scale.y = getValueAt(yScaleKeys,originalScale.y,ms);
	scale.z = getValueAt(zScaleKeys,originalScale.z,ms);
	return scale;
}

ofVec3f ofxFBXNode::getScaleAtFrame(u_int frame){
	ofVec3f scale;
	if(xScaleKeys.empty()){
		scale.x = originalScale.x;
	}else{
		scale.x = xScaleKeys[MIN(frame,xScaleKeys.size()-1)].value;
	}
	if(yScaleKeys.empty()){
		scale.y = originalScale.y;
	}else{
		scale.y = yScaleKeys[MIN(frame,yScaleKeys.size()-1)].value;
	}
	if(zScaleKeys.empty()){
		scale.z = originalScale.z;
	}else{
		scale.z = zScaleKeys[MIN(frame,zScaleKeys.size()-1)].value;
	}
	return scale;
}

ofQuaternion ofxFBXNode::getRotationAt(u_long ms){
	for(u_int i=0;i<rotationKeys.size();i++){
		if(rotationKeys[i].timeMillis==ms){
			return rotationKeys[i].value;
		}else if(rotationKeys[i].timeMillis>ms){
			if(i>0){
				u_long delta = ms - rotationKeys[i-1].timeMillis;
				float pct = double(delta) / double(rotationKeys[i].timeMillis - rotationKeys[i-1].timeMillis);
				ofQuaternion q;
				q.slerp(pct,rotationKeys[i-1].value,rotationKeys[i].value);
				return q;
			}else{
				u_long delta = ms;
				float pct = double(delta) / double(rotationKeys[i].timeMillis);
				ofQuaternion q;
				q.slerp(pct,originalRotation,rotationKeys[i].value);
				return q;
			}
		}
	}
	if(rotationKeys.empty()){
		return originalRotation;
	}else{
		return rotationKeys.back().value;
	}
}

ofQuaternion ofxFBXNode::getRotationAtFrame(u_int frame){
	if(rotationKeys.empty()){
		return originalRotation;
	}else{
		return rotationKeys[MIN(frame,rotationKeys.size()-1)].value;
	}
}

void ofxFBXNode::setTime(u_long ms){
	node.setOrientation(getRotationAt(ms));
	node.setPosition(getPositionAt(ms));
	node.setScale(getScaleAt(ms));
	//node.setScale(-1,-1,1);
}

void ofxFBXNode::setFrame(u_int frame){
	node.setOrientation(getRotationAtFrame(frame));
	node.setPosition(getPositionAtFrame(frame));
	node.setScale(getScaleAtFrame(frame));
	//node.setScale(-1,-1,1);
}

ofNode & ofxFBXNode::getNode(){
	return node;
}
