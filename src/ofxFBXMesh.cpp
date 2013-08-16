/*
 * ofxFBXMesh.cpp
 *
 *  Created on: Sep 10, 2012
 *      Author: arturo
 */

#include "ofxFBXMesh.h"
#include "ofxFBX.h"

ofxFBXMesh::ofxFBXMesh() {
	// TODO Auto-generated constructor stub

}

ofxFBXMesh::~ofxFBXMesh() {
	// TODO Auto-generated destructor stub
}

void ofxFBXMesh::setFBXMesh(FbxMesh* lMesh){
	fbxMesh = lMesh;
	nodeName = lMesh->GetName();
	blendParameters.setName(nodeName);
	mesh.clear();
	blendShapes.clear();
	blendParameters.clear();


	int vertexCounter = 0;
	FbxGeometryElementNormal* normalElement = lMesh->GetElementNormal();

	ofMesh auxMesh;
	if(normalElement && normalElement->GetMappingMode() != FbxLayerElement::eByControlPoint){
		for(int i=0;i<lMesh->GetControlPointsCount();i++){
			auxMesh.addVertex(ofVec3f(lMesh->GetControlPointAt(i).mData[0],lMesh->GetControlPointAt(i).mData[1],lMesh->GetControlPointAt(i).mData[2]));
		}
	}else{
		for(int i=0;i<lMesh->GetControlPointsCount();i++){
			mesh.addVertex(ofVec3f(lMesh->GetControlPointAt(i).mData[0],lMesh->GetControlPointAt(i).mData[1],lMesh->GetControlPointAt(i).mData[2]));
			if(normalElement && normalElement->GetMappingMode() == FbxLayerElement::eByControlPoint){
				mesh.addNormal(toOf(normalElement->GetDirectArray().GetAt(i)));
			}
		}
	}


	for(int i=0;i<lMesh->GetPolygonCount();i++){
		if(lMesh->GetPolygonSize(i)==4){
			if(normalElement && normalElement->GetMappingMode() != FbxLayerElement::eByControlPoint){
				mesh.addNormal(toOf(normalElement->GetDirectArray().GetAt(vertexCounter)));
				mesh.addVertex(auxMesh.getVertex(lMesh->GetPolygonVertex(i,0)));

				mesh.addNormal(toOf(normalElement->GetDirectArray().GetAt(vertexCounter+1)));
				mesh.addVertex(auxMesh.getVertex(lMesh->GetPolygonVertex(i,1)));

				mesh.addNormal(toOf(normalElement->GetDirectArray().GetAt(vertexCounter+2)));
				mesh.addVertex(auxMesh.getVertex(lMesh->GetPolygonVertex(i,2)));


				mesh.addNormal(toOf(normalElement->GetDirectArray().GetAt(vertexCounter+2)));
				mesh.addVertex(auxMesh.getVertex(lMesh->GetPolygonVertex(i,2)));

				mesh.addNormal(toOf(normalElement->GetDirectArray().GetAt(vertexCounter+3)));
				mesh.addVertex(auxMesh.getVertex(lMesh->GetPolygonVertex(i,3)));

				mesh.addNormal(toOf(normalElement->GetDirectArray().GetAt(vertexCounter)));
				mesh.addVertex(auxMesh.getVertex(lMesh->GetPolygonVertex(i,0)));

				vertexCounter+=4;

			}else{
				mesh.addIndex(lMesh->GetPolygonVertex(i,0));
				mesh.addIndex(lMesh->GetPolygonVertex(i,1));
				mesh.addIndex(lMesh->GetPolygonVertex(i,2));

				mesh.addIndex(lMesh->GetPolygonVertex(i,2));
				mesh.addIndex(lMesh->GetPolygonVertex(i,3));
				mesh.addIndex(lMesh->GetPolygonVertex(i,0));
			}
		}else{
			for(int j=0;j<3;j++){
				//
				if(normalElement && normalElement->GetMappingMode() != FbxLayerElement::eByControlPoint){
					mesh.addNormal(toOf(normalElement->GetDirectArray().GetAt(vertexCounter++)));
					mesh.addVertex(auxMesh.getVertex(lMesh->GetPolygonVertex(i,j)));
				}else{
					mesh.addIndex(lMesh->GetPolygonVertex(i,j));
				}
			}
		}
	}

	cout << mesh.getNumVertices() << ": " << mesh.getNumIndices() << endl;

	/*for(int i=0;i<mesh.getNumIndices()/3;i++){
		ofVec3f normal = (mesh.getVertex(mesh.getIndex(i*3+1))-mesh.getVertex(mesh.getIndex(i*3))).cross(mesh.getVertex(mesh.getIndex(i*3+2))-mesh.getVertex(mesh.getIndex(i*3)));
		mesh.addNormal(normal.normalize());
		normal = (mesh.getVertex(mesh.getIndex(i*3+2))-mesh.getVertex(mesh.getIndex(i*3+1))).cross(mesh.getVertex(mesh.getIndex(i*3))-mesh.getVertex(mesh.getIndex(i*3+1)));
		mesh.addNormal(normal.normalize());
		normal = (mesh.getVertex(mesh.getIndex(i*3))-mesh.getVertex(mesh.getIndex(i*2))).cross(mesh.getVertex(mesh.getIndex(i*3+1))-mesh.getVertex(mesh.getIndex(i*3+2)));
		mesh.addNormal(normal.normalize());
	}*/

	for(int i=0;i<lMesh->GetTextureUVCount();++i){

	}

	ofLogVerbose() << "mesh with " << lMesh->GetShapeCount() << " blend shapes";

	int lBlendShapeDeformerCount = lMesh->GetDeformerCount(FbxDeformer::eBlendShape);
	for(int lBlendShapeIndex = 0; lBlendShapeIndex<lBlendShapeDeformerCount; ++lBlendShapeIndex)
	{
		FbxBlendShape* lBlendShape = (FbxBlendShape*)lMesh->GetDeformer(lBlendShapeIndex, FbxDeformer::eBlendShape);


		int lBlendShapeChannelCount = lBlendShape->GetBlendShapeChannelCount();
		ofLogVerbose() << "mesh blend shape with " << lBlendShapeChannelCount << " channels";

		for(int lChannelIndex = 0; lChannelIndex<lBlendShapeChannelCount; ++lChannelIndex)
		{
			FbxBlendShapeChannel* lChannel = lBlendShape->GetBlendShapeChannel(lChannelIndex);

			if(lChannel)
			{
				cout << " adding parameter " << lBlendShape->GetName() << endl;
				ofParameter<float> p;
				p.set(string(lBlendShape->GetName()) + "." + lChannel->GetName(),0,0,1);
				p.addListener(this,&ofxFBXMesh::blendChanged);
				blendParameters.add(p);

			}//If lChannel is valid
		}//For each blend shape channel
	}//For each blend shape deformer

	original = mesh;
}

