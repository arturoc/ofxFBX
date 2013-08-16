/*
 * ofxFBXScene.cpp
 *
 *  Created on: Sep 10, 2012
 *      Author: arturo
 */



#include "ofxFBX.h"

#include "Common/Common.h"
#include "ofxFBXScene.h"

#include "ofConstants.h"
#include "ofUtils.h"
#define HFOV2VFOV(h, ar) (2.0 * atan((ar) * tan( (h * FBXSDK_PI_DIV_180) * 0.5)) * FBXSDK_180_DIV_PI) //ar : aspectY / aspectX
#define VFOV2HFOV(v, ar) (2.0 * atan((ar) * tan( (v * FBXSDK_PI_DIV_180) * 0.5)) * FBXSDK_180_DIV_PI) //ar : aspectX / aspectY


bool ofxFBXScene::load(string path){
	bool lResult;

	// Prepare the FBX SDK.
	InitializeSdkObjects(lSdkManager, lScene);

	lResult = LoadScene(lSdkManager, lScene, ofToDataPath(path).c_str());


	if(!lResult){
		ofLogError("ofxFBXScene") << "An error occurred while loading the scene...";
	}else if(lScene){
		for (int i = 0; i < lScene->GetSrcObjectCount<FbxAnimStack>(); i++)
		{
			FbxAnimStack* lAnimStack = FbxCast<FbxAnimStack>(lScene->GetSrcObject<FbxAnimStack>(i));

			int l;
			int nbAnimLayers = lAnimStack->GetMemberCount<FbxAnimLayer>();


			/*for (l = 0; l < nbAnimLayers; l++)
			{*/
			if(nbAnimLayers){
				FbxAnimLayer* lAnimLayer = lAnimStack->GetMember<FbxAnimLayer>(0);

				//get root node of the fbx scene
				FbxNode* lRootNode = lScene->GetRootNode();

				parseScene(lRootNode, lAnimLayer,NULL);
			}
		}
		list<ofxFBXCamera>::iterator itCameras;
		for(itCameras=camerasList.begin();itCameras!=camerasList.end();itCameras++){
			cameras.push_back(&(*itCameras));
		}
		list<ofxFBXMesh>::iterator itMeshes;
		for(itMeshes=meshesList.begin();itMeshes!=meshesList.end();itMeshes++){
			meshes.push_back(&(*itMeshes));
		}
		list<ofxFBXNode>::iterator itNulls;
		for(itNulls=nullsList.begin();itNulls!=nullsList.end();itNulls++){
			nulls.push_back(&(*itNulls));
		}
		list<ofxFBXLight>::iterator itLights;
		for(itLights=lightsList.begin();itLights!=lightsList.end();itLights++){
			lights.push_back(&(*itLights));
		}
	}else{
		ofLogError("ofxFBXScene") << "Null scene...";
		lResult = false;
	}

	//Destroy all objects created by the FBX SDK.
	//DestroySdkObjects(lSdkManager,false);
	return lResult;
}

std::vector<ofxFBXCamera*> & ofxFBXScene::getCameras(){
	return cameras;
}


std::vector<ofxFBXMesh*> & ofxFBXScene::getMeshes(){
	return meshes;
}


std::vector<ofxFBXNode*> & ofxFBXScene::getNulls(){
	return nulls;
}


std::vector<ofxFBXLight*> & ofxFBXScene::getLights(){
	return lights;
}

void ofxFBXScene::parseScene(FbxNode* pNode, FbxAnimLayer * pAnimLayer, ofxFBXNode * parentNode){
	ofLogVerbose("ofxFBXScene") << "found node " << pNode->GetName() << " of type " << pNode->GetTypeName() << endl;
    if( pNode->GetCamera() ){
    	ofxFBXNode * node = parseCameraInfo(pNode,pAnimLayer);
    	if(parentNode) node->getNode().setParent(parentNode->getNode());
    	parentNode = node;
    }else if(pNode->GetMesh()){
    	ofxFBXNode * node = parseMeshInfo(pNode, pAnimLayer);
    	if(parentNode) node->getNode().setParent(parentNode->getNode());
    	parentNode = node;
    }else if(pNode->GetLight()){
    	ofxFBXNode * node = parseLightInfo(pNode, pAnimLayer);
    	if(parentNode) node->getNode().setParent(parentNode->getNode());
    	parentNode = node;
    }else{
    	ofxFBXNode node;
    	node.nodeName = pNode->GetName();
		parsePositionCurve(node,pAnimLayer,pNode->LclTranslation);
		parseScaleCurve(node,pAnimLayer,pNode->LclScaling);
		parseRotationCurve(node,pAnimLayer,pNode,pNode->LclRotation);
		nullsList.push_back(node);
		if(parentNode) nullsList.back().getNode().setParent(parentNode->getNode());
    	parentNode = &nullsList.back();
    }

    //recursively traverse each node in the scene
    int i, lCount = pNode->GetChildCount();
    for (i = 0; i < lCount; i++)
    {
    	parseScene(pNode->GetChild(i), pAnimLayer, parentNode);
    }
}

