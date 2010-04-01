/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#include "DrawingLifeApp.h"
#include "Data.h"
#include <vector>
//--------------------------------------------------------------
DrawingLifeApp::DrawingLifeApp() :  m_dbReader(NULL),
									m_gpsData(NULL),
									m_isFullscreen(false),
									m_isDebugMode(false),
									m_currentSelectedDayStart(9),
									m_currentSelectedDayEnd(18)
{
	m_viewXOffset = 0;
	m_viewYOffset = 0;
	m_viewMinDimension = 0;
	m_viewPadding = 15;
}
DrawingLifeApp::~DrawingLifeApp()
{
		SAFE_DELETE(m_gpsData);
}
void DrawingLifeApp::setup(){

	ofSetFrameRate(60);
    ofEnableAlphaBlending();

	// reading settings from xml file
	m_settings.loadFile("AppSettings.xml");
//	ofSetLogLevel(m_settings.getAttribute("settings:log", "level", 0));
	ofSetLogLevel(OF_LOG_VERBOSE);
	// db path must be absolute path for DBReader (true as second parameter)
	m_dbPath = ofToDataPath(m_settings.getValue("settings:database", "test.sqlite"), true);

	DBG_VAL(m_dbPath);

	m_gpsData = new GpsData();

	// get GpsData from database
	m_dbReader = new DBReader(m_dbPath);
	if (m_dbReader->setupDbConnection())
	{
	// -----------------------------------------------------------------------------
	// DB query
	if(m_dbReader->getGpsDataDayRange(*m_gpsData, "Dan", 2010, 2, 9, 18))
	{
		ofLog(OF_LOG_SILENT, "--> GpsData load ok!");
		ofLog(OF_LOG_SILENT, "--> Total data: %d GpsSegments, %d GpsPoints!",
			  m_gpsData->getSegments().size(),
			  m_gpsData->getTotalGpsPoints());
	}
	else
	{
		ofLog(OF_LOG_SILENT, "--> No GpsData loaded!");
	}
	}
	// -----------------------------------------------------------------------------
	m_dbReader->closeDbConnection();
	SAFE_DELETE(m_dbReader);

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
//	ofLog(OF_LOG_VERBOSE, "minLon: %lf, maxLon: %lf, minLat: %lf, maxLat: %lf",
//		  m_gpsData->getMinLon(),
//		  m_gpsData->getMaxLon(),
//		  m_gpsData->getMinLat(),
//		  m_gpsData->getMaxLat());
	ofLog(OF_LOG_VERBOSE, "minLon: %lf, maxLon: %lf, minLat: %lf, maxLat: %lf",
		  m_gpsData->getMinUtmX(),
		  m_gpsData->getMaxUtmX(),
		  m_gpsData->getMinUtmY(),
		  m_gpsData->getMaxUtmY());

	setMinMaxRatioUTM();

	// Because the above test print is so slow, here you can prove
	// that the data have been read by showing las gpsData
//	ofLog(OF_LOG_NOTICE, m_gpsData->getSegments().back().getPoints().back().getTimestamp());

	ofBackground((BACKGROUND >> 16) & 0xFF, (BACKGROUND >> 8) & 0xFF, (BACKGROUND) & 0xFF);

	m_currentGpsPoint = 0;
	m_currentGpsSegment = 0;
	m_currentPoint = -1;
	m_firstPoint = true;

	this->setViewAspectRatio();
}

//--------------------------------------------------------------
void DrawingLifeApp::update(){

	// Counting through GpsSegments and GpsPoints
	if (m_gpsData->getSegments().size() > 0)
	{
		if ((unsigned int)m_currentGpsSegment < m_gpsData->getSegments().size()-1)
		{
			if ((unsigned int)m_currentGpsPoint < m_gpsData->getSegments()[m_currentGpsSegment].getPoints().size() - 1)
			{
				if (!m_firstPoint)
					++m_currentGpsPoint;
				else
					m_firstPoint = false;
			}
			else
			{
				++m_currentGpsSegment;
				m_currentGpsPoint = 0;
			}
		}
		else
		{
			if ((unsigned int)m_currentGpsPoint < m_gpsData->getSegments()[m_currentGpsSegment].getPoints().size() - 1)
			{
				++m_currentGpsPoint;
			}
			else
			{
				m_currentGpsPoint = 0;
				m_currentGpsSegment = 0;
				m_currentPoint = -1;
			}
		}
		++m_currentPoint;
	}
}


