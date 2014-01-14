/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#include "DrawingLifeApp.h"
#include "GpsData.h"
#include <vector>
#include <fstream>

#include "DataLoader.h"
#include "ViewHelper.h"

#if defined (WIN32)
#undef max
#endif

//const char* DrawingLifeApp::settingsPath = "AppSettings.xml";
//--------------------------------------------------------------
DrawingLifeApp::DrawingLifeApp(std::string settingsFile) :
    m_settingsFile(settingsFile),
    m_settings(0),
    m_isFullscreen(false),
    m_isDebugMode(false),
    m_isAnimation(true),
    m_showFps(false),
    m_startScreenMode(false),
    m_numPersons(0),
    m_timeline(0),
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
}
DrawingLifeApp::~DrawingLifeApp()
{
    m_gpsDatas.clear();
    m_walks.clear();
    m_magicBoxes.clear();

    m_images.clear();
}

void DrawingLifeApp::setup()
{
    // -----------------------------------------------------------------------------
//    AppSettings& settings = AppSettings::instance();

    ofSetWindowTitle("drawinglife");

    m_settings.reset(new AppSettings(m_settingsFile));

    if (!m_settings->initialized())
    {
        ofLogError(AppLogTag::APP) << "Could not load settings" << endl;
        OF_EXIT_APP(1);
    }

    ofSetLogLevel(ofLogLevel(m_settings->getLogLevel()));

    // -------------------------------------------------------------------------
	// Fonts.
    // -------------------------------------------------------------------------

    DrawingLifeFontMap::const_iterator it = m_settings->getFonts().begin();
    DrawingLifeFontMap::const_iterator itEnd = m_settings->getFonts().end();
    for (; it != itEnd; ++it)
    {
        string fontId = it->first;
        const pair<string, int>& f = it->second;
        string fontName = f.first;
        int fontSize = f.second;
        ofTrueTypeFont font;
        font.loadFont(fontName, fontSize);
        m_fonts[fontId] = font;
    }

    // -----------------------------------------------------------------------------
	// Settings.
	// -----------------------------------------------------------------------------
    m_isDebugMode = m_settings->isDebugMode();
    m_isFullscreen = m_settings->isFullscreen();
    m_hideCursor = m_settings->hideCursor();
    m_showInfo = m_settings->showInfo();
    m_interactiveMode = m_settings->isInteractiveMode();
    m_loopMode = m_settings->isLoopOn();

    m_multiMode = m_settings->isMultiMode();
    m_multiModeInfo = m_settings->isMultiModeInfo();

	// -----------------------------------------------------------------------------
	// Database.
	// -----------------------------------------------------------------------------
    m_dbQueryData.type = m_settings->getQueryType();
    m_dbQueryData.yearStart = m_settings->getQueryYearStart();
    m_dbQueryData.yearEnd = m_settings->getQueryYearEnd();
    m_dbQueryData.city = m_settings->getQueryCity();

	// -----------------------------------------------------------------------------
	// Visual m_settings->
	// -----------------------------------------------------------------------------
    m_imageAsCurrentPoint = m_settings->isCurrentPointImage();

    if (m_imageAsCurrentPoint)
    {
        m_imageList = m_settings->getCurrentImageData();
    }

    m_numPersons = m_settings->getNumPersons();
    m_names = m_settings->getNames();

    for (size_t i = 0; i < m_numPersons; ++i)
    {
        m_viewAspectRatioData.offset.push_back(ofxPoint<double>(0, 0));
        m_viewAspectRatioData.minDimension.push_back(0);
        m_viewAspectRatioData.padding.push_back(15);
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

//    int drSp = m_settings->getDrawSpeed();

    double damp = m_settings->getZoomAnimationDamp();
    double attr = m_settings->getZoomAnimationAttraction();
    double dampCenter = m_settings->getZoomAnimationDampCenter();
    double attrCenter = m_settings->getZoomAnimationAttractionCenter();

    m_zoomIntegrator.reset(new Integrator<double>(0.0f, damp, attr));
    m_isZoomAnimation = m_settings->isZoomAnimation();
    m_theIntegrator.reset(new Integrator<ofxPoint<double> >(0.0f,
                                                            dampCenter,
                                                            attrCenter));

    if ( ! m_isZoomAnimation)
    {

    	//    	m_zoomIntegrator->setTarget();
    }

    // -----------------------------------------------------------------------------

    ViewHelper::setViewAspectRatio(*this);
    // -----------------------------------------------------------------------------
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
            gpsDataAreLoaded = DataLoader::loadGpsDataCity(*this, m_names,
                                                           m_dbQueryData.city);
            break;
            case DBReader::DB_QUERY_YEAR:
            gpsDataAreLoaded = DataLoader::loadGpsDataYearRange(*this, m_names,
                                                                m_dbQueryData.yearStart,
                                                                m_dbQueryData.yearEnd);
            break;
            case DBReader::DB_QUERY_SQLFILE:
            gpsDataAreLoaded = DataLoader::loadGpsDataWithSqlFile(*this,
                                                                  m_sqlFilePaths);
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

            const vector<LocationImageData>& locImgVec =
                    m_settings->getLocationImageData();
            BOOST_FOREACH(const LocationImageData& locImgData, locImgVec)
            {
                LocationImage* lImg;
                if (m_multiMode)
                {
                    lImg = new LocationImage(*m_magicBox.get(), locImgData);
                }
                else
                {
                    lImg = new LocationImage(*m_magicBoxes[0].get(), locImgData);
                }

                lImg->setViewBounds(m_viewAspectRatioData.offset[0],
                                    m_viewAspectRatioData.minDimension[0],
                                    m_viewAspectRatioData.padding[0]);

                m_locationImgs.push_back(lImg);
            }
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
        BOOST_FOREACH(const std::string& sndFilePath, m_settings->getSoundFiles())
    	{
    	    ofSoundPlayer* sndPlay = new ofSoundPlayer();
            sndPlay->loadSound(sndFilePath);
    	    sndPlay->setLoop(false);
    	    m_soundPlayer.push_back(sndPlay);
		}
    }

}
int currentSoundFile = 0;
void DrawingLifeApp::soundUpdate()
{
    if (m_settings->isSoundActive() && m_soundPlayer.size() > 0)
	{
        if (m_timeline->isFirst())
		{
			currentSoundFile = 0;
            std::for_each(m_soundPlayer.begin(), m_soundPlayer.end(),
                          boost::bind(&ofSoundPlayer::stop, _1));
            if (m_soundPlayer.size() > 0)
			{
                m_soundPlayer[currentSoundFile].play();
			}
		}
		else
		{
            if ( ! m_soundPlayer[currentSoundFile].getIsPlaying())
			{
                m_soundPlayer[currentSoundFile].stop();
                m_soundPlayer[currentSoundFile].setPosition(0.0);
                if (currentSoundFile < static_cast<int>(m_soundPlayer.size()) - 1)
				{
					++currentSoundFile;
				}
				else
				{
					currentSoundFile = 0;
				}
                m_soundPlayer[currentSoundFile].play();
			}
		}
	}
}