ofxFBXNode * ofxFBXScene::parseMeshInfo(FbxNode* pNode, FbxAnimLayer * pAnimLayer){
	ofLogVerbose("ofxFBXScene") << "found mesh " << pNode->GetName() << " of type " << pNode->GetTypeName() << endl;
	FbxMesh* lMesh = pNode->GetMesh();
	meshesList.push_back(ofxFBXMesh());
	ofxFBXMesh & mesh = meshesList.back();

	mesh.setFBXMesh(lMesh);

	parsePositionCurve(mesh,pAnimLayer,pNode->LclTranslation);

	parseScaleCurve(mesh,pAnimLayer,pNode->LclScaling);

	parseRotationCurve(mesh,pAnimLayer,pNode,pNode->LclRotation);

	return &meshesList.back();
}

ofxFBXNode * ofxFBXScene::parseLightInfo(FbxNode* pNode, FbxAnimLayer * pAnimLayer){
	ofLogVerbose("ofxFBXScene") << "found light " << pNode->GetName() << " of type " << pNode->GetTypeName() << endl;
	FbxLight* lLight = pNode->GetLight();
	lightsList.push_back(ofxFBXLight());
	ofxFBXLight & light = lightsList.back();

	light.setFBXLight(lLight);

	/*if(lLight->CastLight){
		light.enable();
	}

	light.setAmbientColor(ofColor(lLight->Color.Get().mData[0],lLight->Color.Get().mData[1],lLight->Color.Get().mData[2]));
	//light.setAttenuation(lLight->DecayStart)
	switch(lLight->LightType){
		case FbxLight::ePoint:
			light.setPointLight();
			break;
		case FbxLight::eDirectional:
			light.setDirectional();
			break;
		case FbxLight::eSpot:
			light.setSpotlight();
			break;
		case FbxLight::eArea:
		case FbxLight::eVolume:
			ofLogError() << "area and volume lights not supported";
			break;
	}*/

	parsePositionCurve(light,pAnimLayer,pNode->LclTranslation);

	parseScaleCurve(light,pAnimLayer,pNode->LclScaling);

	parseRotationCurve(light,pAnimLayer,pNode,pNode->LclRotation);

	return &lightsList.back();
}

