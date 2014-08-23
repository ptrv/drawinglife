/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#include "DrawingLifeApp.h"
#include "GpsData.h"
#include <vector>
#include <fstream>

#include "DataLoader.h"
#include "ViewHelper.h"
#include "ZoomAnimation.h"

#if defined (WIN32)
#undef max
#endif

//const char* DrawingLifeApp::settingsPath = "AppSettings.xml";

//------------------------------------------------------------------------------

int DrawingLifeApp::m_sZoomFrameCount = 0;
int DrawingLifeApp::m_sCurrentSoundFile = 0;

//------------------------------------------------------------------------------

DrawingLifeApp::DrawingLifeApp(std::string settingsFile) :
    m_settingsFile(settingsFile),
    //m_settings(0),
    m_isFullscreen(false),
    m_isDebugMode(false),
    m_isAnimation(true),
    m_showFps(false),
    m_startScreenMode(false),
    m_numPersons(0),
//    m_timeline(),
//    m_drawSpeed(1),
//	m_trackAlpha(64),
//	m_dotAlpha(127),
//	m_legendAlpha(255),
//	m_startScreenDuration(5000),
//	m_loadOnStart(0),
//	m_frameRate(60),
//	m_magicBoxSize(3000.0),
//	m_magicBoxPadding(500.0),
//	m_maxPointsToDraw(10000),
	m_imageAsCurrentPoint(false),
	m_hideCursor(false),
	m_interactiveMode(false),
	m_showKeyCommands(false),
	m_showInfo(true),
    m_loopMode(true),
    m_multiMode(false),
    m_multiModeInfo(false),
    m_pause(false)
//    m_magicBox(0)
{
    fnWalkDrawAll = boost::bind(&Walk::drawAll, _1);
    fnWalkReset = boost::bind(&Walk::reset, _1);
    fnLocationImageDraw = boost::bind(&LocationImage::draw, _1);
}

//------------------------------------------------------------------------------

DrawingLifeApp::~DrawingLifeApp()
{
    m_gpsDatas.clear();
    m_walks.clear();
    m_magicBoxes.clear();

    m_images.clear();

    clearLocationImageSources();
    clearLocationImageVec();
}

//------------------------------------------------------------------------------

