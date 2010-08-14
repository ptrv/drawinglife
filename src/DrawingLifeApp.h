/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#ifndef _DRAWINGLIFEAPP_H_
#define _DRAWINGLIFEAPP_H_

#include "DrawingLifeIncludes.h"
#include "ofxXmlSettings.h"
#include "ofxVectorMath.h"
#include "DBReader.h"
#include "Timeline.h"
#include "Walk.h"

/**
 *  \brief Main application class.
 */

class DrawingLifeApp : public ofBaseApp
{
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
	void fillViewArea();
	/**
	* \brief Draw a square to draw onto.
	* \param backgroundColor color of the square.
	*/
	void fillViewAreaUTM();
	/**
	* \brief
	* \param names vector with name strings.
	* \param city string with city name.
	*/
	void loadGpsDataCity(std::vector<string> names, std::string city);
	/**
	* \brief
	* \param names vector with name strings.
	* \param yearStart start year for query.
	* \param yearEnd end year for query.
	*/
	void loadGpsDataYearRange(std::vector<string> names, int yearStart, int yearEnd);
	/**
	* \brief Set square view area and center.
	*/
	void setViewAspectRatio();

    /**
    * \brief Draw start screen with app info.
    */
	void drawStartScreen();

	void calculateGlobalMinMaxValues();

	void loadXmlSettings();
    //---------------------------------------------------------------------------
    // Member variables
    //---------------------------------------------------------------------------
	ofxXmlSettings m_settings;
	DBReader* m_dbReader;

	std::vector<GpsData* > m_gpsDatas;
	std::vector<Walk* > m_walks;
	std::vector<MagicBox* > m_magicBoxes;

	std::string m_dbPath;
	//---------------------------------------------------------------------------
	int maxPoints;
	//---------------------------------------------------------------------------
	std::vector<double> m_viewXOffset;
	std::vector<double> m_viewYOffset;
	std::vector<double> m_viewMinDimension;
	std::vector<double> m_viewPadding;
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
	ofTrueTypeFont m_fontInfo;
    // -----------------------------------------------------------------------------
    int m_numPerson;
    std::vector<string> m_names;
	std::string m_currentCity;
    // -----------------------------------------------------------------------------
    Timeline* m_timeline;
    int m_drawSpeed;

	int m_trackAlpha;
    int m_dotAlpha;
    int m_legendAlpha;
    int m_startScreenDuration;
    int m_loadOnStart;
    int m_frameRate;

    int m_logLevel;

    DBQueryData m_dbQueryData;

    ofColor m_colorForeground;
    ofColor m_colorBackground;
    ofColor m_colorViewbox;

    double m_magicBoxSize;
    double m_magicBoxPadding;
};

#endif // _DRAWINGLIFEAPP_H_