int zoomFrameCount = 0;
bool DrawingLifeApp::zoomHasChanged()
{
    if (m_timeline->isFirst())
	{
		zoomFrameCount = 0;
		return true;
	}

    switch (m_settings->getZoomAnimationCriteria())
    {
		case 1:
			return zoomHasChangedTime();
		case 2:
			return zoomHasChangedId();
		case 3:
			return zoomHasChangedTimestamp();
		default:
			return false;
	}
}

bool DrawingLifeApp::zoomHasChangedId()
{
    if (zoomFrameCount + 1 >= static_cast<int>(m_settings->getZoomAnimFrames().size()))
    {
		return false;
    }
	int currentId = m_timeline->getCurrentTimelineObj().gpsid;
	int zoomChangeId = m_settings->getZoomAnimFrames()[zoomFrameCount+1].gpsId;
    if (currentId == zoomChangeId)
    {
		++zoomFrameCount;
		return true;
    }
    else
    {
		return false;
	}
}

bool DrawingLifeApp::zoomHasChangedTimestamp()
{
    if (zoomFrameCount+1 >=  static_cast<int>(m_settings->getZoomAnimFrames().size()))
    {
		return false;
    }
	string currentTimestamp = m_timeline->getCurrentTimelineObj().timeString;
	string zoomChangeTimestamp = m_settings->getZoomAnimFrames()[zoomFrameCount+1].timestamp;
    if (zoomChangeTimestamp.compare(currentTimestamp) == 0)
    {
		++zoomFrameCount;
		return true;
    }
    else
    {
		return false;
	}
}

