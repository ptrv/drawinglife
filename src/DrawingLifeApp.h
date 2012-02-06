/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#ifndef _DRAWINGLIFEAPP_H_
#define _DRAWINGLIFEAPP_H_

#include "DrawingLifeIncludes.h"
#include "DBReader.h"
#include "Timeline.h"
#include "Walk.h"
#include "LocationImage.h"
#include "Integrator.h"
#include "ofSoundPlayer.h"

/**
 *  \brief Main application class.
 */

class DrawingLifeApp : public ofBaseApp
{
public:
	DrawingLifeApp(std::string settingsFile);
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

//	static const char* settingsPath;

private:
    //---------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------
	void fillViewArea();
	/**
	* \brief Draw a square to draw onto.
	*/
	void fillViewAreaUTM();
	/**
	* \brief
	* \param names vector with name strings.
	* \param city string with city name.
	*/
	bool loadGpsDataCity(std::vector<std::string> names, std::string city);
	/**
	* \brief
	* \param names vector with name strings.
	* \param yearStart start year for query.
	* \param yearEnd end year for query.
	*/
	bool loadGpsDataYearRange(std::vector<std::string> names, int yearStart, int yearEnd);

	bool loadGpsDataWithSqlFile(std::vector<std::string> names, std::vector<std::string> m_sqlFilePaths);
	/**
	* \brief Set square view area and center.
	*/
	void setViewAspectRatio();

    /**
    * \brief Draw start screen with app info.
    */
	void drawStartScreen();

	void showKeyCommands();

	void calculateGlobalMinMaxValues();

	void loadXmlSettings();

	void fpsDisplay();

	void loadCurrentPointImages();

	void prepareGpsData();

	void processGpsData();

	bool zoomHasChanged();

	void zoomUpdate();

	void soundUpdate();

    //---------------------------------------------------------------------------
    // Member variables
    //---------------------------------------------------------------------------
    std::string m_settingsFile;
    AppSettings* m_settings;
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
	bool m_showFps;
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
    unsigned int m_numPerson;
    std::vector<string> m_names;
	std::string m_currentCity;
    // -----------------------------------------------------------------------------
    Timeline* m_timeline;

    DBQueryData m_dbQueryData;

    float timeNow, timeThen;
    double timeSum;
    float fpsToShow;

    std::vector<CurrentImageData > m_imageList;
    std::vector<ofImage > m_images;
    bool m_imageAsCurrentPoint;

    bool m_hideCursor;

    bool m_interactiveMode;

    std::vector<std::string > m_sqlFilePaths;

    bool m_showKeyCommands;

    bool m_showInfo;

    bool m_loopMode;

    bool m_multiMode;
    bool m_multiModeInfo;

    std::vector<LocationImage*> m_locationImgs;

    bool m_pause;
    MagicBox* m_magicBox;

    std::vector<ofSoundPlayer*> m_soundPlayer;

    bool m_isZoomAnimation;

    Integrator* m_zoomIntegrator;
    Integrator* m_integratorX;
    Integrator* m_integratorY;

};

#endif // _DRAWINGLIFEAPP_H_
