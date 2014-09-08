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

class ZoomAnimation;
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

    Timeline& getTimeline() const { return *m_timeline.get(); }
    const AppSettings& getAppSettings() const { return *m_settings.get(); }
    GpsDataVector& getGpsDataVector() { return m_gpsDatas; }
    WalkVector& getWalkVector() { return m_walks; }
    MagicBoxVector& getMagicBoxVector() { return m_magicBoxes; }

    const std::vector<CurrentPointImageData>& getCurrentPointImageList() const
    { return m_imageList; }
    const boost::ptr_vector<ofImage>& getCurrentPointImages() const
    { return m_images; }

    bool getIsImageAsCurrentPoint() const { return m_imageAsCurrentPoint; }
    void setImageAsCurrentPoint(bool isImageAsCurrentPoint)
    { m_imageAsCurrentPoint = isImageAsCurrentPoint; }

    void addCurrentPointImage(ofImage* img) { m_images.push_back(img); }
    void clearCurrentPointImages() { m_images.clear(); }

    void addLocationImageSource(const ofImagePtr& image)
    { m_locationImageSources.push_back(image); }
    void clearLocationImageSources();

    void addLocationImageVec(LocationImageVec& locationImageVector)
    { m_locationImages.push_back(locationImageVector); }
    void clearLocationImageVec();

    void addSoundPlayer(ofSoundPlayer* soundPlayer)
    { m_soundPlayers.push_back(soundPlayer); }
    void clearSoundPlayers() { m_soundPlayers.clear(); }

    void resetData();

    ViewDimensionsVec& getViewDimensionsVec() { return m_viewDimensions; }
    const ViewDimensionsVec& getViewDimensionsVec() const
    { return m_viewDimensions; }

private:
    //---------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------

	void soundUpdate();

    void shaderBegin();
    void shaderEnd();

    void handleFirstTimelineObject();

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
    ViewDimensionsVec m_viewDimensions;
    //---------------------------------------------------------------------------
	bool m_isFullscreen;
	bool m_isDebugMode;
	bool m_isAnimation;
	bool m_showFps;
	//---------------------------------------------------------------------------
	bool m_startScreenMode;

    DrawingLifeFonts m_fonts;
    // -----------------------------------------------------------------------------
    unsigned int m_numPersons;
    StringVec m_names;
	std::string m_currentCity;
    // -----------------------------------------------------------------------------
    boost::shared_ptr<Timeline> m_timeline;

    DBQueryData m_dbQueryData;

    std::vector<CurrentPointImageData> m_imageList;
    boost::ptr_vector<ofImage> m_images;
    bool m_imageAsCurrentPoint;

    bool m_hideCursor;

    bool m_interactiveMode;

    StringVec m_sqlFilePaths;

    bool m_showKeyCommands;

    bool m_showInfo;

    bool m_loopMode;

    bool m_multiMode;
    bool m_multiModeInfo;

    ofImagePtrVec m_locationImageSources;
    std::vector<LocationImageVec> m_locationImages;

    bool m_pause;

    ofSoundPlayerVec m_soundPlayers;

    bool m_isZoomAnimation;

    boost::scoped_ptr<ZoomAnimation> m_zoomAnimation;

    ofShader shader;
    bool doShader;

    static int m_sZoomFrameCount;
    static int m_sCurrentSoundFile;

    boost::function<void(Walk&)> fnWalkDrawAll;
    boost::function<void(Walk&)> fnWalkReset;
    boost::function<void(LocationImage&)> fnLocationImageDraw;

};

#endif // _DRAWINGLIFEAPP_H_
