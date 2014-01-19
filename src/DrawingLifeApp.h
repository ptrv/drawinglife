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

    Timeline& getTimeline() const { return *m_timeline.get(); }
    const AppSettings& getAppSettings() const { return *m_settings.get(); }
    GpsDataVector& getGpsDataVector() { return m_gpsDatas; }
    WalkVector& getWalkVector() { return m_walks; }
    MagicBoxVector& getMagicBoxVector() { return m_magicBoxes; }
    MagicBox& getMagicBox() { return *m_magicBox.get(); }
    void setMagicBox(MagicBox* const magicBox) { m_magicBox.reset(magicBox); }

    const std::vector<CurrentPointImageData>& getCurrentPointImageList() const
    { return m_imageList; }
    const boost::ptr_vector<ofImage>& getCurrentPointImages() const
    { return m_images; }

    bool getIsImageAsCurrentPoint() const { return m_imageAsCurrentPoint; }
    void setImageAsCurrentPoint(bool isImageAsCurrentPoint)
    { m_imageAsCurrentPoint = isImageAsCurrentPoint; }

    void addCurrentPointImage(ofImage* img) { m_images.push_back(img); }
    void clearCurrentPointImages() { m_images.clear(); }

    void addLocationOfImage(const ofImagePtr& images)
    { m_locationOfImages.push_back(images); }
    void clearLocationOfImages();

    void addLocationImageVec(LocationImageVec& locationImageVector)
    { m_locationImgs.push_back(locationImageVector); }
    void clearLocationImageVec();

    void addSoundPlayer(ofSoundPlayer* soundPlayer)
    { m_soundPlayers.push_back(soundPlayer); }
    void clearSoundPlayers() { m_soundPlayers.clear(); }

    void resetData();

    ViewDimensionsVec& getViewDimensionsVec() { return m_viewDimensions; }

private:
    //---------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------

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
    boost::scoped_ptr<Timeline> m_timeline;

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

    ofImagePtrVec m_locationOfImages;
    std::vector<boost::ptr_vector<LocationImage> > m_locationImgs;

    bool m_pause;
    MagicBoxPtr m_magicBox;

    ofSoundPlayerVec m_soundPlayers;

    bool m_isZoomAnimation;

    boost::scoped_ptr<Integrator<double> > m_zoomIntegrator;
    boost::scoped_ptr<Integrator<ofxPoint<double> > > m_theIntegrator;

    ofShader shader;
    bool doShader;

};

#endif // _DRAWINGLIFEAPP_H_