//--------------------------------------------------------------
void DrawingLifeApp::draw(){


	if (m_gpsData->getSegments().size() > 0 && m_gpsData->getSegments()[m_currentGpsSegment].getPoints().size() > 0)
	{
		// -----------------------------------------------------------------------------
		// Draw rectangle with text.
		// -----------------------------------------------------------------------------
		double lat = m_gpsData->getSegments()[m_currentGpsSegment].getPoints()[m_currentGpsPoint].getLatitude();
		double lon = m_gpsData->getSegments()[m_currentGpsSegment].getPoints()[m_currentGpsPoint].getLongitude();
		double ele = m_gpsData->getSegments()[m_currentGpsSegment].getPoints()[m_currentGpsPoint].getElevation();

		string timest = m_gpsData->getSegments()[m_currentGpsSegment].getPoints()[m_currentGpsPoint].getTimestamp();
		string currentLocation = m_gpsData->getGpsLocation(m_currentGpsSegment, m_currentGpsPoint);
		string info =	"Longitude  : " + ofToString(lon) + "\n" +
						"Latitude   : " + ofToString(lat) + "\n" +
						"Elevation  : " + ofToString(ele) + "\n" +
						"Time       : " + timest + "\n" +
						"Location   : " + currentLocation + "\n" +
						"Cur. point : " + ofToString(m_currentPoint) + "\n" +
						"Segment nr : " + ofToString(m_gpsData->getSegments()[m_currentGpsSegment].getSegmentNum());

		//fillViewArea( 0xededed);
		fillViewAreaUTM( 0xededed);

		ofFill();
		ofSetColor(0xE5A93F);
		ofRect(10,10,300,120);
		ofSetColor(0x000000);
		ofDrawBitmapString(info,30,30);

		// -----------------------------------------------------------------------------
		// Draw Gps data
		// -----------------------------------------------------------------------------
		ofSetColor(FOREGROUND);
		ofNoFill();
		for (unsigned int i = 0; i <= m_currentGpsSegment; ++i)
		{
			glBegin(GL_LINE_STRIP);
			int pointEnd;
			if (i == m_currentGpsSegment)
				pointEnd = m_currentGpsPoint;
			else
				pointEnd = (int)m_gpsData->getSegments()[i].getPoints().size()-1;
			for (unsigned int j = 0; j <= pointEnd; ++j)
			{
//				glVertex2d(getNormalizedLongitude(m_gpsData->getLongitude(i,j)),
//						   getNormalizedLatitude(m_gpsData->getLatitude(i,j)));
				glVertex2d(getNormalizedUtmX(m_gpsData->getUtmX(i,j)),
						   getNormalizedUtmY(m_gpsData->getUtmY(i,j)));
			}
			glEnd();
		}
		ofFill();
		ofSetColor(0, 255, 0, 127);
//		ofCircle(getNormalizedLongitude(m_gpsData->getLongitude(m_currentGpsSegment,m_currentGpsPoint)),
//				 getNormalizedLatitude(m_gpsData->getLatitude(m_currentGpsSegment,m_currentGpsPoint)), 5);
		ofCircle(getNormalizedUtmX(m_gpsData->getUtmX(m_currentGpsSegment,m_currentGpsPoint)),
				 getNormalizedUtmY(m_gpsData->getUtmY(m_currentGpsSegment,m_currentGpsPoint)), 5);
	}



}
// -----------------------------------------------------------------------------
// Retreiving new GpsData
// -----------------------------------------------------------------------------
void DrawingLifeApp::getNewGpsData()
{
	// get GpsData from database
	m_gpsData->clear();
	SAFE_DELETE(m_gpsData);
	m_gpsData = new GpsData();
	m_currentGpsSegment = 0;
	m_currentGpsPoint = 0;
	m_currentPoint = -1;
	m_dbReader = new DBReader(m_dbPath);
	if (m_dbReader->setupDbConnection())
	{
		// -----------------------------------------------------------------------------
		// DB query
		if(m_dbReader->getGpsDataDayRange(*m_gpsData, "Dan", 2010, 2, m_currentSelectedDayStart, m_currentSelectedDayEnd))
		{
			ofLog(OF_LOG_SILENT, "--> GpsData load ok!");
			ofLog(OF_LOG_SILENT, "--> Total data: %d GpsSegments, %d GpsPoints!",
				  m_gpsData->getSegments().size(),
				  m_gpsData->getTotalGpsPoints());
		}
		else
		{
			ofLog(OF_LOG_SILENT, "--> No GpsData loaded!");
		}
	}
	// -----------------------------------------------------------------------------
	m_dbReader->closeDbConnection();
	SAFE_DELETE(m_dbReader);
	setMinMaxRatioUTM();
}
// -----------------------------------------------------------------------------
// Set min/max ratio
// -----------------------------------------------------------------------------
void DrawingLifeApp::setMinMaxRatio()
{
	double minLon = m_gpsData->getMinLon();
	double maxLon = m_gpsData->getMaxLon();
	double minLat = m_gpsData->getMinLat();
	double maxLat = m_gpsData->getMaxLat();

	double deltaLon = maxLon - minLon;
	double deltaLat = maxLat - minLat;

	if (deltaLon <	deltaLat)
	{
		m_minLon = minLon - (deltaLat - deltaLon)/2;
		m_maxLon = maxLon + (deltaLat - deltaLon)/2;
		m_minLat = minLat;
		m_maxLat = maxLat;
	}
	else if (deltaLat < deltaLon)
	{
		m_minLon = minLon;
		m_maxLon = maxLon;
		m_minLat = minLat - (deltaLon - deltaLat);
		m_maxLat = maxLat + (deltaLon - deltaLat);
	}
	else
	{
		m_minLon = minLon;
		m_maxLon = maxLon;
		m_minLat = minLat;
		m_maxLat = maxLat;
	}
}
// -----------------------------------------------------------------------------
// Set min/max ratio with UTM values
// -----------------------------------------------------------------------------
void DrawingLifeApp::setMinMaxRatioUTM()
{
	double minLon = m_gpsData->getMinUtmX();
	double maxLon = m_gpsData->getMaxUtmX();
	double minLat = m_gpsData->getMinUtmY();
	double maxLat = m_gpsData->getMaxUtmY();

	double deltaLon = maxLon - minLon;
	double deltaLat = maxLat - minLat;
	if (deltaLon <	deltaLat)
	{
		m_minUtmX = minLon - (deltaLat - deltaLon)/2;
		m_maxUtmX = maxLon + (deltaLat - deltaLon)/2;
		m_minUtmY = minLat;
		m_maxUtmY = maxLat;
	}
	else if (deltaLat < deltaLon)
	{
		m_minUtmX = minLon;
		m_maxUtmX = maxLon;
		m_minUtmY = minLat - (deltaLon - deltaLat);
		m_maxUtmY = maxLat + (deltaLon - deltaLat);
	}
	else
	{
		m_minUtmX = minLon;
		m_maxUtmX = maxLon;
		m_minUtmY = minLat;
		m_maxUtmY = maxLat;
	}
}

