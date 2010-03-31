/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#ifndef _DRAWINGLIFEAPP_H_
#define _DRAWINGLIFEAPP_H_


#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxVectorMath.h"
#include "Logger.h"
#include "DBReader.h"

/**
 *  \brief Main application class.
 */

class DrawingLifeApp : public ofBaseApp{
	
public:
	DrawingLifeApp();
	virtual ~DrawingLifeApp();
	void setup();
	void update();
	void draw();
	
	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	
	double getNormalizedLatitude(double lat);
	double getNormalizedLongitude(double lon);
	
	void setViewAspectRatio();
	
	ofxXmlSettings m_settings;
	DBReader* m_dbReader;
	GpsData* m_gpsData;
	string m_dbPath;
	int m_currentGpsPoint;
	int m_currentGpsSegment;
	int maxPoints;
	int m_currentPoint;
	int m_viewXOffset;
	int m_viewYOffset;
	int m_viewMinDimension;
	int m_viewPadding;
	bool m_firstPoint;
	double m_minLon;
	double m_maxLon;
	double m_minLat;
	double m_maxLat;

	bool m_isFullscreen;
	bool m_isDebugMode;
	static const int BACKGROUND = 0xFFFFFF; 
	static const int FOREGROUND = 0x000000;

private:
	void fillViewArea( int backgroundColor);
};

#endif // _DRAWINGLIFEAPP_H_
