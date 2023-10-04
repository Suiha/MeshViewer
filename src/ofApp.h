#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofxGui.h"

class Mesh {

public:
	vector<glm::vec3> vertexList;
	vector<tuple<int, int, int>> triangleList; // int = vertexList index -> vertices of the triangle (counter clockwise)
	vector<tuple<int, int, int>> selectTriangles; // int = triangleList index
	
	void draw();
	void drawFromTriangle(tuple<int, int, int> t, ofColor c);
	
	// reading obj file and helper methods to parse vector & face lines
	void readFile();
	void parseVector(string line);
	void parseFace(string line);

	void meshAddVert(float x, float y, float z);
	void meshAddVert(glm::vec3 v);
	void meshAddTriangle(int i, int j, int k);

	ofFile file;
	ofBuffer buffer = ofBufferFromFile("car_model/Car.obj");
	// ofxAssimpModelLoader model;

	bool showMesh;
	float meshSize;
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void readObjFile();

		// objects
		ofEasyCam cam;
		Mesh mesh;

		bool bModelLoaded = false;

		// part 3 gui
		bool bHide = false;

		void vertexEntered(bool& value);
		void faceEntered(bool& value);
		bool isNumber(string s);
		void ofApp::addAdjTriangle(int tIndex, int v);

		ofxToggle meshView;
		ofParameter<string> userInput;
		ofxToggle vertexButton;
		ofxToggle faceButton;
		ofxLabel vertexNum;
		ofxLabel faceNum;
		ofxLabel modelSize;

		ofxPanel gui;
};