// -----------------------------------------------------------------------------
double DrawingLifeApp::getNormalizedLongitude(double lon)
{
	return ( (lon - m_minLon) / (m_maxLon - m_minLon) * (m_viewMinDimension - 2 * m_viewPadding) + m_viewXOffset);
}

double DrawingLifeApp::getNormalizedLatitude(double lat)
{
//    return ( (lat - m_minLat) / (m_maxLat - m_minLat) * (m_viewMinDimension - 2 * m_viewPadding) + m_viewYOffset);
	// Flip y coordinates ??
	return m_viewMinDimension - ( (lat - m_minLat) / (m_maxLat - m_minLat) * (m_viewMinDimension - 2 * m_viewPadding) + m_viewYOffset);
}

// -----------------------------------------------------------------------------
double DrawingLifeApp::getNormalizedUtmX(double lon)
{
	return ( (lon - m_minUtmX) / (m_maxUtmX - m_minUtmX) * (m_viewMinDimension - 2 * m_viewPadding) + m_viewXOffset);
}

double DrawingLifeApp::getNormalizedUtmY(double lat)
{
	//    return ( (lat - m_minUtmY) / (m_maxUtmY - m_minUtmY) * (m_viewMinDimension - 2 * m_viewPadding) + m_viewYOffset);
	// Flip y coordinates ??
	return m_viewMinDimension - ( (lat - m_minUtmY) / (m_maxUtmY - m_minUtmY) * (m_viewMinDimension - 2 * m_viewPadding) + m_viewYOffset);
}