int ofxFBXMesh::getNumBlendShapes(){
	return blendShapes.size();
}

void ofxFBXMesh::blendChanged(const void * sender, float & pct){
	updateBlend();
}

void ofxFBXMesh::updateBlend(){
	mesh = original;
	int lBlendShapeDeformerCount = fbxMesh->GetDeformerCount(FbxDeformer::eBlendShape);
	for(int lBlendShapeIndex = 0; lBlendShapeIndex<lBlendShapeDeformerCount; ++lBlendShapeIndex)
	{
		FbxBlendShape* lBlendShape = (FbxBlendShape*)fbxMesh->GetDeformer(lBlendShapeIndex, FbxDeformer::eBlendShape);
		int lBlendShapeChannelCount = lBlendShape->GetBlendShapeChannelCount();
		for(int lChannelIndex = 0; lChannelIndex<lBlendShapeChannelCount; ++lChannelIndex){
			//Find which shape should we use according to the weight.
			FbxBlendShapeChannel* lChannel = lBlendShape->GetBlendShapeChannel(lChannelIndex);

			if(lChannel)
			{
				int lShapeCount = lChannel->GetTargetShapeCount();
				double* lFullWeights = lChannel->GetTargetShapeFullWeights();
				for(int lShapeIndex = 0; lShapeIndex<lShapeCount; ++lShapeIndex)
				{
					float lWeight = blendParameters.getFloat(string(lBlendShape->GetName()) + "." + lChannel->GetName());
					FbxShape* lShape = NULL;
					if(lWeight > 0 && lWeight <= lFullWeights[0])
					{
						lShape = lChannel->GetTargetShape(0);
					}
					if(lWeight > lFullWeights[lShapeIndex] && lWeight < lFullWeights[lShapeIndex+1])
					{
						lShape = lChannel->GetTargetShape(lShapeIndex+1);
					}

					if(lShape)
					{
						for (int j = 0; j < mesh.getNumVertices(); j++)
						{
							// Add the influence of the shape vertex to the mesh vertex.
						//	ofVec3f lInfluence = (toOf(lShape->GetControlPoints()[j]) - mesh.getVertices()[j]) * lWeight * 0.01;
							ofVec3f influence = (toOf(lShape->GetControlPoints()[j]) - original.getVertices()[j]) * lWeight;
							mesh.getVertices()[j] += influence;
							//original.getVertices()[j].getInterpolated(toOf(lShape->GetControlPoints()[j]),lWeight);
							//cout << lInfluence << endl;
						}
					}else{
						ofLogWarning() << "no lshape";
					}
				}//For each target shape
			}else{
				ofLogError() << "no channels";
			}
		}
	}
}

void ofxFBXMesh::draw(){
	getNode().transformGL();
	mesh.draw();
	getNode().restoreTransformGL();
}

void ofxFBXMesh::drawWireframe(){
	getNode().transformGL();
	mesh.drawWireframe();
	getNode().restoreTransformGL();
}

ofMesh & ofxFBXMesh::getMesh(){
	return mesh;
}

ofParameterGroup & ofxFBXMesh::getBlendParameters(){
	return blendParameters;
}
