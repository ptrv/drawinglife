/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#include "DrawingLifeApp.h"
#include "GpsData.h"
#include <vector>
#include <limits>
#include <fstream>

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
    m_zoomX(0.0),
    m_zoomY(0.0),
    m_zoomZ(0.0),
    m_startScreenMode(false),
    m_numPerson(0),
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
	timeNow(0.0f),
	timeThen(0.0f),
	timeSum(0.0),
	fpsToShow(0.0),
//	m_maxPointsToDraw(10000),
	m_imageAsCurrentPoint(false),
	m_hideCursor(false),
	m_interactiveMode(false),
	m_showKeyCommands(false),
	m_showInfo(true),
    m_loopMode(true),
    m_multiMode(false),
    m_multiModeInfo(false),
    m_pause(false),
    m_magicBox(0)
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
        ofLog() << "Could not load settings" << endl;
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

    if(m_imageAsCurrentPoint)
    {
        m_imageList = m_settings->getCurrentImageData();
    }

    m_numPerson = m_settings->getNumPerson();
    m_names = m_settings->getNames();

    for(size_t i = 0; i < m_numPerson; ++i)
    {
        m_viewXOffset.push_back(0);
        m_viewYOffset.push_back(0);
        m_viewMinDimension.push_back(0);
        m_viewPadding.push_back(15);
    }

    m_dbPath = m_settings->getDatabasePath();
    DBG_VAL(m_dbPath);

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

    DBG_VAL((ofToString(damp) + " " + ofToString(attr)));
    m_zoomIntegrator.reset(new Integrator<double>(0.0f, damp, attr));
    m_isZoomAnimation = m_settings->isZoomAnimation();
    m_theIntegrator.reset(new Integrator<ofxPoint<double> >(0.0f,
                                                            dampCenter,
                                                            attrCenter));

    if( ! m_isZoomAnimation)
    {

    	//    	m_zoomIntegrator->setTarget();
    }

    DBG_VAL(m_numPerson);
    // -----------------------------------------------------------------------------

    this->setViewAspectRatio();
    // -----------------------------------------------------------------------------
    loadCurrentPointImages();

    m_timeline.reset(new Timeline());

    m_sqlFilePaths = m_settings->getSqlFilePaths();

    if (m_settings->loadGpsOnStart() == 1)
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
            gpsDataAreLoaded = loadGpsDataCity(m_names, m_dbQueryData.city);
            break;
            case DBReader::DB_QUERY_YEAR:
            gpsDataAreLoaded = loadGpsDataYearRange(m_names,
                                                    m_dbQueryData.yearStart,
                                                    m_dbQueryData.yearEnd);
            break;
            case DBReader::DB_QUERY_SQLFILE:
            gpsDataAreLoaded = loadGpsDataWithSqlFile(m_sqlFilePaths);
			break;
        }
        if(gpsDataAreLoaded)
        {
			// GpsData are loaded now. Drawing routine can start.
            for(size_t personIndex = 0; personIndex < m_numPerson; ++personIndex)
	        {
                if(m_walks[personIndex].getGpsData().getTotalGpsPoints() == 0)
                {
                    m_startScreenMode = true;
                }
            }
        }

        BOOST_FOREACH(const LocationImageData& locImgData, m_settings->getLocationImageData())
        {
            LocationImage* lImg;
            if(m_multiMode)
            {
                lImg = new LocationImage(*m_magicBox.get(), locImgData);
            }
            else
            {
                lImg = new LocationImage(m_magicBoxes[0], locImgData);
            }

            lImg->setViewBounds(m_viewMinDimension[0],
                                m_viewPadding[0],
                                m_viewXOffset[0],
                                m_viewYOffset[0]);

            m_locationImgs.push_back(lImg);
        }
    }
    else
    {
        m_startScreenMode = true;
    }
    ofSetFullscreen(m_isFullscreen);

    if(m_hideCursor)
        ofHideCursor();


    if(m_settings->isSoundActive())
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
	if(m_settings->isSoundActive() && m_soundPlayer.size() > 0)
	{
		if(m_timeline->isFirst())
		{
			currentSoundFile = 0;
            for (size_t i = 0; i < m_soundPlayer.size(); ++i)
			{
                m_soundPlayer[i].stop();
			}
			if(m_soundPlayer.size() > 0)
			{
                m_soundPlayer[currentSoundFile].play();
			}
		}
		else
		{
            if( ! m_soundPlayer[currentSoundFile].getIsPlaying())
			{
                m_soundPlayer[currentSoundFile].stop();
                m_soundPlayer[currentSoundFile].setPosition(0.0);
				if(currentSoundFile < static_cast<int>(m_soundPlayer.size()-1))
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
	if(m_timeline->isFirst())
	{
		zoomFrameCount = 0;
		return true;
	}

	switch (m_settings->getZoomAnimationCriteria()) {
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
    if(zoomFrameCount + 1 >= static_cast<int>(m_settings->getZoomAnimFrames().size()))
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
	if(zoomFrameCount+1 >=  static_cast<int>(m_settings->getZoomAnimFrames().size()))
		return false;
	string currentTimestamp = m_timeline->getCurrentTimelineObj().timeString;
	string zoomChangeTimestamp = m_settings->getZoomAnimFrames()[zoomFrameCount+1].timestamp;
	if (zoomChangeTimestamp.compare(currentTimestamp) == 0) {
		++zoomFrameCount;
		return true;
	} else {
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
		if((current / (float)all) > m_settings->getZoomAnimFrames()[i].frameTime)
		{
			currIndex = i;
		}
		else
		{
			break;
		}
	}
	if(zoomFrameCount != currIndex)
	{
		++zoomFrameCount;
		if(zoomFrameCount >=  static_cast<int>(m_settings->getZoomAnimFrames().size()))
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
	if(m_isZoomAnimation)
	{
		if(zoomHasChanged())
		{
            const ZoomAnimFrame& zoomAnimFrame =
                    m_settings->getZoomAnimFrames()[zoomFrameCount];
            float zoomLevel = zoomAnimFrame.frameZoom;
            double centerX = zoomAnimFrame.frameCenterX;
            double centerY = zoomAnimFrame.frameCenterY;
            UtmPoint utmP = GpsData::getUtmPointWithRegion(centerY, centerX,
                                                           *m_settings);

			if(m_timeline->isFirst())
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

    if(m_zoomIntegrator->isTargeting() || m_theIntegrator->isTargeting())
    {
        if(m_multiMode)
        {
            m_magicBox->setSize(m_zoomIntegrator->getValue());
            m_magicBox->setupBox(m_theIntegrator->getValue(), 0);
        }
        else
        {
            BOOST_FOREACH(MagicBox& box, m_magicBoxes)
            {
                box.setSize(m_zoomIntegrator->getValue());
                box.setupBox(m_theIntegrator->getValue(), 0);
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
        if(m_loopMode)
        {
            for(int i = 0; i < m_settings->getDrawSpeed(); ++i)
            {
                int id = m_timeline->getNext();
                if (id < (int)m_numPerson)
                {
                    m_walks[id].update();
                }

                zoomUpdate();
                soundUpdate();

                m_timeline->countUp();
                if(m_timeline->isFirst())
                {
//                            std::cout << "First timeline object!" << std::endl;
                    for(unsigned int i = 0; i < m_walks.size(); ++i)
                    {
                        m_walks[i].reset();
                    }
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
            for(int i = 0; i < m_settings->getDrawSpeed(); ++i)
            {
                if(!m_timeline->isLast())
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
unsigned currentFrame = 0;
void DrawingLifeApp::draw()
{
    if (m_startScreenMode)
    {
        drawStartScreen();
    }
    else
    {
        fillViewAreaUTM();
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
            for(size_t i = 0; i < m_locationImgs.size(); ++i)
            {
                m_locationImgs[i].draw();
            }
            if(m_multiMode && m_multiModeInfo)
            {
                ofSetColor(255, 255, 255, m_settings->getAlphaLegend());
                ofSetHexColor(0xffffff);
                std::string infoText = m_timeline->getCurrentTime();
//                if(m_pause)
//                    infoText.append(" (stopped)");
                m_fonts["info"].drawString(infoText,
                                           m_viewPadding[0],
                                           m_viewYOffset[0] + 10);

            }

            for(size_t i = 0; i < m_numPerson; ++i)
            {
                Walk& walk = m_walks[i];
                if (m_isDebugMode)
                {
                    ofSetColor(255, 255, 255, m_settings->getAlphaLegend());

                    int debugTextX = 30 + (ofGetWidth() / m_numPerson) * i;
                    int debugTextY = 30;
                    ofDrawBitmapString(walk.getCurrentGpsInfoDebug(),
                                       debugTextX, debugTextY);
                }
                else if(m_showInfo)
                {
                    ofSetColor(255, 255, 255, m_settings->getAlphaLegend());
                    ofSetHexColor(0xffffff);
                    std::string infoText = walk.getCurrentGpsInfo();
//                    if(m_pause)
//                        infoText.append(" (stopped)");
                    int infoX = m_viewPadding[i] + (ofGetWidth() / m_numPerson) * i;
                    int infoY = m_viewYOffset[i] + 10;
                    m_fonts["info"].drawString(infoText, infoX, infoY);
                }

                // -----------------------------------------------------------------------------
                // Draw Gps data
                // -----------------------------------------------------------------------------
                if(doShader)
                {
                    shaderBegin();
                }
                ofSetColor(m_settings->getColorForegroundR(),
                           m_settings->getColorForegroundG(),
                           m_settings->getColorForegroundB(),
                           m_settings->getAlphaTrack());
                ofNoFill();
                walk.draw();
                if(doShader)
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
            for(size_t i = 0; i < m_numPerson; ++i)
            {
                m_walks[i].drawAll();
            }
        }

        if(m_showFps)
            fpsDisplay();

        if(m_showKeyCommands)
            showKeyCommands();
        if(m_timeline->isLast())
        {

        }
    }

    if(m_settings->getIsGrabScreen())
    {
        static char fileNameStr[255];
        sprintf(fileNameStr, "output/output_%.4u.png", currentFrame);
        ofSaveScreen(fileNameStr);
        ++currentFrame;
    }
}
// -----------------------------------------------------------------------------
// Start screen
// -----------------------------------------------------------------------------
void DrawingLifeApp::drawStartScreen()
{
    ofSetColor(255,255,255);

    std::string title = APP_NAME_STR;
    title += " ";
    title += APP_VERSION_STR;
    m_fonts["title"].drawString(title, ofGetWidth()/2 - 365, ofGetHeight()/2 - 100);

    m_fonts["author"].drawString(APP_AUTHOR_STR, ofGetWidth()/2 - 91, ofGetHeight()/2);
    m_fonts["author"].drawString("plan b", ofGetWidth()/2 - 60, ofGetHeight()/2 + 60);

//    m_fontText.drawString("Press key 0 - 9 to choose a life map.", ofGetWidth()/2 - 300, ofGetHeight()/2 + 250);
}

void DrawingLifeApp::showKeyCommands()
{
    ofSetColor(255, 255, 255, m_settings->getAlphaLegend());
    std::stringstream stream;

    stream << "a           : draw all gps points\n";
    stream << "d           : debug mode\n";
    stream << "f           : toggle fullscreen\n";
    stream << "p           : show fps\n";
    stream << "k           : show key commands\n";
    stream << "+           : zoom in\n";
    stream << "-           : zoom out\n";
    stream << "left arrow  : move view left\n";
    stream << "right arrow : move view right\n";
    stream << "up arrow    : move view up\n";
    stream << "down arrow  : move view down\n";
    stream << "1 - 4       : set zoom level 1 - 4\n";
    stream << "0 - 9       : load gps data with a city (when querying city)\n";
    stream << "t           : toggle traced drawing (interactive mode)\n";
    stream << "space       : go to next segment (interactive mode)\n";
    stream << "backspace   : go to previous segment (interactive mode)\n";

   ofDrawBitmapString(stream.str(), 30, ofGetHeight() - 320);
}
// -----------------------------------------------------------------------------
// Retrieving new GpsData
// -----------------------------------------------------------------------------
void DrawingLifeApp::prepareGpsData()
{
    m_startScreenMode = false;

    m_gpsDatas.clear();
    m_walks.clear();
    m_magicBoxes.clear();
}

// -----------------------------------------------------------------------------

void DrawingLifeApp::processGpsData()
{
    ofLog(OF_LOG_VERBOSE, "------------------------\n");
    if (m_gpsDatas.size() > 0)
    {
        m_timeline->setTimeline(m_gpsDatas);
//        for(unsigned int i = 0; i < m_timeline->getTimeline().size(); ++i)
//        {
//            ofLog(OF_LOG_VERBOSE, "%s : %d : %li", m_timeline->getTimeline()[i].timeString.c_str(),
//                                                    m_timeline->getTimeline()[i].id,
//                                                    (long)(m_timeline->getTimeline()[i].secs));
//        }
        this->calculateGlobalMinMaxValues();

        Walk::setTrackAlpha(m_settings->getAlphaDot());

        Walk::setDotSize(m_settings->getDotSize());

        for(size_t i = 0; i < m_walks.size(); ++i)
        {
            Walk& walk = m_walks[i];
//            m_walks[i]->setDotColors();

            if(!m_settings->isBoundingBoxAuto() && !m_multiMode)
            {
                //m_walks[i]->setMagicBoxStatic(m_magicBoxes[i]);
                double bbLat = m_settings->getBoundingBoxLat();
                double bbLon = m_settings->getBoundingBoxLon();
                walk.setMagicBoxStatic(&m_magicBoxes[i], bbLat, bbLon);
            }
            else
            {
                if(m_multiMode)
                {
                    double bbLat = m_settings->getBoundingBoxLat();
                    double bbLon = m_settings->getBoundingBoxLon();
                    walk.setMagicBoxStatic(m_magicBox.get(), bbLat, bbLon);
//                    walk.setMagicBox(m_magicBox);
                }
                else
                {
                    walk.setMagicBox(&m_magicBoxes[i]);
                }
            }

            if (m_imageAsCurrentPoint && (unsigned int)m_images.size() >= m_numPerson)
            {
                walk.setCurrentPointImage(m_images[i], m_imageList[i].alpha);
            }
        }
    }
}

// -----------------------------------------------------------------------------

bool DrawingLifeApp::loadGpsDataCity(const StringVec& names,
                                     const std::string& city)
{
    std::vector<tFuncLoadGpsData> funcVec;
    for (size_t i = 0; i < m_numPerson; ++i)
    {
        tFuncLoadGpsData f = boost::bind(&DBReader::getGpsDataCity, _1, _2,
                                         names[i], city);
        funcVec.push_back(f);
    }

    return loadGpsData(funcVec);
}

// -----------------------------------------------------------------------------

bool DrawingLifeApp::loadGpsDataYearRange(const StringVec& names,
                                          int yearStart, int yearEnd)
{
    std::vector<tFuncLoadGpsData> funcVec;

    for (size_t i = 0; i < m_numPerson; ++i)
    {
        tFuncLoadGpsData f = boost::bind(&DBReader::getGpsDataYearRange, _1, _2,
                                         names[i], yearStart, yearEnd);
        funcVec.push_back(f);
    }

    return loadGpsData(funcVec);
}

// -----------------------------------------------------------------------------

bool DrawingLifeApp::loadGpsDataWithSqlFile(const StringVec& sqlFilePaths)
{
    std::vector<tFuncLoadGpsData> funcVec;
    for (size_t i = 0; i < m_numPerson; ++i)
    {
        std::ifstream sqlFile(ofToDataPath(sqlFilePaths[i]).c_str(),
                              std::ifstream::in);
        std::string sqlFileSource = std::string(std::istreambuf_iterator<char>(sqlFile),
                                                std::istreambuf_iterator<char>());

//        DBG_VAL(sqlFilePaths[i]);
//        DBG_VAL(sqlFileSource);

        tFuncLoadGpsData f = boost::bind(&DBReader::getGpsDataWithSqlFile, _1, _2,
                                         sqlFileSource);
        funcVec.push_back(f);
    }

    return loadGpsData(funcVec);
}

// -----------------------------------------------------------------------------

bool DrawingLifeApp::loadGpsData(const std::vector<tFuncLoadGpsData>& funcVec)
{
    prepareGpsData();

    bool dbOk = false;

    if(m_multiMode)
    {
        m_magicBox.reset(new MagicBox(*m_settings,
                                      m_settings->getBoundingBoxSize(),
                                      m_settings->getBoundingBoxPadding()));
    }
    // get GpsData from database
    for(size_t i = 0; i < m_numPerson; ++i)
    {
        GpsData* gpsData = new GpsData(*m_settings);
        m_gpsDatas.push_back(gpsData);

        if(!m_multiMode)
        {
            m_magicBoxes.push_back(new MagicBox(*m_settings,
                                                m_settings->getBoundingBoxSize(),
                                                m_settings->getBoundingBoxPadding()));
        }

        Walk* walk = new Walk(*m_settings,
                              m_settings->getNameColors()[i]);
        m_walks.push_back(walk);

        walk->setViewBounds(ofGetWidth(),
                            ofGetHeight(),
                            m_viewXOffset[i],
                            m_viewYOffset[i],
                            m_viewMinDimension[i],
                            m_viewPadding[i]);
        walk->reset();

        DBReaderPtr dbReader(new DBReader(m_dbPath, m_settings->useSpeed()));
        if (dbReader->setupDbConnection())
        {
            // -----------------------------------------------------------------------------
            // DB query
            tFuncLoadGpsData getGpsDataFunc = funcVec.at(i);
            dbOk = getGpsDataFunc(dbReader.get(), *gpsData);

            if(dbOk)
            {
                ofLog(OF_LOG_SILENT, "--> GpsData load ok!");
                ofLog(OF_LOG_SILENT, "--> Total data: %d GpsSegments, %d GpsPoints!\n",
                      gpsData->getSegments().size(),
                      gpsData->getTotalGpsPoints());

                walk->setGpsData(gpsData);
            }
            else
            {
                ofLog(OF_LOG_SILENT, "--> No GpsData loaded!");
                break;
            }
            dbReader->closeDbConnection();
        }
        // -----------------------------------------------------------------------------

        ofLog(OF_LOG_VERBOSE, "minLon: %lf, maxLon: %lf, minLat: %lf, maxLat: %lf",
          gpsData->getMinUtmX(),
          gpsData->getMaxUtmX(),
          gpsData->getMinUtmY(),
          gpsData->getMaxUtmY());
        ofLog(OF_LOG_VERBOSE, "Central Meridian: %lf",
              gpsData->getProjectionCentralMeridian());
    }

    processGpsData();

    return dbOk;

}

// -----------------------------------------------------------------------------
// View
// -----------------------------------------------------------------------------
void DrawingLifeApp::setViewAspectRatio()
{
    double width;
    double height;
    if(m_multiMode)
    {
        width = ofGetWidth();
    }
    else
    {
        width = ofGetWidth() / (int)m_numPerson;
    }

    height = ofGetHeight();

    for(size_t i = 0; i < m_numPerson; ++i)
    {
        // Reset for view padding.
        m_viewXOffset[i] = 0;
        m_viewYOffset[i] = 0;

//        if(false)
//        {
//            m_viewMinDimension[personIndex] = height;
//
//            m_viewXOffset[personIndex] += m_viewPadding[personIndex];
//            m_viewYOffset[personIndex] += m_viewPadding[personIndex];
//        }
//        else
//        {
        // Set square view area and center.
        if (height < width)
        {
            m_viewMinDimension[i] = height;
            m_viewXOffset[i] = (width - height) / 2.0;
        }
        else if (width < height)
        {
            m_viewMinDimension[i] = width;
            m_viewYOffset[i] = (height - width) / 2.0;
        }
        else
        {
            m_viewMinDimension[i] = width;
        }

        // Left and top indentation.
        m_viewXOffset[i] += m_viewPadding[i];
        m_viewYOffset[i] += m_viewPadding[i];

        if(!m_multiMode)
        {
            m_viewXOffset[i] += width * i;
        }

//        m_viewYOffset += m_viewPadding;
    }
}
void DrawingLifeApp::fillViewAreaUTM()
{
    for(size_t i = 0; i < m_numPerson; ++i)
    {
        // Normalized value range from 0 to 1.
        Walk& walk = m_walks[i];
        double x = walk.getScaledUtmX(0);
        double y = walk.getScaledUtmY(0);
        double w = walk.getScaledUtmX(1) - x;
        double h = walk.getScaledUtmY(1) - y;
        ofFill();
        ofSetColor(m_settings->getColorViewboxR(),
                   m_settings->getColorViewboxG(),
                   m_settings->getColorViewboxB());
        ofRect(x, y, w, h);
        if(m_multiMode)
            break;

    }
}

// -----------------------------------------------------------------------------
// Input
// TODO Add function for processing keys
//--------------------------------------------------------------
void DrawingLifeApp::keyPressed  (int key)
{
//    DBG_VAL(key);
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
        if(m_hideCursor)
        {
            if(!m_isDebugMode)
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
//        for(unsigned int i = 0; i < m_walks.size(); ++i)
//        {
//            m_walks[i]->setDotColors();
//        }
        break;
//    case 49:
//    if(m_dbQueryData.type == DBReader::DB_QUERY_CITY)
//    {
//        loadGpsDataCity(m_names, "Berlin");
//    }
//    else if(m_dbQueryData.type != DBReader::DB_QUERY_CITY)
//    {
//        for(unsigned int i = 0; i < m_magicBoxes.size(); ++i)
//        {
//            m_magicBoxes[i]->toggleZoomLevel(1);
//        }
//        if(m_multiMode)
//            m_magicBox->toggleZoomLevel(1);
//    }
//    break;
//    case 50:
//    if(m_dbQueryData.type == DBReader::DB_QUERY_CITY)
//    {
//        loadGpsDataCity(m_names, "London");
//    }
//    else
//    {
//        for(unsigned int i = 0; i < m_magicBoxes.size(); ++i)
//        {
//            m_magicBoxes[i]->toggleZoomLevel(2);
//        }
//        if(m_multiMode)
//            m_magicBox->toggleZoomLevel(2);
//    }
//    break;
//    case 51:
//    if(m_dbQueryData.type == DBReader::DB_QUERY_CITY)
//    {
//        loadGpsDataCity(m_names, "Barcelona");
//    }
//    else
//    {
//        for(unsigned int i = 0; i < m_magicBoxes.size(); ++i)
//        {
//            m_magicBoxes[i]->toggleZoomLevel(3);
//        }
//        if(m_multiMode)
//            m_magicBox->toggleZoomLevel(3);
//    }
//    break;
//    case 52:
//    if(m_dbQueryData.type == DBReader::DB_QUERY_CITY)
//    {
//        loadGpsDataCity(m_names, "Hamburg");
//    }
//    else
//    {
//        for(unsigned int i = 0; i < m_magicBoxes.size(); ++i)
//        {
//            m_magicBoxes[i]->toggleZoomLevel(4);
//        }
//        if(m_multiMode)
//            m_magicBox->toggleZoomLevel(4);
//    }
//    break;
//    case 53:
//    if(m_dbQueryData.type == DBReader::DB_QUERY_CITY)
//        loadGpsDataCity(m_names, "Vienna");
//        break;
//    case 54:
//    if(m_dbQueryData.type == DBReader::DB_QUERY_CITY)
//        loadGpsDataCity(m_names,"New York");
//        break;
//    case 55:
//    if(m_dbQueryData.type == DBReader::DB_QUERY_CITY)
//        loadGpsDataCity(m_names, "Tokyo");
//        break;
//    case 56:
//    if(m_dbQueryData.type == DBReader::DB_QUERY_CITY)
//        loadGpsDataCity(m_names, "San Francisco");
//        break;
//    case 57:
//    if(m_dbQueryData.type == DBReader::DB_QUERY_CITY)
//        loadGpsDataCity(m_names, "Bristol");
//        break;
//    case 48:
//    if(m_dbQueryData.type == DBReader::DB_QUERY_CITY)
//        loadGpsDataCity(m_names, "Banff");
//        break;
    case '+':
        for(unsigned int i = 0; i < m_magicBoxes.size(); ++i)
        {
            m_magicBoxes[i].zoom(MagicBox::ZOOM_IN);
        }
        if(m_multiMode)
            m_magicBox->zoom(MagicBox::ZOOM_IN);
        break;
    case '-':
        for(unsigned int i = 0; i < m_magicBoxes.size(); ++i)
        {
            m_magicBoxes[i].zoom(MagicBox::ZOOM_OUT);
        }
        if(m_multiMode)
            m_magicBox->zoom(MagicBox::ZOOM_OUT);
        break;
    case OF_KEY_UP:
        for(unsigned int i = 0; i < m_magicBoxes.size(); ++i)
        {
            m_magicBoxes[i].move(MagicBox::UP);
        }
        if(m_multiMode)
            m_magicBox->move(MagicBox::UP);
        break;
    case OF_KEY_DOWN:
        for(unsigned int i = 0; i < m_magicBoxes.size(); ++i)
        {
            m_magicBoxes[i].move(MagicBox::DOWN);
        }
        if(m_multiMode)
            m_magicBox->move(MagicBox::DOWN);
        break;
       break;
    case OF_KEY_RIGHT:
        for(unsigned int i = 0; i < m_magicBoxes.size(); ++i)
        {
            m_magicBoxes[i].move(MagicBox::RIGHT);
        }
        if(m_multiMode)
            m_magicBox->move(MagicBox::RIGHT);
        break;
    case OF_KEY_LEFT:
        for(unsigned int i = 0; i < m_magicBoxes.size(); ++i)
        {
            m_magicBoxes[i].move(MagicBox::LEFT);
        }
        if(m_multiMode)
            m_magicBox->move(MagicBox::LEFT);

        break;
    case ' ':
        if(m_interactiveMode)
        {
            for (unsigned int i=0; i < m_walks.size(); ++i)
            {
                m_walks[i].updateToNextSegment();
            }
        }
        else
        {
            m_pause = !m_pause;
        }
        break;
    case OF_KEY_BACKSPACE:
        if(m_interactiveMode)
        {
            for (unsigned int i=0; i < m_walks.size();++i)
            {
                m_walks[i].updateToPreviousSegment();
            }
        }
        break;
    case 't':
        if(m_interactiveMode)
        {
            for (unsigned int i=0; i < m_walks.size();++i)
            {
                m_walks[i].toggleTraced();
            }
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
    this->setViewAspectRatio();
    for(size_t i = 0; i < m_numPerson; ++i)
    {
//        if(m_walks[personIndex])
        m_walks[i].setViewBounds(ofGetWidth(),
                                 ofGetHeight(),
                                 m_viewXOffset[i],
                                 m_viewYOffset[i],
                                 m_viewMinDimension[i],
                                 m_viewPadding[i]);
    }
    for(size_t i = 0; i < m_locationImgs.size(); ++i)
    {
        m_locationImgs[i].setViewBounds(m_viewMinDimension[0],
                                        m_viewPadding[0],
                                        m_viewXOffset[0],
                                        m_viewYOffset[0]);
    }
}

void DrawingLifeApp::calculateGlobalMinMaxValues()
{
    double minX = numeric_limits<double>::max();
    double maxX = -numeric_limits<double>::max();
    double minY = numeric_limits<double>::max();
    double maxY = -numeric_limits<double>::max();
    double minLon = numeric_limits<double>::max();
    double maxLon = -numeric_limits<double>::max();
    double minLat = numeric_limits<double>::max();
    double maxLat = -numeric_limits<double>::max();
    BOOST_FOREACH(const GpsData& gpsData, m_gpsDatas)
    {
        if (gpsData.getMinUtmX() < minX) minX = gpsData.getMinUtmX();
        if (gpsData.getMaxUtmX() > maxX) maxX = gpsData.getMaxUtmX();
        if (gpsData.getMinUtmY() < minY) minY = gpsData.getMinUtmY();
        if (gpsData.getMaxUtmY() > maxY) maxY = gpsData.getMaxUtmY();
        if (gpsData.getMinLon() < minLon) minLon = gpsData.getMinLon();
        if (gpsData.getMaxLon() > maxLon) maxLon = gpsData.getMaxLon();
        if (gpsData.getMinLat() < minLat) minLat = gpsData.getMinLon();
        if (gpsData.getMaxLat() > maxLat) maxLat = gpsData.getMaxLon();
    }

    bool isRegionOn = m_settings->isRegionsOn();
    if(isRegionOn)
    {
        GpsData::setGlobalValues(minX, maxX, minY, maxY, 0.0);
    }
    else
    {
        if(m_settings->isMeridianAuto())
            GpsData::setGlobalValues(minX, maxX, minY, maxY,
                                     (minLon + (maxLon - minLon)/2));
        else
        {
            double lon0 = 0.0;
            lon0 = m_settings->getMeridian();
            GpsData::setGlobalValues(minX, maxX, minY, maxY, lon0);
        }
    }

    BOOST_FOREACH(GpsData& gpsData, m_gpsDatas)
    {
        gpsData.calculateUtmPointsGlobalLon(isRegionOn);
        gpsData.normalizeUtmPointsGlobal();
    }
}

void DrawingLifeApp::fpsDisplay()
{
    timeNow = ofGetElapsedTimef();
    double diff = timeNow-timeThen;
    timeThen		= timeNow;

    timeSum += diff;
    if (timeSum > 0.5)
    {
        fpsToShow = 1.0/diff;
        timeSum = 0.0;
    }

    ofSetHexColor(0xffffff);
    std::string str = "FPS: "+ofToString((double)fpsToShow, 1);
    ofDrawBitmapString(str, 30.0, ofGetHeight()-30 );
}

void DrawingLifeApp::loadCurrentPointImages()
{
    for(size_t i = 0; i < m_imageList.size(); ++i)
    {
        ofImage* tmpImg = new ofImage();
        std::string str = m_imageList[i].path;
        float width = m_imageList[i].width;
        float height= m_imageList[i].height;

        if(tmpImg->loadImage(str))
        {
            tmpImg->resize(width, height);
            m_images.push_back(tmpImg);
        }
        else
        {
            m_imageAsCurrentPoint = false;
            break;
        }
    }
}
