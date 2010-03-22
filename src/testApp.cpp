/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#include "testApp.h"
#include "Data.h"
#include <vector>
//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetFrameRate(20);

	// reading settings from xml file
	settings.loadFile("AppSettings.xml");
	ofSetLogLevel(settings.getAttribute("settings:log", "level", 0));
	string dbPath = ofToDataPath(settings.getValue("settings:database", "test.db"), true);
	
	DBG_VAL(dbPath);
	
	GpsData gpsData;
	
	// get GpsData from database
	m_dbReader = new DBReader(dbPath);
	m_dbReader->setupDbConnection();
	m_dbReader->getGpsDataDay(gpsData, 11);
	m_dbReader->closeDbConnection();
	
	// test print
	int k = 0;
	for (unsigned int i = 0; i < gpsData.getSegments().size(); ++i) {
		for (unsigned int j = 0; j < gpsData.getSegments()[i].getPoints().size(); ++j) {
			stringstream message;
			message << "Value i " << i << ", j " << j << ", k " << k <<": ";
			message << ofToString(gpsData.getSegments()[i].getPoints()[j].getLongitude());
			message << ", ";
			message << ofToString(gpsData.getSegments()[i].getPoints()[j].getLatitude());
			message << ", ";
			message << ofToString(gpsData.getSegments()[i].getPoints()[j].getElevation());
			message << ", ";
			message << gpsData.getSegments()[i].getPoints()[j].getTimestamp();
			message << ", ";
			message << gpsData.getSegments()[i].getSegmentNum();
			ofLog(OF_LOG_NOTICE, message.str() );
			++k;
		}
	}
	delete m_dbReader;
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

