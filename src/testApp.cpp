/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#include "testApp.h"
#include "Data.h"
#include <vector>
//--------------------------------------------------------------
void testApp::setup(){
	ofSetFrameRate(20);
	settings.loadFile("AppSettings.xml");
	ofSetLogLevel(settings.getAttribute("settings:log", "level", 0));
	string dbPath = ofToDataPath(settings.getValue("settings:database", "test.db"), true);
	DBG_VAL(dbPath);
	m_dbReader = new DBReader(dbPath);
	vector<GpsPoint> qVec;
	m_dbReader->setupDbConnection();
	//m_dbReader->getQuery(qVec);
	m_dbReader->getGpsDataDayRange(qVec, 9, 11);
	m_dbReader->closeDbConnection();
//	for (unsigned int i = 0; i < qVec.size(); ++i) {
//		stringstream message;
//		message << "Value " << i << ": ";
//		message << ofToString(qVec[i].getLongitude());
//		message << ", ";
//		message << ofToString(qVec[i].getLatitude());
//		message << ", ";
//		message << ofToString(qVec[i].getElevation());
//		message << ", ";
//		message << qVec[i].getTimestamp();
//		//ofLog(OF_LOG_SILENT, "value %i: %lf, %lf, %lf, %s",i, , ,, );
//		ofLog(OF_LOG_SILENT, message.str());
//	}
}

//--------------------------------------------------------------
void testApp::update(){
}


//--------------------------------------------------------------
void testApp::draw(){
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){

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
void testApp::resized(int w, int h){

}