bool DrawingLifeApp::zoomHasChangedTime()
{
	int current = m_timeline->getCurrentCount();
	int all = m_timeline->getAllCount();

	int currIndex = 0;
    for (size_t i = 0; i < m_settings->getZoomAnimFrames().size(); ++i)
	{
        if ((current / (float)all) > m_settings->getZoomAnimFrames()[i].frameTime)
		{
			currIndex = i;
		}
		else
		{
			break;
		}
	}
    if (zoomFrameCount != currIndex)
	{
		++zoomFrameCount;
        if (zoomFrameCount >=  static_cast<int>(m_settings->getZoomAnimFrames().size()))
        {
			--zoomFrameCount;
        }
		return true;
	}
	else
	{
		return false;
	}
}

void DrawingLifeApp::zoomUpdate()
{
    if (m_isZoomAnimation)
	{
        if (zoomHasChanged())
		{
            const ZoomAnimFrame& zoomAnimFrame =
                    m_settings->getZoomAnimFrames()[zoomFrameCount];
            float zoomLevel = zoomAnimFrame.frameZoom;
            double centerX = zoomAnimFrame.frameCenterX;
            double centerY = zoomAnimFrame.frameCenterY;
            UtmPoint utmP = GpsData::getUtmPointWithRegion(centerY, centerX,
                                                           *m_settings);

            if (m_timeline->isFirst())
			{
                m_zoomIntegrator->set(zoomLevel);
                m_theIntegrator->set(utmP);
			}
            m_zoomIntegrator->setTarget(zoomLevel);
            m_theIntegrator->setTarget(utmP);

		}
	}
    m_zoomIntegrator->update();
    m_theIntegrator->update();

    if (m_zoomIntegrator->isTargeting() || m_theIntegrator->isTargeting())
    {
        if (m_multiMode)
        {
            m_magicBox->setSize(m_zoomIntegrator->getValue());
            m_magicBox->setupBox(m_theIntegrator->getValue(), 0);
        }
        else
        {
            BOOST_FOREACH(MagicBoxPtr box, m_magicBoxes)
            {
                box->setSize(m_zoomIntegrator->getValue());
                box->setupBox(m_theIntegrator->getValue(), 0);
            }
        }
    }
}
//--------------------------------------------------------------
bool firstSleep = true;

void DrawingLifeApp::update()
{
    if (m_isAnimation
            && !m_pause
            && !m_interactiveMode
            && m_timeline->getTimeline().size() > 0)
    {
        if (m_loopMode)
        {
            for (int i = 0; i < m_settings->getDrawSpeed(); ++i)
            {
                int id = m_timeline->getNext();
                if (id < (int)m_numPersons)
                {
                    m_walks[id].update();
                }

                zoomUpdate();
                soundUpdate();

                m_timeline->countUp();
                if (m_timeline->isFirst())
                {
//                            std::cout << "First timeline object!" << std::endl;
                    std::for_each(m_walks.begin(), m_walks.end(),
                                  boost::bind(&Walk::reset, _1));
                    zoomFrameCount = 0;
#if defined (TARGET_WIN32)
                    Sleep(m_settings->getSleepTime()*1000);
#else
                    sleep(m_settings->getSleepTime());
#endif
                }
            }
        }
        else
        {
            for (int i = 0; i < m_settings->getDrawSpeed(); ++i)
            {
                if (!m_timeline->isLast())
                {
                    int id = m_timeline->getNext();
                    m_walks[id].update();
                    m_timeline->countUp();
                }
                else
                {
                    OF_EXIT_APP(0);
                }
            }
        }
    }
}