void DrawingLifeApp::setup()
{
    ofSetWindowTitle("drawinglife");

    m_settings.reset(new AppSettings(m_settingsFile));

    if (!m_settings->initialized())
    {
        ofLogError(Logger::APP) << "Could not load settings" << endl;
        OF_EXIT_APP(1);
    }

    ofSetLogLevel(ofLogLevel(m_settings->getLogLevel()));

    // -------------------------------------------------------------------------
	// Fonts.
    // -------------------------------------------------------------------------

    DataLoader::loadFonts(*this, m_fonts);

    // -------------------------------------------------------------------------
	// Settings.
    // -------------------------------------------------------------------------
    m_isDebugMode = m_settings->isDebugMode();
    m_isFullscreen = m_settings->isFullscreen();
    m_hideCursor = m_settings->hideCursor();
    m_showInfo = m_settings->showInfo();
    m_interactiveMode = m_settings->isInteractiveMode();
    m_loopMode = m_settings->isLoopOn();

    m_multiMode = m_settings->isMultiMode();
    m_multiModeInfo = m_settings->isMultiModeInfo();

    // -------------------------------------------------------------------------
	// Database.
    // -------------------------------------------------------------------------
    m_dbQueryData.type = m_settings->getQueryType();
    m_dbQueryData.yearStart = m_settings->getQueryYearStart();
    m_dbQueryData.yearEnd = m_settings->getQueryYearEnd();
    m_dbQueryData.city = m_settings->getQueryCity();

    // -------------------------------------------------------------------------
	// Visual m_settings->
    // -------------------------------------------------------------------------
    m_imageAsCurrentPoint = m_settings->isCurrentPointImage();

    if (m_imageAsCurrentPoint)
    {
        m_imageList = m_settings->getCurrentImageData();
    }

    m_numPersons = m_settings->getNumPersons();
    m_names = m_settings->getNames();

    for (size_t i = 0; i < m_numPersons; ++i)
    {
        ViewDimensions viewDimensions;
        viewDimensions.offset = ofxPoint<double>(0, 0);
        viewDimensions.minDimension = 0;
        viewDimensions.padding = 15;
        m_viewDimensions.push_back(viewDimensions);
    }

    m_dbPath = m_settings->getDatabasePath();

    ofBackground(m_settings->getColorBackgroundR(),
                 m_settings->getColorBackgroundG(),
                 m_settings->getColorBackgroundB());

    ofSetFrameRate(m_settings->getFrameRate());

    ofEnableAlphaBlending();

    ofSetVerticalSync(false);
    string shaderDir = "shaders/";
    string vertSrc = shaderDir + m_settings->getVertexShaderSource().c_str();
    string fragSrc = shaderDir + m_settings->getFragmentShaderSource().c_str();
    shader.load(vertSrc, fragSrc);
    doShader = m_settings->useShader();

    m_isZoomAnimation = m_settings->isZoomAnimation();

    // -------------------------------------------------------------------------

    ViewHelper::setViewAspectRatio(*this);
    // -------------------------------------------------------------------------
    m_imageAsCurrentPoint = DataLoader::loadCurrentPointImages(*this);

    m_timeline.reset(new Timeline());

    m_sqlFilePaths = m_settings->getSqlFilePaths();

    if (m_settings->loadGpsOnStart())
    {
		bool gpsDataAreLoaded = false;
        switch(m_dbQueryData.type)
        {
			case DBReader::DB_QUERY_DAY:
			// TODO 101028_1700_TP: Not implemented yet.
			break;
			case DBReader::DB_QUERY_MONTH:
			// TODO 101028_1701_TP: Not implemented yet.
			break;
            case DBReader::DB_QUERY_CITY:
            gpsDataAreLoaded = DataLoader::loadGpsDataCity(
                *this, m_names, m_dbQueryData.city);
            break;
            case DBReader::DB_QUERY_YEAR:
            gpsDataAreLoaded = DataLoader::loadGpsDataYearRange(
                *this, m_names, m_dbQueryData.yearStart, m_dbQueryData.yearEnd);
            break;
            case DBReader::DB_QUERY_SQLFILE:
            gpsDataAreLoaded = DataLoader::loadGpsDataWithSqlFile(
                *this, m_sqlFilePaths);
			break;
        }
        if (gpsDataAreLoaded)
        {
			// GpsData are loaded now. Drawing routine can start.
            for (size_t personIndex = 0; personIndex < m_numPersons; ++personIndex)
	        {
                const GpsDataPtr gpsData = m_gpsDatas[personIndex];
                if (gpsData->getTotalGpsPoints() == 0)
                {
                    m_startScreenMode = true;
                    break;
                }
            }

            DataLoader::loadLocationImages(*this);

            m_zoomAnimation.reset(new ZoomAnimation(*m_settings,
                                                    m_timeline));
        }
    }
    else
    {
        m_startScreenMode = true;
    }

    ofSetFullscreen(m_isFullscreen);

    if (m_hideCursor)
    {
        ofHideCursor();
    }

    if (m_settings->isSoundActive())
    {
        DataLoader::loadSoundPlayers(*this);
    }

}


//------------------------------------------------------------------------------

bool firstRun = true;
void DrawingLifeApp::handleFirstTimelineObject()
{
    if (m_timeline->isFirst())
    {
        if (firstRun)
        {
            firstRun = false;
        }
        else
        {
            if (m_loopMode)
            {
                std::for_each(m_walks.begin(), m_walks.end(), fnWalkReset);
                m_sZoomFrameCount = 0;
                const int sleepTime = m_settings->getSleepTime();
                if (sleepTime > 0)
                {
#if defined (TARGET_WIN32)
                    Sleep(sleepTime*1000);
#else
                    sleep(sleepTime);
#endif
                }
            }
            else
            {
                OF_EXIT_APP(0);
            }
        }
    }
}

void DrawingLifeApp::update()
{
    if (m_isAnimation &&
        !m_pause &&
        !m_interactiveMode &&
        m_timeline->getTimeline().size() > 0)
    {
        for (int i = 0; i < m_settings->getDrawSpeed(); ++i)
        {
            handleFirstTimelineObject();

            const int id = m_timeline->getCurrentId();
            try
            {
                m_walks.at(id).update();
            }
            catch (const std::out_of_range&) {}

            m_zoomAnimation->update(*this);
            soundUpdate();

            m_timeline->countUp();
        }
    }
}

//------------------------------------------------------------------------------

