/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#include "DrawingLifeApp.h"
#include "Data.h"
#include <vector>
//--------------------------------------------------------------
DrawingLifeApp::~DrawingLifeApp()
{
	delete m_gpsData;
}
void DrawingLifeApp::setup(){
	
	ofSetFrameRate(20);

	// reading settings from xml file
	m_settings.loadFile("AppSettings.xml");
	ofSetLogLevel(m_settings.getAttribute("settings:log", "level", 0));
	// db path must be absolute path for DBReader
	string dbPath = ofToDataPath(m_settings.getValue("settings:database", "test.db"), true);
	
	DBG_VAL(dbPath);
	
	m_gpsData = new GpsData();
	
	// get GpsData from database
	m_dbReader = new DBReader(dbPath);
	m_dbReader->setupDbConnection();
	m_dbReader->getGpsDataDay(*m_gpsData, 11);
	m_dbReader->closeDbConnection();
	
	// test print
	int k = 0;
	for (unsigned int i = 0; i < m_gpsData->getSegments().size(); ++i) {
		for (unsigned int j = 0; j < m_gpsData->getSegments()[i].getPoints().size(); ++j) {
			stringstream message;
			//message << "Value i " << i << ", j " << j << ", k " << k <<": ";
			message << "GpsPoint nr " << k << ": ";
			message << m_gpsData->getSegments()[i].getPoints()[j].getLatitude();
			message << ", ";
			message << m_gpsData->getSegments()[i].getPoints()[j].getLongitude();
			message << ", ";
			message << m_gpsData->getSegments()[i].getPoints()[j].getElevation();
			message << ", ";
			message << m_gpsData->getSegments()[i].getPoints()[j].getTimestamp();
			message << ", ";
			message << m_gpsData->getSegments()[i].getSegmentNum();
			ofLog(OF_LOG_NOTICE, message.str() );
			++k;
		}
	}
	
	delete m_dbReader;
}

//--------------------------------------------------------------
void DrawingLifeApp::update(){
}


//--------------------------------------------------------------
void DrawingLifeApp::draw(){
}

//--------------------------------------------------------------
void DrawingLifeApp::keyPressed  (int key){

}

//--------------------------------------------------------------
void DrawingLifeApp::keyReleased(int key){

}

//--------------------------------------------------------------
void DrawingLifeApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void DrawingLifeApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void DrawingLifeApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void DrawingLifeApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void DrawingLifeApp::resized(int w, int h){

}
