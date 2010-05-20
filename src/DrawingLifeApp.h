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
	void fillViewArea( int backgroundColor);
	/**
	* \brief Draw a square to draw onto.
	* \param backgroundColor color of the square.
	*/
	void fillViewAreaUTM( int backgroundColor);
	/**
	* \brief
	* \param city string with city name.
	*/
	void loadGpsDataCity(vector<string> names, string city);
	/**
	* \brief Set square view area and center.
	*/
	void setViewAspectRatio();

    /**
    * \brief Draw start screen with app info.
    */
	void drawStartScreen();
    //---------------------------------------------------------------------------
    // Member variables
    //---------------------------------------------------------------------------
	ofxXmlSettings m_settings;
	DBReader* m_dbReader;
	vector<GpsData* > m_gpsDatas;
	string m_dbPath;
	//---------------------------------------------------------------------------
	int maxPoints;
	//---------------------------------------------------------------------------
	double m_viewXOffset;
	double m_viewYOffset;
	double m_viewMinDimension;
	double m_viewPadding;
	//---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
	bool m_isFullscreen;
	bool m_isDebugMode;
	bool m_isAnimation;
	//---------------------------------------------------------------------------
	double m_zoomX;
	double m_zoomY;
	double m_zoomZ;
	//---------------------------------------------------------------------------
	bool m_startScreenMode;
	ofTrueTypeFont m_fontTitle;
	ofTrueTypeFont m_fontAuthor;
	ofTrueTypeFont m_fontText;
    // -----------------------------------------------------------------------------
    int m_numPerson;
    vector<string> m_names;
};

#endif // _DRAWINGLIFEAPP_H_
