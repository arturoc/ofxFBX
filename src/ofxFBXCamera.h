#pragma once

#include "ofConstants.h"
#include "ofVec3f.h"
#include "ofCamera.h"
#include "ofxFBXNode.h"


class ofxFBXCamera: public ofCamera, public ofxFBXNode{
public:
	void setTime(u_long ms);
	void setFrame(u_int frame);

private:
	void updateLookAt();
	friend class ofxFBXScene;
};
