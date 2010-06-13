#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	this->counter = new TimedCounter(5);
	this->counter->startCount();
}

void testApp::exit(){
	delete this->counter;
}

//--------------------------------------------------------------
void testApp::update(){
	if(this->counter->isCounting()){
		this->counter->update();
		if(this->counter->hasChanged())
			cout << "counter changed:" << this->counter->getCurrentCount() << endl;
		if(this->counter->isCountComplete())
			cout << "counter complete!" << endl;
	}
}

//--------------------------------------------------------------
void testApp::draw(){

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

