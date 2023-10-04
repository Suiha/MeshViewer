#include "ofApp.h"

void Mesh::draw() {
	
	ofPushMatrix();
	ofSetColor(ofColor::dimGray);
	ofRotateDeg(90);
	ofDrawGridPlane();
	ofPopMatrix();

	ofPushMatrix();
	ofNoFill();
	// draw the model
	if (showMesh) {
		for (tuple<int, int, int> t : triangleList) {
			drawFromTriangle(t, ofColor::red);
		}
	}

	// draw selected faces from gui (user input)
	if (selectTriangles.size() > 0) {
		for (auto st : selectTriangles) {
			ofFill();
			drawFromTriangle(st, ofColor::green);
		}
	}
	ofPopMatrix();
}

void Mesh::drawFromTriangle(tuple<int, int, int> t, ofColor c) {
	int t1 = get<0>(t);
	int t2 = get<1>(t);
	int t3 = get<2>(t);
	// each int represents a vertex from vertexList
	glm::vec3 v1 = vertexList[t1];
	glm::vec3 v2 = vertexList[t2];
	glm::vec3 v3 = vertexList[t3];
	ofSetColor(c);
	ofDrawTriangle(v1, v2, v3);
}

void Mesh::readFile() {
	file.open("car_model/Car.obj");
	for (auto line : buffer.getLines()) {
		vector<string> lineContent;
		stringstream s(line);
		string word;

		// only parse lines that are vertex ("v") or face ("f")
		while (s >> word) {
			if (word == "v") {
				parseVector(line);
			}
			else if (word == "f") {
				parseFace(line);
			}
			break;
		}
	}

	// Number of Vertices & Faces
	printf("Vertices: %zd\n", vertexList.size());
	printf("Faces: %zd\n", triangleList.size());

	// Size of the Mesh in kB
	// each vertex is 3 coordinates * 4 bytes
	float vertexData = vertexList.size() * sizeof(float) * 3;
	// each triangle is 3 indices * 4 bytes * 2 triangles per vertex (on average)
	float triangleData = triangleList.size() * sizeof(int) * 6;
	// 1 kB = 1000 bytes
	meshSize = (vertexData + triangleData) / 1000;
	printf("Size of Mesh: %f kB\n", meshSize);

	file.close();
}

void Mesh::parseVector(string line) {
	vector<string> lineContent;
	stringstream s(line);
	string word;
	while (s >> word) {
		lineContent.push_back(word);
	}

	float x = std::stof(lineContent[1]);
	float y = std::stof(lineContent[2]);
	float z = std::stof(lineContent[3]);
	meshAddVert(x, y, z);
}

void Mesh::parseFace(string line) {
	vector<string> lineContent;
	stringstream s(line);
	string word;
	while (s >> word) {
		lineContent.push_back(word);
	}

	// face lines are given as n0/n1/n2, where we only want n0
	for (int i = 1; i < lineContent.size(); i++) {
		string tmp;
		for (char c : lineContent[i]) {
			if (c == '/') {
				lineContent[i] = tmp;
				tmp = "";
				break;
			}
			tmp += c;
		}
	}

	// face indexes start at 1 in file, subtract by 1 to start by 0
	int v1 = stoi(lineContent[1]) - 1;
	int v2 = stoi(lineContent[2]) - 1;
	int v3 = stoi(lineContent[3]) - 1;
	meshAddTriangle(v1, v2, v3);
}

void Mesh::meshAddVert(float x, float y, float z) {
	glm::vec3 vertex = glm::vec3(x, y, z);
	vertexList.push_back(vertex);
}

void Mesh::meshAddVert(glm::vec3 vertex) {
	vertexList.push_back(vertex);
};

void Mesh::meshAddTriangle(int i, int j, int k) {
	tuple<int, int, int> triangle = { i, j, k };
	triangleList.push_back(triangle);
}

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetBackgroundColor(ofColor::black);
	cam.setDistance(10);
	cam.setNearClip(.1);

	vertexButton.addListener(this, &ofApp::vertexEntered);
	faceButton.addListener(this, &ofApp::faceEntered);

	gui.setup();
	gui.add(meshView.setup("View Mesh", true));
	gui.add(userInput.set("Vertex/Face Index: ", ""));
	gui.add(vertexButton.setup("Select Vertex", false));
	gui.add(faceButton.setup("Select Face", false));

	mesh.readFile();

	gui.add(vertexNum.setup("Vertices", std::to_string(mesh.vertexList.size())));
	gui.add(faceNum.setup("Faces", std::to_string(mesh.triangleList.size())));
	gui.add(modelSize.setup("Mesh Size", std::to_string(mesh.meshSize) + " kB"));
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw() {
	cam.begin();

	ofEnableDepthTest();
	mesh.showMesh = meshView;
	mesh.draw();
	ofDisableDepthTest();
	
	cam.end();

	if (!bHide) {
		gui.draw();
	}
}

void ofApp::vertexEntered(bool& value) {
	// every time the button is pressed, highlighted triangles are reset
	mesh.selectTriangles.clear();

	if (vertexButton) {
		// for visual clarity, vertex and face button can't be on at the same time
		if (faceButton) faceButton = false;
		
		// parsing user input
		if (!isNumber(userInput)) {
			printf("invalid input: enter an integer\n");
		}
		else {
			int vertex = stoi(userInput);
			if (vertex >= mesh.vertexList.size()) {
				printf("vertex index out of bounds\n");
			}
			else {
				// adding adj triangles to highlight
				for (int i = 0; i < mesh.triangleList.size(); i++) {
					addAdjTriangle(i, vertex);
				}
			}
		}
	}
}

void ofApp::faceEntered(bool& value) {
	// every time the button is pressed, highlighted triangles are reset
	mesh.selectTriangles.clear();

	if (faceButton) {
		// for visual clarity, vertex and face button can't be on at the same time
		if (vertexButton) vertexButton = false;

		// parsing user input
		if (!isNumber(userInput)) {
			printf("invalid input: enter an integer\n");
		}
		else {
			int faceIndex = stoi(userInput);
			if (faceIndex >= mesh.triangleList.size()) {
				printf("face index out of bounds\n");
			}
			else {
				// adding adj triangles to highlight
				auto face = mesh.triangleList[faceIndex];
				int f1 = get<0>(face);
				int f2 = get<1>(face);
				int f3 = get<2>(face);
				for (int i = 0; i < mesh.triangleList.size(); i++) {
					if (i == faceIndex) continue;
					addAdjTriangle(i, f1);
					addAdjTriangle(i, f2);
					addAdjTriangle(i, f3);
				}
			}
		}
	}
}

bool ofApp::isNumber(string s) {
	if (s == "") return false;
	for (int i = 0; i < s.size(); i++) {
		if (!isdigit(s[i])) {
			return false;
		}
	}
	return true;
}

void ofApp::addAdjTriangle(int tIndex, int v) {
	tuple<int, int, int> t = mesh.triangleList[tIndex];
	int t1 = get<0>(t);
	int t2 = get<1>(t);
	int t3 = get<2>(t);
	if (v == t1 || v == t2 || v == t3) {
		mesh.selectTriangles.push_back(t);
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key) {
	case 'f':
		ofToggleFullscreen();
		break;
	case 'h':
		bHide = !bHide;
		break;
	default:
		break;
	}
}

//--------------------------------------------------------------
void ofApp::readObjFile() {

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