ofxFBXNode * ofxFBXScene::parseCameraInfo(FbxNode* pNode, FbxAnimLayer * pAnimLayer){
    FbxCamera* lCamera = pNode->GetCamera();

	ofxFBXCamera camera;

	camera.nodeName = lCamera->GetName();

	parsePositionCurve(camera,pAnimLayer,pNode->LclTranslation);

	parseScaleCurve(camera,pAnimLayer,pNode->LclScaling);
	//camera.originalScale.set(1,1,1);

	parseRotationCurve(camera,pAnimLayer,pNode,pNode->LclRotation);

	camera.setPosition(camera.originalPosition);

	if(pNode->GetTarget()){
		camera.target = ofxFBXNodePosition(pNode->GetTarget());
		//TODO: process lookAt animation
	}else{
		camera.target = toOf(lCamera->InterestPosition.Get());
	}


	float lNearPlane = lCamera->GetNearPlane();
	float lFarPlane = lCamera->GetFarPlane();

	//Get global scaling.
	FbxVector4 lCameraScaling = pNode->Scaling.Get();//GetGlobalPosition(pNode, 0).GetS();
	static const int  FORWARD_SCALE = 2;

	//scaling near plane and far plane
	//lNearPlane *= lCameraScaling[FORWARD_SCALE];
	//lFarPlane *= lCameraScaling[FORWARD_SCALE];


	FbxCamera::EAspectRatioMode lCamAspectRatioMode = lCamera->GetAspectRatioMode();
	double lAspectX = lCamera->AspectWidth.Get();
	double lAspectY = lCamera->AspectHeight.Get();
	double lAspectRatio = 1.333333;
	switch( lCamAspectRatioMode)
	{
	case FbxCamera::eWindowSize:
		lAspectRatio = lAspectX / lAspectY;
		break;
	case FbxCamera::eFixedRatio:
		lAspectRatio = lAspectX;

		break;
	case FbxCamera::eFixedResolution:
		lAspectRatio = lAspectX / lAspectY * lCamera->GetPixelRatio();
		break;
	case FbxCamera::eFixedWidth:
		lAspectRatio = lCamera->GetPixelRatio() / lAspectY;
		break;
	case FbxCamera::eFixedHeight:
		lAspectRatio = lCamera->GetPixelRatio() * lAspectX;
		break;
	default:
		break;

	}

	//get the aperture ratio
	double lFilmHeight = lCamera->GetApertureHeight();
	double lFilmWidth = lCamera->GetApertureWidth() * lCamera->GetSqueezeRatio();
	//here we use Height : Width
	double lApertureRatio = lFilmHeight / lFilmWidth;


	//change the aspect ratio to Height : Width
	lAspectRatio = 1 / lAspectRatio;
	//revise the aspect ratio and aperture ratio
	FbxCamera::EGateFit lCameraGateFit = lCamera->GateFit.Get();
	switch( lCameraGateFit )
	{

	case FbxCamera::eFitFill:
		if( lApertureRatio > lAspectRatio)  // the same as eHORIZONTAL_FIT
		{
			lFilmHeight = lFilmWidth * lAspectRatio;
			lCamera->SetApertureHeight( lFilmHeight);
			lApertureRatio = lFilmHeight / lFilmWidth;
		}
		else if( lApertureRatio < lAspectRatio) //the same as eVERTICAL_FIT
		{
			lFilmWidth = lFilmHeight / lAspectRatio;
			lCamera->SetApertureWidth( lFilmWidth);
			lApertureRatio = lFilmHeight / lFilmWidth;
		}
		break;
	case FbxCamera::eFitVertical:
		lFilmWidth = lFilmHeight / lAspectRatio;
		lCamera->SetApertureWidth( lFilmWidth);
		lApertureRatio = lFilmHeight / lFilmWidth;
		break;
	case FbxCamera::eFitHorizontal:
		lFilmHeight = lFilmWidth * lAspectRatio;
		lCamera->SetApertureHeight( lFilmHeight);
		lApertureRatio = lFilmHeight / lFilmWidth;
		break;
	case FbxCamera::eFitStretch:
		lAspectRatio = lApertureRatio;
		break;
	case FbxCamera::eFitOverscan:
		if( lFilmWidth > lFilmHeight)
		{
			lFilmHeight = lFilmWidth * lAspectRatio;
		}
		else
		{
			lFilmWidth = lFilmHeight / lAspectRatio;
		}
		lApertureRatio = lFilmHeight / lFilmWidth;
		break;
	case FbxCamera::eFitNone:
	default:
		break;
	}
	//change the aspect ratio to Width : Height
	lAspectRatio = 1 / lAspectRatio;

	double lFieldOfViewX = 0.0;
	double lFieldOfViewY = 0.0;
	if ( lCamera->GetApertureMode() == FbxCamera::eVertical)
	{
		lFieldOfViewY = lCamera->FieldOfView.Get();
		lFieldOfViewX = VFOV2HFOV( lFieldOfViewY, 1 / lApertureRatio);
	}
	else if (lCamera->GetApertureMode() == FbxCamera::eHorizontal)
	{
		lFieldOfViewX = lCamera->FieldOfView.Get(); //get HFOV
		lFieldOfViewY = HFOV2VFOV( lFieldOfViewX, lApertureRatio);
	}
	else if (lCamera->GetApertureMode() == FbxCamera::eFocalLength)
	{
		lFieldOfViewX = lCamera->ComputeFieldOfView(lCamera->FocalLength.Get());    //get HFOV
		lFieldOfViewY = HFOV2VFOV( lFieldOfViewX, lApertureRatio);
	}
	else if (lCamera->GetApertureMode() == FbxCamera::eHorizAndVert) {
		lFieldOfViewX = lCamera->FieldOfViewX.Get();
		lFieldOfViewY = lCamera->FieldOfViewY.Get();
	}


	//revise the Perspective since we have film offset
	double lFilmOffsetX = lCamera->FilmOffsetX.Get();
	double lFilmOffsetY = lCamera->FilmOffsetY.Get();
	lFilmOffsetX = 0 - lFilmOffsetX / lFilmWidth * 2.0;
	lFilmOffsetY = 0 - lFilmOffsetY / lFilmHeight * 2.0;

	camera.setFov(lFieldOfViewY);
	camera.setNearClip(lNearPlane);
	camera.setFarClip(lFarPlane);


	camerasList.push_back(camera);
	return &camerasList.back();


}