void DrawingLifeApp::draw()
{
    if (m_startScreenMode)
    {
        ViewHelper::drawStartScreen(m_fonts["title"], m_fonts["author"]);
    }
    else
    {
        ViewHelper::fillViewAreaUTM(*this);
		// -----------------------------------------------------------------------------
		// Draw Gps data as an animation.
		// -----------------------------------------------------------------------------
        if (m_isAnimation)
        {
            // -----------------------------------------------------------------------------
            // Draw rectangle with text.
            // -----------------------------------------------------------------------------
//            fillViewAreaUTM();
            //---------------------------------------------------------------------------

            BOOST_FOREACH(LocationImageVec& locVec, m_locationImages)
            {
                std::for_each(locVec.begin(), locVec.end(), fnLocationImageDraw);
            }

            if (m_multiMode && m_multiModeInfo)
            {
                ViewHelper::drawInfoMultiMode(*m_settings.get(),
                                              *m_timeline.get(),
                                              m_viewDimensions[0],
                                              m_fonts["info"]);
            }

            for (size_t i = 0; i < m_numPersons; ++i)
            {
                const GpsDataPtr& gpsData = m_gpsDatas[i];
                Walk& walk = m_walks[i];
                if (m_isDebugMode)
                {
                    const MagicBoxPtr& box = m_settings->isMultiMode()
                            ? m_magicBox : m_magicBoxes[i];
                    ViewHelper::drawInfoDebug(*m_settings.get(), *box.get(),
                                              *gpsData.get(), walk, i);
                }
                else if (m_showInfo)
                {
                    ViewHelper::drawInfo(*m_settings.get(), *gpsData.get(), walk,
                                         m_viewDimensions[i], m_fonts["info"], i);
                }

                // -----------------------------------------------------------------------------
                // Draw Gps data
                // -----------------------------------------------------------------------------
                if (doShader)
                {
                    shaderBegin();
                }
                ofSetColor(m_settings->getColorForegroundR(),
                           m_settings->getColorForegroundG(),
                           m_settings->getColorForegroundB(),
                           m_settings->getAlphaTrack());
                ofNoFill();
                walk.draw();
                if (doShader)
                {
                    shaderEnd();
                }
            }
        }
		// -----------------------------------------------------------------------------
		// Draw all Gps data at once. No animation.
		// -----------------------------------------------------------------------------
        else
        {
//            fillViewAreaUTM();
            // -----------------------------------------------------------------------------
            // Draw Gps data
            // -----------------------------------------------------------------------------
            ofSetColor(m_settings->getColorForegroundR(),
                       m_settings->getColorForegroundG(),
                       m_settings->getColorForegroundB(),
                       m_settings->getAlphaTrack());
            ofNoFill();

            std::for_each(m_walks.begin(), m_walks.end(), fnWalkDrawAll);
        }

        if (m_showFps)
        {
            ViewHelper::drawFPS();
        }

        if (m_showKeyCommands)
        {
            const ofColor c(255, 255, 255, m_settings->getAlphaLegend());
            ViewHelper::drawKeyCommands(c);
        }

        if (m_timeline->isLast())
        {

        }
    }

    if (m_settings->getIsGrabScreen())
    {
        Utils::grabScreen();
    }
}

