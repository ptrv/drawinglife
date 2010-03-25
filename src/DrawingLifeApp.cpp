/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#include "DrawingLifeApp.h"
#include "Data.h"
#include <vector>
//--------------------------------------------------------------
DrawingLifeApp::~DrawingLifeApp()
{
	if (m_gpsData != NULL) 
	{
		delete m_gpsData;
	}
}
void DrawingLifeApp::setup(){
	
	ofSetFrameRate(10);

	// reading settings from xml file
	m_settings.loadFile("AppSettings.xml");
	ofSetLogLevel(m_settings.getAttribute("settings:log", "level", 0));
	// db path must be absolute path for DBReader (true as second parameter)
	string dbPath = ofToDataPath(m_settings.getValue("settings:database", "test.db"), true);
	
	DBG_VAL(dbPath);
	
	m_gpsData = new GpsData();
	
	// get GpsData from database
	m_dbReader = new DBReader(dbPath);
	m_dbReader->setupDbConnection();
	m_dbReader->getGpsDataDay(*m_gpsData, "Dan", 9);
	m_dbReader->closeDbConnection();
	delete m_dbReader;
	
	// test print
	maxPoints = 0;
	for (unsigned int i = 0; i < m_gpsData->getSegments().size(); ++i) {
		for (unsigned int j = 0; j < m_gpsData->getSegments()[i].getPoints().size(); ++j) {
			stringstream message;
			//message << "Value i " << i << ", j " << j << ", k " << k <<": ";
			message << "GpsPoint nr " << maxPoints << ": ";
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
			++maxPoints;
		}
	}
	// printing min/max values
	ofLog(OF_LOG_NOTICE, "minLon: %lf, maxLon: %lf, minLat: %lf, maxLat: %lf", 
		  m_gpsData->getMinLon(), 
		  m_gpsData->getMaxLon(),
		  m_gpsData->getMinLat(),
		  m_gpsData->getMaxLat());

	// Because the above test print is so slow, here you can prove 
	// that the data have been read by showing las gpsData
//	ofLog(OF_LOG_NOTICE, m_gpsData->getSegments().back().getPoints().back().getTimestamp());
	
	ofBackground(0, 0, 0);
	
	currentGpsPoint = -1;
	currentGpsSegment = 0;
	currentPoint = -1;
}

//--------------------------------------------------------------
void DrawingLifeApp::update(){
	
	if (m_gpsData->getSegments().size() > 0) 
	{
		
		if ((unsigned int)currentGpsSegment < m_gpsData->getSegments().size()-1) 
		{
			if ((unsigned int)currentGpsPoint < m_gpsData->getSegments()[currentGpsSegment].getPoints().size() - 1) 
			{
				++currentGpsPoint;
			}
			else
			{
				++currentGpsSegment;
				currentGpsPoint = 0;
			}
		}
		else
		{
			currentGpsPoint = 0;
			currentGpsSegment = 0;
			currentPoint = -1;
		}
		++currentPoint;
	}
}


//--------------------------------------------------------------
void DrawingLifeApp::draw(){
	
	if (m_gpsData->getSegments().size() > 0)
	{
		double lat = m_gpsData->getSegments()[currentGpsSegment].getPoints()[currentGpsPoint].getLatitude();
		double lon = m_gpsData->getSegments()[currentGpsSegment].getPoints()[currentGpsPoint].getLongitude();
		double ele = m_gpsData->getSegments()[currentGpsSegment].getPoints()[currentGpsPoint].getElevation();
		string timest = m_gpsData->getSegments()[currentGpsSegment].getPoints()[currentGpsPoint].getTimestamp();
		string info = "Latitude:   " + ofToString(lat) + "\n" +
		"Longitude:  " + ofToString(lon) + "\n" +
		"Elevation:  " + ofToString(ele) + "\n" +
		"Time:       " + timest + "\n" +
		"Cur. point: " + ofToString(currentPoint);
		
		ofFill();
		ofSetColor(0xE5A93F);
		ofRect(10,10,300,85);
		ofSetColor(0x000000);
		ofDrawBitmapString(info,30,30);
	}
	
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

