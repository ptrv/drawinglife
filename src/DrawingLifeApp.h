/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#ifndef _DRAWINGLIFEAPP_H_
#define _DRAWINGLIFEAPP_H_

#include "DrawingLifeIncludes.h"
#include "ofxXmlSettings.h"
#include "ofxVectorMath.h"
#include "DBReader.h"

/**
 *  \brief Main application class.
 */

class DrawingLifeApp : public ofBaseApp
{
public: /* static */
	static const int BACKGROUND = 0x000000;
	static const int FOREGROUND = 0xFFFFFF;
	static const int VIEWBOX = 0x343434;

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

private:
    //---------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------
	double getNormalizedLatitude(double lat);
	double getNormalizedLongitude(double lon);
	double getScaledUtmY(double utmY);
	double getScaledUtmX(double utmX);

	void fillViewArea( int backgroundColor);
	void fillViewAreaUTM( int backgroundColor);
	void getNewGpsData();
	void setMinMaxRatio();
	// Sets square view area and center.
	void setViewAspectRatio();
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    // Member variables
    //---------------------------------------------------------------------------
	ofxXmlSettings m_settings;
	DBReader* m_dbReader;
	GpsData* m_gpsData;
	string m_dbPath;
	//---------------------------------------------------------------------------
	int m_currentGpsPoint;
	int m_currentGpsSegment;
	int maxPoints;
	int m_currentPoint;
	bool m_firstPoint;
	//---------------------------------------------------------------------------
	double m_viewXOffset;
	double m_viewYOffset;
	double m_viewMinDimension;
	double m_viewPadding;
	//---------------------------------------------------------------------------
	double m_minLon;
	double m_maxLon;
	double m_minLat;
	double m_maxLat;
	double m_minUtmX;
	double m_maxUtmX;
	double m_minUtmY;
	double m_maxUtmY;
    //---------------------------------------------------------------------------
	bool m_isFullscreen;
	bool m_isDebugMode;
	bool m_isAnimation;
    //---------------------------------------------------------------------------
	int m_currentSelectedDayStart;
	int m_currentSelectedDayEnd;
	//---------------------------------------------------------------------------
};

#endif // _DRAWINGLIFEAPP_H_