void ofxFBXScene::parsePositionCurve(ofxFBXNode & node, FbxAnimLayer * pAnimLayer, FbxPropertyT<FbxDouble3> &position){
	node.originalPosition = toOf(position.Get());
	node.getNode().setPosition(node.originalPosition);
	ofLogVerbose("ofxFBXScene") << "original position " << node.originalPosition << endl;


	if(!position.GetCurve(pAnimLayer)) return;
	FbxAnimCurve* lAnimCurveX = position.GetCurve(pAnimLayer,"X");
	FbxAnimCurve* lAnimCurveY = position.GetCurve(pAnimLayer,"Y");
	FbxAnimCurve* lAnimCurveZ = position.GetCurve(pAnimLayer,"Z");

    FbxTime   lKeyTime;
    int     lCount;

    int xKeyCount = lAnimCurveX? lAnimCurveX->KeyGetCount() : 0;
    int yKeyCount = lAnimCurveY? lAnimCurveY->KeyGetCount() : 0;
    int zKeyCount = lAnimCurveZ? lAnimCurveZ->KeyGetCount() : 0;

    ofxFBXKey<float> key;
    for(lCount = 0; lCount < xKeyCount; lCount++)
    {
    	key.value = lAnimCurveX->KeyGetValue(lCount);
        lKeyTime  = lAnimCurveX->KeyGetTime(lCount);
        key.timeMillis = lKeyTime.GetMilliSeconds();
        node.xKeys.push_back(key);
    }
    for(lCount = 0; lCount < yKeyCount; lCount++)
    {
    	key.value = lAnimCurveY->KeyGetValue(lCount);
        lKeyTime  = lAnimCurveY->KeyGetTime(lCount);
        key.timeMillis = lKeyTime.GetMilliSeconds();
        node.yKeys.push_back(key);
    }
    for(lCount = 0; lCount < zKeyCount; lCount++)
    {
    	key.value = lAnimCurveZ->KeyGetValue(lCount);
        lKeyTime  = lAnimCurveZ->KeyGetTime(lCount);
        key.timeMillis = lKeyTime.GetMilliSeconds();
        node.zKeys.push_back(key);
    }
}

void ofxFBXScene::parseScaleCurve(ofxFBXNode & node, FbxAnimLayer * pAnimLayer, FbxPropertyT<FbxDouble3> &scale){
	node.originalScale = toOf(scale.Get());
	node.getNode().setScale(node.originalScale);
	ofLogVerbose("ofxFBXScene") << "original scale " << node.originalScale << endl;

	if(!scale.GetCurve(pAnimLayer)) return;
	FbxAnimCurve* lAnimCurveX = scale.GetCurve(pAnimLayer,"X");
	FbxAnimCurve* lAnimCurveY = scale.GetCurve(pAnimLayer,"Y");
	FbxAnimCurve* lAnimCurveZ = scale.GetCurve(pAnimLayer,"Z");

    FbxTime   lKeyTime;
    int     lCount;

    int xKeyCount = lAnimCurveX? lAnimCurveX->KeyGetCount() : 0;
    int yKeyCount = lAnimCurveY? lAnimCurveY->KeyGetCount() : 0;
    int zKeyCount = lAnimCurveZ? lAnimCurveZ->KeyGetCount() : 0;

    ofxFBXKey<float> key;
    for(lCount = 0; lCount < xKeyCount; lCount++)
    {
    	key.value = lAnimCurveX->KeyGetValue(lCount);
        lKeyTime  = lAnimCurveX->KeyGetTime(lCount);
        key.timeMillis = lKeyTime.GetMilliSeconds();
        node.xScaleKeys.push_back(key);
    }
    for(lCount = 0; lCount < yKeyCount; lCount++)
    {
    	key.value = lAnimCurveY->KeyGetValue(lCount);
        lKeyTime  = lAnimCurveY->KeyGetTime(lCount);
        key.timeMillis = lKeyTime.GetMilliSeconds();
        node.yScaleKeys.push_back(key);
    }
    for(lCount = 0; lCount < zKeyCount; lCount++)
    {
    	key.value = lAnimCurveZ->KeyGetValue(lCount);
        lKeyTime  = lAnimCurveZ->KeyGetTime(lCount);
        key.timeMillis = lKeyTime.GetMilliSeconds();
        node.zScaleKeys.push_back(key);
    }
}