//--------------------------------------------------------------
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

void DrawingLifeApp::shaderEnd()
{
    shader.end();
}

//--------------------------------------------------------------
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
            std::for_each(m_locationImgs.begin(), m_locationImgs.end(),
                          boost::bind(&LocationImage::draw, _1));

            if (m_multiMode && m_multiModeInfo)
            {
                ofSetColor(255, 255, 255, m_settings->getAlphaLegend());
                ofSetHexColor(0xffffff);
                std::string infoText = m_timeline->getCurrentTime();
//                if (m_pause)
//                    infoText.append(" (stopped)");
                m_fonts["info"].drawString(infoText,
                                           m_viewAspectRatioData.padding[0],
                                           m_viewAspectRatioData.offset[0].y + 10);

            }

            for (size_t i = 0; i < m_numPersons; ++i)
            {
                const GpsDataPtr& gpsData = m_gpsDatas[i];
                Walk& walk = m_walks[i];
                if (m_isDebugMode)
                {
                    const MagicBoxPtr& box = m_settings->isMultiMode()
                            ? m_magicBox : m_magicBoxes[i];
                    ofSetColor(255, 255, 255, m_settings->getAlphaLegend());

                    int debugTextX = 30 + (ofGetWidth() / m_numPersons) * i;
                    int debugTextY = 30;
                    ofDrawBitmapString(Utils::getCurrentGpsInfoDebug(
                                           gpsData.get(), &walk, box.get()),
                                       debugTextX, debugTextY);
                }
                else if (m_showInfo)
                {
                    ofSetColor(255, 255, 255, m_settings->getAlphaLegend());
                    ofSetHexColor(0xffffff);
                    std::string infoText = Utils::getCurrentGpsInfo(
                                gpsData.get(), &walk);
//                    if (m_pause)
//                        infoText.append(" (stopped)");
                    int infoX = m_viewAspectRatioData.padding[i]
                            + (ofGetWidth() / m_numPersons) * i;
                    int infoY = m_viewAspectRatioData.offset[i].y + 10;
                    m_fonts["info"].drawString(infoText, infoX, infoY);
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
            for (size_t i = 0; i < m_numPersons; ++i)
            {
                m_walks[i].drawAll();
            }
        }

        if (m_showFps)
        {
            ViewHelper::drawFPS();
        }

        if (m_showKeyCommands)
        {
            ofColor c(255, 255, 255, m_settings->getAlphaLegend());
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

// -----------------------------------------------------------------------------
void DrawingLifeApp::resetData()
{
    m_startScreenMode = false;

    m_gpsDatas.clear();
    m_walks.clear();
    m_magicBoxes.clear();
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
            if (!m_isDebugMode)
                ofHideCursor();
            else
                ofShowCursor();
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

//--------------------------------------------------------------

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------
void DrawingLifeApp::windowResized(int /*w*/, int /*h*/)
{
    ViewHelper::setViewAspectRatio(*this);

    for (size_t i = 0; i < m_numPersons; ++i)
    {
        m_walks[i].setViewBounds(m_viewAspectRatioData.offset[i],
                                 m_viewAspectRatioData.minDimension[i],
                                 m_viewAspectRatioData.padding[i]);
    }

    BOOST_FOREACH(LocationImage& locImg, m_locationImgs)
    {
        locImg.setViewBounds(m_viewAspectRatioData.offset[0],
                m_viewAspectRatioData.minDimension[0],
                m_viewAspectRatioData.padding[0]);
    }
}