//------------------------------------------------------------------------------
// Input
// TODO Add function for processing keys
//------------------------------------------------------------------------------
void DrawingLifeApp::keyPressed  (int key)
{
    switch (key)
    {
    case 'a':
        m_isAnimation = !m_isAnimation;
        break;
    case 'f':
        m_isFullscreen = !m_isFullscreen;
        ofSetFullscreen(m_isFullscreen);
        break;
    case 'd':
        m_isDebugMode = !m_isDebugMode;
        if (m_hideCursor)
        {
            m_isDebugMode ? ofShowCursor() : ofHideCursor();
        }
        break;
    case 'p':
        m_showFps = !m_showFps;
        break;
    case 'k':
        m_showKeyCommands = !m_showKeyCommands;
        break;
//    case 'c':
//        for (unsigned int i = 0; i < m_walks.size(); ++i)
//        {
//            m_walks[i]->setDotColors();
//        }
//        break;
//    case 49:
//    if (m_dbQueryData.type == DBReader::DB_QUERY_CITY)
//    {
//        loadGpsDataCity(m_names, "Berlin");
//    }
//    else if (m_dbQueryData.type != DBReader::DB_QUERY_CITY)
//    {
//        for (unsigned int i = 0; i < m_magicBoxes.size(); ++i)
//        {
//            m_magicBoxes[i]->toggleZoomLevel(1);
//        }
//        if (m_multiMode)
//            m_magicBox->toggleZoomLevel(1);
//    }
//    break;
//    case 50:
//    if (m_dbQueryData.type == DBReader::DB_QUERY_CITY)
//    {
//        loadGpsDataCity(m_names, "London");
//    }
//    else
//    {
//        for (unsigned int i = 0; i < m_magicBoxes.size(); ++i)
//        {
//            m_magicBoxes[i]->toggleZoomLevel(2);
//        }
//        if (m_multiMode)
//            m_magicBox->toggleZoomLevel(2);
//    }
//    break;
//    case 51:
//    if (m_dbQueryData.type == DBReader::DB_QUERY_CITY)
//    {
//        loadGpsDataCity(m_names, "Barcelona");
//    }
//    else
//    {
//        for (unsigned int i = 0; i < m_magicBoxes.size(); ++i)
//        {
//            m_magicBoxes[i]->toggleZoomLevel(3);
//        }
//        if (m_multiMode)
//            m_magicBox->toggleZoomLevel(3);
//    }
//    break;
//    case 52:
//    if (m_dbQueryData.type == DBReader::DB_QUERY_CITY)
//    {
//        loadGpsDataCity(m_names, "Hamburg");
//    }
//    else
//    {
//        for (unsigned int i = 0; i < m_magicBoxes.size(); ++i)
//        {
//            m_magicBoxes[i]->toggleZoomLevel(4);
//        }
//        if (m_multiMode)
//            m_magicBox->toggleZoomLevel(4);
//    }
//    break;
//    case 53:
//    if (m_dbQueryData.type == DBReader::DB_QUERY_CITY)
//        loadGpsDataCity(m_names, "Vienna");
//        break;
//    case 54:
//    if (m_dbQueryData.type == DBReader::DB_QUERY_CITY)
//        loadGpsDataCity(m_names,"New York");
//        break;
//    case 55:
//    if (m_dbQueryData.type == DBReader::DB_QUERY_CITY)
//        loadGpsDataCity(m_names, "Tokyo");
//        break;
//    case 56:
//    if (m_dbQueryData.type == DBReader::DB_QUERY_CITY)
//        loadGpsDataCity(m_names, "San Francisco");
//        break;
//    case 57:
//    if (m_dbQueryData.type == DBReader::DB_QUERY_CITY)
//        loadGpsDataCity(m_names, "Bristol");
//        break;
//    case 48:
//    if (m_dbQueryData.type == DBReader::DB_QUERY_CITY)
//        loadGpsDataCity(m_names, "Banff");
//        break;
    case '+':
        std::for_each(m_magicBoxes.begin(), m_magicBoxes.end(),
                      boost::bind(&MagicBox::zoom, _1, MagicBox::ZOOM_IN));
        if (m_multiMode)
            m_magicBox->zoom(MagicBox::ZOOM_IN);
        break;
    case '-':
        std::for_each(m_magicBoxes.begin(), m_magicBoxes.end(),
                      boost::bind(&MagicBox::zoom, _1, MagicBox::ZOOM_OUT));
        if (m_multiMode)
            m_magicBox->zoom(MagicBox::ZOOM_OUT);
        break;
    case OF_KEY_UP:
        std::for_each(m_magicBoxes.begin(), m_magicBoxes.end(),
                      boost::bind(&MagicBox::move, _1, MagicBox::UP));
        if (m_multiMode)
            m_magicBox->move(MagicBox::UP);
        break;
    case OF_KEY_DOWN:
        std::for_each(m_magicBoxes.begin(), m_magicBoxes.end(),
                      boost::bind(&MagicBox::move, _1, MagicBox::DOWN));
        if (m_multiMode)
            m_magicBox->move(MagicBox::DOWN);
        break;
       break;
    case OF_KEY_RIGHT:
        std::for_each(m_magicBoxes.begin(), m_magicBoxes.end(),
                      boost::bind(&MagicBox::move, _1, MagicBox::RIGHT));
        if (m_multiMode)
            m_magicBox->move(MagicBox::RIGHT);
        break;
    case OF_KEY_LEFT:
        std::for_each(m_magicBoxes.begin(), m_magicBoxes.end(),
                      boost::bind(&MagicBox::move, _1, MagicBox::LEFT));
        if (m_multiMode)
            m_magicBox->move(MagicBox::LEFT);

        break;
    case ' ':
        if (m_interactiveMode)
        {
            std::for_each(m_walks.begin(), m_walks.end(),
                          boost::bind(&Walk::updateToNextSegment, _1));
        }
        else
        {
            m_pause = !m_pause;
        }
        break;
    case OF_KEY_BACKSPACE:
        if (m_interactiveMode)
        {
            std::for_each(m_walks.begin(), m_walks.end(),
                          boost::bind(&Walk::updateToPreviousSegment, _1));
        }
        break;
    case 't':
        if (m_interactiveMode)
        {
            std::for_each(m_walks.begin(), m_walks.end(),
                          boost::bind(&Walk::toggleTraced, _1));
        }
        break;
    case 's':
        doShader = !doShader;
        break;
    default:
        break;
    }
}