// -----------------------------------------------------------------------------

void DrawingLifeApp::setViewAspectRatio()
{
	int width = ofGetWidth();
	int height = ofGetHeight();

	// Reset for view padding.
	m_viewXOffset = 0;
	m_viewYOffset = 0;

	// Set square view area and center.
	if (height < width)
	{
		m_viewMinDimension = height;
		m_viewXOffset = (width - height) / 2;
	}
	else if (width < height)
	{
		m_viewMinDimension = width;
		m_viewYOffset = (height - width) / 2;
	}
	else
	{
		m_viewMinDimension = width;
	}

	// Left and top intend.
	m_viewXOffset += m_viewPadding;
	m_viewYOffset += m_viewPadding;
}
void DrawingLifeApp::fillViewArea( int backgroundColor)
{
	double x = getNormalizedLongitude(m_minLon);
	double y = getNormalizedLatitude(m_minLat);
	double w = getNormalizedLongitude(m_maxLon) - x;
	double h = getNormalizedLatitude(m_maxLat) - y;
	ofFill();
	ofSetColor( backgroundColor);
	ofRect(x, y, w, h);
}
void DrawingLifeApp::fillViewAreaUTM( int backgroundColor)
{
	double x = getNormalizedUtmX(m_minUtmX);
	double y = getNormalizedUtmY(m_minUtmY);
	double w = getNormalizedUtmX(m_maxUtmX) - x;
	double h = getNormalizedUtmY(m_maxUtmY) - y;
	ofFill();
	ofSetColor( backgroundColor);
	ofRect(x, y, w, h);
}

//--------------------------------------------------------------
void DrawingLifeApp::keyPressed  (int key){
	switch (key) {
		case 'f':
			m_isFullscreen = !m_isFullscreen;
			ofSetFullscreen(m_isFullscreen);
			break;
		case 'w':
			m_currentSelectedDayStart += 1;
			getNewGpsData();
			DBG_VAL(m_currentSelectedDayStart);
			break;
		case 's':
			m_currentSelectedDayStart -= 1;
			DBG_VAL(m_currentSelectedDayStart);
			getNewGpsData();
			break;
		case 'e':
			m_currentSelectedDayEnd += 1;
			DBG_VAL(m_currentSelectedDayEnd);
			getNewGpsData();
			break;
		case 'd':
			m_currentSelectedDayEnd -= 1;
			DBG_VAL(m_currentSelectedDayEnd);
			getNewGpsData();
			break;
		default:
			break;
	}
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
void DrawingLifeApp::windowResized(int w, int h)
{
	this->setViewAspectRatio();
}

