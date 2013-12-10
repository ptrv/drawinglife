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
    bool loadGpsDataCity(const std::vector<std::string>& names,
                         const std::string& city);
	/**
	* \brief
	* \param names vector with name strings.
	* \param yearStart start year for query.
	* \param yearEnd end year for query.
	*/
    bool loadGpsDataYearRange(const std::vector<std::string>& names,
                              int yearStart, int yearEnd);

    bool loadGpsDataWithSqlFile(const std::vector<std::string>& names,
                                const std::vector<std::string>& m_sqlFilePaths);

    typedef boost::function<bool(DBReader*,GpsData&)> tFuncLoadGpsData;
    bool loadGpsData(const std::vector<tFuncLoadGpsData>& funcVec);
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
	bool zoomHasChangedTime();
	bool zoomHasChangedId();
	bool zoomHasChangedTimestamp();

	void zoomUpdate();

	void soundUpdate();

    void shaderBegin();
    void shaderEnd();

    //---------------------------------------------------------------------------
    // Member variables
    //---------------------------------------------------------------------------
    std::string m_settingsFile;

    boost::scoped_ptr<AppSettings> m_settings;

    GpsDataVector m_gpsDatas;
    WalkVector m_walks;
    MagicBoxVector m_magicBoxes;

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
    boost::scoped_ptr<Timeline> m_timeline;

    DBQueryData m_dbQueryData;

    float timeNow, timeThen;
    double timeSum;
    float fpsToShow;

    std::vector<CurrentImageData> m_imageList;
//    std::vector<ofImage > m_images;
    boost::ptr_vector<ofImage> m_images;
    bool m_imageAsCurrentPoint;

    bool m_hideCursor;

    bool m_interactiveMode;

    std::vector<std::string> m_sqlFilePaths;

    bool m_showKeyCommands;

    bool m_showInfo;

    bool m_loopMode;

    bool m_multiMode;
    bool m_multiModeInfo;

    boost::ptr_vector<LocationImage> m_locationImgs;

    bool m_pause;
    boost::scoped_ptr<MagicBox> m_magicBox;

    boost::ptr_vector<ofSoundPlayer> m_soundPlayer;

    bool m_isZoomAnimation;

    boost::scoped_ptr<Integrator<double> > m_zoomIntegrator;
    boost::scoped_ptr<Integrator<ofxPoint<double> > > m_theIntegrator;

    ofShader shader;
    bool doShader;
};

#endif // _DRAWINGLIFEAPP_H_