//--------------------------------------------------------------
void DrawingLifeApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void DrawingLifeApp::mouseMoved(int x, int y )
{

}

//--------------------------------------------------------------
void DrawingLifeApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void DrawingLifeApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------

void DrawingLifeApp::mouseReleased(int x, int y, int button)
{

}

//------------------------------------------------------------------------------

void DrawingLifeApp::windowResized(int /*w*/, int /*h*/)
{
    ViewHelper::setViewAspectRatio(*this);

    for (size_t i = 0; i < m_numPersons; ++i)
    {
        m_walks[i].setViewBounds(m_viewDimensions[i]);
    }

    BOOST_FOREACH(LocationImageVec& locVec, m_locationImages)
    {
        if (m_settings->isMultiMode())
        {
            locVec[0].setViewBounds(m_viewDimensions[0]);
        }
        else
        {
            for (size_t i = 0; i < locVec.size(); ++i)
            {
                locVec[i].setViewBounds(m_viewDimensions[i]);
            }
        }
    }
}

//------------------------------------------------------------------------------
// Helpers
//------------------------------------------------------------------------------

void DrawingLifeApp::clearLocationImageSources()
{
    BOOST_FOREACH(ofImagePtr image, m_locationImageSources)
    {
        image.reset();
    }
    m_locationImageSources.clear();
}

//------------------------------------------------------------------------------

void DrawingLifeApp::clearLocationImageVec()
{
    std::for_each(m_locationImages.begin(), m_locationImages.end(),
                  boost::bind(&LocationImageVec::clear, _1));
}

//------------------------------------------------------------------------------

void DrawingLifeApp::resetData()
{
    m_startScreenMode = false;

    m_gpsDatas.clear();
    m_walks.clear();
    m_magicBoxes.clear();
}

//------------------------------------------------------------------------------
// Private functions
//------------------------------------------------------------------------------

void DrawingLifeApp::soundUpdate()
{
    if (m_settings->isSoundActive() && m_soundPlayers.size() > 0)
    {
        if (m_timeline->isFirst())
        {
            m_sCurrentSoundFile = 0;
            std::for_each(m_soundPlayers.begin(), m_soundPlayers.end(),
                          boost::bind(&ofSoundPlayer::stop, _1));
            if (m_soundPlayers.size() > 0)
            {
                m_soundPlayers[m_sCurrentSoundFile].play();
            }
        }
        else if (!m_soundPlayers[m_sCurrentSoundFile].getIsPlaying())
        {
            ofSoundPlayer& player = m_soundPlayers[m_sCurrentSoundFile];
            player.stop();
            player.setPosition(0.0);
            if (m_sCurrentSoundFile < static_cast<int>(m_soundPlayers.size()) - 1)
            {
                ++m_sCurrentSoundFile;
            }
            else
            {
                m_sCurrentSoundFile = 0;
            }

            m_soundPlayers[m_sCurrentSoundFile].play();
        }
    }
}

//------------------------------------------------------------------------------

void DrawingLifeApp::shaderBegin()
{
    shader.begin();
    //we want to pass in some varrying values to animate our type / color
    shader.setUniform1f("timeValX", ofGetElapsedTimef() * 0.1 );
    shader.setUniform1f("timeValY", -ofGetElapsedTimef() * 0.18 );

    //we also pass in the mouse position
    //we have to transform the coords to what the shader is expecting which is 0,0 in the center and y axis flipped.
    shader.setUniform2f("mouse", mouseX - ofGetWidth()/2, ofGetHeight()/2-mouseY );
}

//------------------------------------------------------------------------------

void DrawingLifeApp::shaderEnd()
{
    shader.end();
}

//------------------------------------------------------------------------------