void ofxFBXScene::parseRotationCurve(ofxFBXNode & node, FbxAnimLayer * pAnimLayer, FbxNode* fbxNode, FbxPropertyT<FbxDouble3> &rotation){
	node.originalRotation = ofQuaternion(rotation.Get().mData[0], ofVec3f(1, 0, 0), rotation.Get().mData[1], ofVec3f(0, 1, 0), rotation.Get().mData[2], ofVec3f(0, 0, 1));
	node.getNode().setOrientation(node.originalRotation);
	ofLogVerbose("ofxFBXScene") << "original rotation " << endl << node.originalRotation << endl;

	if(!rotation.GetCurve(pAnimLayer)) return;
	FbxAnimCurve* lAnimCurveX = rotation.GetCurve(pAnimLayer,"X");
	FbxAnimCurve* lAnimCurveY = rotation.GetCurve(pAnimLayer,"Y");
	FbxAnimCurve* lAnimCurveZ = rotation.GetCurve(pAnimLayer,"Z");


    int xKeyCount = lAnimCurveX ? lAnimCurveX->KeyGetCount() : 0;
    int yKeyCount = lAnimCurveY ? lAnimCurveY->KeyGetCount() : 0;
    int zKeyCount = lAnimCurveZ ? lAnimCurveZ->KeyGetCount() : 0;

	FbxTime   lKeyTime;
	int     lCount;
	FbxTime lXKeyTime,lYKeyTime,lZKeyTime;
	for(lCount = 0; lCount < max(max(xKeyCount,yKeyCount),zKeyCount); lCount++)
	{
		if(lCount<xKeyCount){
			lXKeyTime  = lAnimCurveX->KeyGetTime(lCount);
		}
		if(lCount<yKeyCount){
			lYKeyTime  = lAnimCurveY->KeyGetTime(lCount);
		}
		if(lCount<zKeyCount){
			lZKeyTime  = lAnimCurveZ->KeyGetTime(lCount);
		}
		lKeyTime = min(min(lXKeyTime,lYKeyTime),lZKeyTime);
		lKeyTime = lXKeyTime;

		FbxAMatrix & matrix = fbxNode->EvaluateLocalTransform(lKeyTime);
		ofxFBXKey<ofQuaternion> key;
		ofVec3f t,s;
		ofQuaternion so;
		ofMatrix4x4 m = toOf(matrix);
		m.decompose(t,key.value,s,so);
		key.timeMillis = lKeyTime.GetMilliSeconds();
		node.rotationKeys.push_back(key);
	}
}

void ofxFBXScene::setTime(u_long ms){
	for(u_int i=0;i<cameras.size();i++){
		cameras[i]->setTime(ms);
	}
	for(u_int i=0;i<meshes.size();i++){
		meshes[i]->setTime(ms);
	}
	for(u_int i=0;i<nulls.size();i++){
		nulls[i]->setTime(ms);
	}
}

void ofxFBXScene::setFrame(u_int frame){
	for(u_int i=0;i<cameras.size();i++){
		cameras[i]->setFrame(frame);
	}
	for(u_int i=0;i<meshes.size();i++){
		meshes[i]->setFrame(frame);
	}
	for(u_int i=0;i<nulls.size();i++){
		nulls[i]->setFrame(frame);
	}
}

