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
    m_settings(new AppSettings(settingsFile)),
    m_dbReader(0),
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

    for(unsigned int personIndex = 0; personIndex < m_numPerson; ++personIndex)
    {
        SAFE_DELETE(m_gpsDatas[personIndex]);
		SAFE_DELETE(m_walks[personIndex]);
		if(!m_multiMode)
            SAFE_DELETE(m_magicBoxes[personIndex]);
    }
    if(m_multiMode)
        SAFE_DELETE(m_magicBox);

    SAFE_DELETE(m_timeline);
    SAFE_DELETE(m_settings);
    for(unsigned int i = 0; i < m_images.size(); ++i)
    {
        m_images[i].clear();
    }
    m_images.clear();
    for(unsigned int i = 0; i < m_locationImgs.size(); ++i)
    {
        SAFE_DELETE(m_locationImgs[i]);
    }
}

void DrawingLifeApp::setup()
{
    // -----------------------------------------------------------------------------
//    AppSettings& settings = AppSettings::instance();

    ofSetLogLevel(m_settings->getLogLevel());

	// -----------------------------------------------------------------------------
	// Fonts.
	// -----------------------------------------------------------------------------
	m_fontTitle.loadFont(m_settings->getFontTitleName(),
						 m_settings->getFontTitleSize());

    m_fontAuthor.loadFont(m_settings->getFontAuthorName(),
                          m_settings->getFontAuthorSize());

    m_fontText.loadFont(m_settings->getFontTextName(),
                        m_settings->getFontTextSize());

    m_fontInfo.loadFont(m_settings->getFontInfoName(),
                        m_settings->getFontInfoSize());

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
	// TODO 101028_1709_TP: Why are those vectors stuffed with 0?
	// This hinders to check the vectors themselves for unequal 0 later.
	// See affected TODO 101028_1710_TP.
    for(unsigned int personIndex = 0; personIndex < m_numPerson; ++personIndex)
    {
//        m_names.push_back(m_settings.getValue("name", "", i));
        m_gpsDatas.push_back(0);
		m_walks.push_back(0);

		if(!m_multiMode)
            m_magicBoxes.push_back(0);

        DBG_VAL(m_names[personIndex]);

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


    DBG_VAL(m_numPerson);
    // -----------------------------------------------------------------------------

    this->setViewAspectRatio();
    // -----------------------------------------------------------------------------
    loadCurrentPointImages();

    m_timeline = new Timeline();

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
            gpsDataAreLoaded = loadGpsDataYearRange(m_names, m_dbQueryData.yearStart, m_dbQueryData.yearEnd);
            break;
            case DBReader::DB_QUERY_SQLFILE:
            gpsDataAreLoaded = loadGpsDataWithSqlFile(m_names, m_sqlFilePaths);
			break;
        }
		if(gpsDataAreLoaded)
		{
			// GpsData are loaded now. Drawing routine can start.
			for(unsigned int personIndex = 0; personIndex < m_numPerson; ++personIndex)
	        {
				// TODO 101028_1710_TP: Check for !m_walks before accessing the object.
                if(m_walks[personIndex]->getGpsData().getTotalGpsPoints() == 0)
                {
                    m_startScreenMode = true;
                }
            }
        }

        for(unsigned int i = 0; i < m_settings->getLocationImageData().size(); ++i)
        {
            LocationImage* lImg;
            if(m_multiMode)
                lImg = new LocationImage(m_magicBox, m_settings->getLocationImageData()[i]);
            else
                lImg = new LocationImage(m_magicBoxes[0], m_settings->getLocationImageData()[i]);

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
}

//--------------------------------------------------------------
bool firstSleep = true;
void DrawingLifeApp::update()
{
    if (m_isAnimation && !m_pause)
    {
        if(!m_interactiveMode)
        {
            if(m_timeline->getTimeline().size() > 0)
            {

                if(m_loopMode)
                {
                    for(int i = 0; i < m_settings->getDrawSpeed(); ++i)
                    {
                        int id = m_timeline->getNext();
                        if (id < (int)m_numPerson)
                        {
                            m_walks[id]->update();
                        }
                        m_timeline->countUp();
                        if(m_timeline->isFirst())
                        {

//                            std::cout << "First timeline object!" << std::endl;
                            for(unsigned int i = 0; i < m_walks.size(); ++i)
                            {
                                m_walks[i]->reset();
                            }
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
                            m_walks[id]->update();
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
    }
}

//--------------------------------------------------------------
void DrawingLifeApp::draw()
{
    if (m_startScreenMode)
    {
        drawStartScreen();
    }
    else
    {
		// -----------------------------------------------------------------------------
		// Draw Gps data as an animation.
		// -----------------------------------------------------------------------------
        if (m_isAnimation)
        {
            // -----------------------------------------------------------------------------
            // Draw rectangle with text.
            // -----------------------------------------------------------------------------
            fillViewAreaUTM();
            //---------------------------------------------------------------------------
            for(unsigned int i = 0; i < m_locationImgs.size(); ++i)
            {
                m_locationImgs[i]->draw();
            }
            if(m_multiMode && m_multiModeInfo)
            {
                ofSetColor(255, 255, 255, m_settings->getAlphaLegend());
                ofSetColor(0xffffff);
                std::string infoText = m_timeline->getCurrentTime();
//                if(m_pause)
//                    infoText.append(" (stopped)");
                m_fontInfo.drawString(infoText,
                                      m_viewPadding[0],
                                      m_viewYOffset[0] + 10);

            }

            for(unsigned int personIndex = 0; personIndex < m_numPerson; ++personIndex)
            {
                if (m_isDebugMode)
                {
                    ofSetColor(255, 255, 255, m_settings->getAlphaLegend());
					ofDrawBitmapString(m_walks[personIndex]->getCurrentGpsInfoDebug(),
									   30 + (ofGetWidth()/m_numPerson)*personIndex,
									   30);
                }
                else if(m_showInfo)
                {
                    ofSetColor(255, 255, 255, m_settings->getAlphaLegend());
                    ofSetColor(0xffffff);
                    std::string infoText = m_walks[personIndex]->getCurrentGpsInfo();
//                    if(m_pause)
//                        infoText.append(" (stopped)");
                    m_fontInfo.drawString(infoText,
                                          m_viewPadding[personIndex] + (ofGetWidth()/m_numPerson)*personIndex ,
                                          m_viewYOffset[personIndex] + 10);
                }

                // -----------------------------------------------------------------------------
                // Draw Gps data
                // -----------------------------------------------------------------------------
                ofSetColor(m_settings->getColorForegroundR(),
                           m_settings->getColorForegroundG(),
                           m_settings->getColorForegroundB(),
                           m_settings->getAlphaTrack());
                ofNoFill();
                m_walks[personIndex]->draw();
            }
        }
		// -----------------------------------------------------------------------------
		// Draw all Gps data at once. No animation.
		// -----------------------------------------------------------------------------
        else
        {
            fillViewAreaUTM();
            // -----------------------------------------------------------------------------
            // Draw Gps data
            // -----------------------------------------------------------------------------
            ofSetColor(m_settings->getColorForegroundR(),
                       m_settings->getColorForegroundG(),
                       m_settings->getColorForegroundB(),
                       m_settings->getAlphaTrack());
            ofNoFill();
            for(unsigned int personIndex = 0; personIndex < m_numPerson; ++personIndex)
            {
				m_walks[personIndex]->drawAll();
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
    m_fontTitle.drawString(title, ofGetWidth()/2 - 365, ofGetHeight()/2 - 100);

    m_fontAuthor.drawString(APP_AUTHOR_STR, ofGetWidth()/2 - 91, ofGetHeight()/2);
    m_fontAuthor.drawString("plan b", ofGetWidth()/2 - 60, ofGetHeight()/2 + 60);

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

    for(unsigned int personIndex = 0; personIndex < m_numPerson; ++personIndex)
    {
        SAFE_DELETE(m_gpsDatas[personIndex]);
		SAFE_DELETE(m_walks[personIndex]);
		if(!m_multiMode)
            SAFE_DELETE(m_magicBoxes[personIndex]);
    }
    if(m_multiMode)
        SAFE_DELETE(m_magicBox)
}

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

        for(unsigned int i = 0; i < m_walks.size(); ++i)
        {
//            m_walks[i]->setDotColors();

            if(!m_settings->isBoundingBoxAuto() && !m_multiMode)
            {
                //m_walks[i]->setMagicBoxStatic(m_magicBoxes[i]);
                double bbLat = m_settings->getBoundingBoxLat();
                double bbLon = m_settings->getBoundingBoxLon();
                m_walks[i]->setMagicBoxStatic(m_magicBoxes[i], bbLat, bbLon);
            }
            else
            {
                if(m_multiMode)
                {
                    double bbLat = m_settings->getBoundingBoxLat();
                    double bbLon = m_settings->getBoundingBoxLon();
                    m_walks[i]->setMagicBoxStatic(m_magicBox, bbLat, bbLon);
//                    m_walks[i]->setMagicBox(m_magicBox);
                }
                else
                {
                    m_walks[i]->setMagicBox(m_magicBoxes[i]);
                }
            }


            if (m_imageAsCurrentPoint && (unsigned int)m_images.size() >= m_numPerson)
            {
                m_walks[i]->setCurrentPointImage(m_images[i], m_imageList[i].alpha);
            }
        }
    }

}

bool DrawingLifeApp::loadGpsDataCity(std::vector<std::string> names, std::string city)
{
    prepareGpsData();

    bool dbOk = false;

    // get GpsData from database
    if(m_multiMode)
    {
        m_magicBox = new MagicBox(m_settings, m_settings->getBoundingBoxSize(),
                                    m_settings->getBoundingBoxPadding());
    }

    for(unsigned int personIndex = 0; personIndex < m_numPerson; ++personIndex)
    {
        m_gpsDatas[personIndex] = new GpsData(m_settings);

        if(!m_multiMode)
        {
            m_magicBoxes[personIndex] = new MagicBox(m_settings, m_settings->getBoundingBoxSize(),
                                        m_settings->getBoundingBoxPadding());
        }

        m_walks[personIndex] = new Walk(m_settings, m_settings->getNameColors()[personIndex]);

		m_walks[personIndex]->setViewBounds(ofGetWidth(),
											ofGetHeight(),
											m_viewXOffset[personIndex],
											m_viewYOffset[personIndex],
											m_viewMinDimension[personIndex],
											m_viewPadding[personIndex]);
        m_walks[personIndex]->reset();

        m_dbReader = new DBReader(m_dbPath, m_settings->useSpeed());
        if (m_dbReader->setupDbConnection())
        {
            // -----------------------------------------------------------------------------
            // DB query
            dbOk = m_dbReader->getGpsDataCity(*m_gpsDatas[personIndex], names[personIndex], city);
            if(dbOk)
            {
                ofLog(OF_LOG_SILENT, "--> GpsData load ok!");
                ofLog(OF_LOG_SILENT, "--> Total data: %d GpsSegments, %d GpsPoints!\n",
                      m_gpsDatas[personIndex]->getSegments().size(),
                      m_gpsDatas[personIndex]->getTotalGpsPoints());
				m_walks[personIndex]->setGpsData(m_gpsDatas[personIndex]);
            }
            else
            {
                ofLog(OF_LOG_SILENT, "--> No GpsData loaded!");
                break;
            }
            m_dbReader->closeDbConnection();
        }
        // -----------------------------------------------------------------------------
        SAFE_DELETE(m_dbReader);
        // test print
        maxPoints = 0;
        for (unsigned int i = 0; i < m_gpsDatas[personIndex]->getSegments().size(); ++i)
        {
            for (unsigned int j = 0; j < m_gpsDatas[personIndex]->getSegments()[i].getPoints().size(); ++j)
            {
                ++maxPoints;
            }
        }

        DBG_VAL(city);
        ofLog(OF_LOG_VERBOSE, "minLon: %lf, maxLon: %lf, minLat: %lf, maxLat: %lf",
          m_gpsDatas[personIndex]->getMinUtmX(),
          m_gpsDatas[personIndex]->getMaxUtmX(),
          m_gpsDatas[personIndex]->getMinUtmY(),
          m_gpsDatas[personIndex]->getMaxUtmY());
        ofLog(OF_LOG_VERBOSE, "Central Meridian: %lf", m_gpsDatas[personIndex]->getProjectionCentralMeridian());
    }

    processGpsData();

    return dbOk;
}
bool DrawingLifeApp::loadGpsDataYearRange(std::vector<std::string> names, int yearStart, int yearEnd)
{
//    m_startScreenMode = false;
//
//    // get GpsData from database
//    for(unsigned int personIndex = 0; personIndex < m_numPerson; ++personIndex)
//    {
//        SAFE_DELETE(m_gpsDatas[personIndex]);
//        SAFE_DELETE(m_magicBoxes[personIndex]);
//		SAFE_DELETE(m_walks[personIndex]);
//    }

    prepareGpsData();

    bool dbOk = false;

    if(m_multiMode)
    {
        m_magicBox = new MagicBox(m_settings, m_settings->getBoundingBoxSize(),
                                        m_settings->getBoundingBoxPadding());
    }
    for(unsigned int personIndex = 0; personIndex < m_numPerson; ++personIndex)
    {
        m_gpsDatas[personIndex] = new GpsData(m_settings);

        if(!m_multiMode)
        {
            m_magicBoxes[personIndex] = new MagicBox(m_settings, m_settings->getBoundingBoxSize(),
                                        m_settings->getBoundingBoxPadding());
        }
        m_walks[personIndex] = new Walk(m_settings, m_settings->getNameColors()[personIndex]);

		m_walks[personIndex]->setViewBounds(ofGetWidth(), ofGetHeight(), m_viewXOffset[personIndex], m_viewYOffset[personIndex], m_viewMinDimension[personIndex], m_viewPadding[personIndex]);
        m_walks[personIndex]->reset();

        m_dbReader = new DBReader(m_dbPath, m_settings->useSpeed());

        if (m_dbReader->setupDbConnection())
        {
            // -----------------------------------------------------------------------------
            // DB query
            dbOk = m_dbReader->getGpsDataYearRange(*m_gpsDatas[personIndex], names[personIndex], yearStart, yearEnd);
            if(dbOk)
            {
                ofLog(OF_LOG_SILENT, "--> GpsData set %d: load ok!", personIndex+1);
                ofLog(OF_LOG_SILENT, "--> Total data: %d GpsSegments, %d GpsPoints!\n",
                      m_gpsDatas[personIndex]->getSegments().size(),
                      m_gpsDatas[personIndex]->getTotalGpsPoints());
				m_walks[personIndex]->setGpsData(m_gpsDatas[personIndex]);
            }
            else
            {
                ofLog(OF_LOG_SILENT, "--> No GpsData loaded!");
                break;
            }
            m_dbReader->closeDbConnection();
        }
        // -----------------------------------------------------------------------------
        SAFE_DELETE(m_dbReader);
        // test print
        maxPoints = 0;
        for (unsigned int i = 0; i < m_gpsDatas[personIndex]->getSegments().size(); ++i)
        {
            for (unsigned int j = 0; j < m_gpsDatas[personIndex]->getSegments()[i].getPoints().size(); ++j)
            {
                ++maxPoints;
            }
        }

        ofLog(OF_LOG_VERBOSE, "Start year: %d, end year: %d", yearStart, yearEnd);
        ofLog(OF_LOG_VERBOSE, "minLon: %lf, maxLon: %lf, minLat: %lf, maxLat: %lf",
          m_gpsDatas[personIndex]->getMinUtmX(),
          m_gpsDatas[personIndex]->getMaxUtmX(),
          m_gpsDatas[personIndex]->getMinUtmY(),
          m_gpsDatas[personIndex]->getMaxUtmY());
        ofLog(OF_LOG_VERBOSE, "Central Meridian: %lf\n", m_gpsDatas[personIndex]->getProjectionCentralMeridian());
    }
//    ofLog(OF_LOG_VERBOSE, "------------------------\n");
//    if (m_gpsDatas.size() > 0 && dbOk)
//    {
//        m_timeline->setTimeline(m_gpsDatas);
//
////        ofstream myfile;
////        myfile.open ("timeline_output.txt");
////        for(unsigned int i = 0; i < m_timeline->getTimeline().size(); ++i)
////        {
//////            ofLog(OF_LOG_VERBOSE, "%s : %d : %li", m_timeline->getTimeline()[i].timeString.c_str(),
//////                                                    m_timeline->getTimeline()[i].id,
//////                                                    (long)(m_timeline->getTimeline()[i].secs));
////            myfile << m_timeline->getTimeline()[i].timeString.c_str()
////            << " : " << m_timeline->getTimeline()[i].id
////            << " : " << (long)(m_timeline->getTimeline()[i].secs) << "\n";
////
////        }
////        myfile.close();
//
//        this->calculateGlobalMinMaxValues();
//
//        Walk::setTrackAlpha(m_settings->getAlphaDot());
//
//        Walk::setDotSize(m_settings->getDotSize());
//
//        for(unsigned int i = 0; i < m_walks.size(); ++i)
//        {
//            m_walks[i]->setDotColors();
//
//            if(!m_settings->isBoundingBoxEnabled())
//                m_walks[i]->setMagicBoxStatic(m_magicBoxes[i]);
//            else
//                m_walks[i]->setMagicBox(m_magicBoxes[i]);
//
//            if (m_imageAsCurrentPoint && (unsigned int)m_images.size() >= m_numPerson)
//            {
//                m_walks[i]->setCurrentPointImage(m_images[i]);
//            }
//        }
//    }

    processGpsData();

    return dbOk;
}

bool DrawingLifeApp::loadGpsDataWithSqlFile(std::vector<std::string> names, std::vector<std::string> sqlFilePaths)
{
    prepareGpsData();

    bool dbOk = false;

    if(m_multiMode)
    {
        m_magicBox = new MagicBox(m_settings, m_settings->getBoundingBoxSize(),
                                        m_settings->getBoundingBoxPadding());
    }
    // get GpsData from database
    for(unsigned int personIndex = 0; personIndex < m_numPerson; ++personIndex)
    {
        m_gpsDatas[personIndex] = new GpsData(m_settings);

        if(!m_multiMode)
        {
            m_magicBoxes[personIndex] = new MagicBox(m_settings, m_settings->getBoundingBoxSize(),
                                        m_settings->getBoundingBoxPadding());
        }
        m_walks[personIndex] = new Walk(m_settings, m_settings->getNameColors()[personIndex]);

		m_walks[personIndex]->setViewBounds(ofGetWidth(), ofGetHeight(), m_viewXOffset[personIndex], m_viewYOffset[personIndex], m_viewMinDimension[personIndex], m_viewPadding[personIndex]);
        m_walks[personIndex]->reset();

        std::ifstream sqlFile(ofToDataPath(sqlFilePaths[personIndex]).c_str(), std::ifstream::in);
        std::string sqlFileSource = std::string(std::istreambuf_iterator<char>(sqlFile), std::istreambuf_iterator<char>());

        DBG_VAL(sqlFilePaths[personIndex]);
        DBG_VAL(sqlFileSource);

        m_dbReader = new DBReader(m_dbPath, m_settings->useSpeed());
        if (m_dbReader->setupDbConnection())
        {
            // -----------------------------------------------------------------------------
            // DB query
            dbOk = m_dbReader->getGpsDataWithSqlFile(*m_gpsDatas[personIndex], names[personIndex], sqlFileSource);
            if(dbOk)
            {
                ofLog(OF_LOG_SILENT, "--> GpsData load ok!");
                ofLog(OF_LOG_SILENT, "--> Total data: %d GpsSegments, %d GpsPoints!\n",
                      m_gpsDatas[personIndex]->getSegments().size(),
                      m_gpsDatas[personIndex]->getTotalGpsPoints());
				m_walks[personIndex]->setGpsData(m_gpsDatas[personIndex]);
            }
            else
            {
                ofLog(OF_LOG_SILENT, "--> No GpsData loaded!");
                break;
            }
            m_dbReader->closeDbConnection();
        }
        // -----------------------------------------------------------------------------
        SAFE_DELETE(m_dbReader);
        // test print
        maxPoints = 0;
        for (unsigned int i = 0; i < m_gpsDatas[personIndex]->getSegments().size(); ++i)
        {
            for (unsigned int j = 0; j < m_gpsDatas[personIndex]->getSegments()[i].getPoints().size(); ++j)
            {
                ++maxPoints;
            }
        }

        ofLog(OF_LOG_VERBOSE, "minLon: %lf, maxLon: %lf, minLat: %lf, maxLat: %lf",
          m_gpsDatas[personIndex]->getMinUtmX(),
          m_gpsDatas[personIndex]->getMaxUtmX(),
          m_gpsDatas[personIndex]->getMinUtmY(),
          m_gpsDatas[personIndex]->getMaxUtmY());
        ofLog(OF_LOG_VERBOSE, "Central Meridian: %lf", m_gpsDatas[personIndex]->getProjectionCentralMeridian());
    }

    processGpsData();

    return dbOk;
}

// -----------------------------------------------------------------------------

void DrawingLifeApp::setViewAspectRatio()
{
    double width;
    double height;
    if(m_multiMode)
        width = ofGetWidth();
    else
        width = ofGetWidth()/(int)m_numPerson;

    height = ofGetHeight();

    for(unsigned int personIndex = 0; personIndex < m_numPerson; ++personIndex)
    {
        // Reset for view padding.
        m_viewXOffset[personIndex] = 0;
        m_viewYOffset[personIndex] = 0;

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
            m_viewMinDimension[personIndex] = height;
            m_viewXOffset[personIndex] = (width - height) / 2.0;
        }
        else if (width < height)
        {
            m_viewMinDimension[personIndex] = width;
            m_viewYOffset[personIndex] = (height - width) / 2.0;
        }
        else
        {
            m_viewMinDimension[personIndex] = width;
        }

        // Left and top indentation.
        m_viewXOffset[personIndex] += m_viewPadding[personIndex];
        m_viewYOffset[personIndex] += m_viewPadding[personIndex];

        if(!m_multiMode)
        {
            m_viewXOffset[personIndex] += width * personIndex;
        }

//        m_viewYOffset += m_viewPadding;
    }
}
void DrawingLifeApp::fillViewAreaUTM()
{
    for(unsigned int personIndex = 0; personIndex < m_numPerson; ++personIndex)
    {
        // Normalized value range from 0 to 1.
        double x = m_walks[personIndex]->getScaledUtmX(0);
        double y = m_walks[personIndex]->getScaledUtmY(0);
        double w = m_walks[personIndex]->getScaledUtmX(1) - x;
        double h = m_walks[personIndex]->getScaledUtmY(1) - y;
        ofFill();
        ofSetColor(m_settings->getColorViewboxR(),
                   m_settings->getColorViewboxG(),
                   m_settings->getColorViewboxB());
        ofRect(x, y, w, h);
        if(m_multiMode)
            break;

    }
}

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
            m_magicBoxes[i]->addToBoxSize(-500.0);

        }
        if(m_multiMode)
            m_magicBox->addToBoxSize(-500.0);
        break;
    case '-':
        for(unsigned int i = 0; i < m_magicBoxes.size(); ++i)
        {
            m_magicBoxes[i]->addToBoxSize(500.0);
        }
        if(m_multiMode)
            m_magicBox->addToBoxSize(500.0);
        break;
    case OF_KEY_UP:
        for(unsigned int i = 0; i < m_magicBoxes.size(); ++i)
        {
            m_magicBoxes[i]->goUp(500.0);
        }
        if(m_multiMode)
            m_magicBox->goUp(500.0);
        break;
    case OF_KEY_DOWN:
        for(unsigned int i = 0; i < m_magicBoxes.size(); ++i)
        {
            m_magicBoxes[i]->goDown(500.0);
        }
        if(m_multiMode)
            m_magicBox->goDown(500.0);
        break;
       break;
    case OF_KEY_RIGHT:
        for(unsigned int i = 0; i < m_magicBoxes.size(); ++i)
        {
            m_magicBoxes[i]->goRight(500.0);
        }
        if(m_multiMode)
            m_magicBox->goRight(500.0);
        break;
    case OF_KEY_LEFT:
        for(unsigned int i = 0; i < m_magicBoxes.size(); ++i)
        {
            m_magicBoxes[i]->goLeft(500.0);
        }
        if(m_multiMode)
            m_magicBox->goLeft(500.0);

        break;
    case 32:
        if(m_interactiveMode)
        {
            for (unsigned int i=0; i < m_walks.size();++i)
            {
            	m_walks[i]->updateToNextSegment();
            }
        }
        else
        {
            m_pause = !m_pause;
        }
        break;
    case 8:
        if(m_interactiveMode)
        {
            for (unsigned int i=0; i < m_walks.size();++i)
            {
            	m_walks[i]->updateToPreviousSegment();
            }
        }
        break;
    case 't':
        if(m_interactiveMode)
        {
            for (unsigned int i=0; i < m_walks.size();++i)
            {
            	m_walks[i]->toggleTraced();
            }
        }
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
void DrawingLifeApp::windowResized(int w, int h)
{
    this->setViewAspectRatio();
    for(unsigned int personIndex = 0; personIndex < m_numPerson; ++personIndex)
    {
        if(m_walks[personIndex])
            m_walks[personIndex]->setViewBounds(ofGetWidth(), ofGetHeight(), m_viewXOffset[personIndex], m_viewYOffset[personIndex], m_viewMinDimension[personIndex], m_viewPadding[personIndex]);
    }
    for(unsigned int i = 0; i < m_locationImgs.size(); ++i)
    {
        m_locationImgs[i]->setViewBounds(m_viewMinDimension[0], m_viewPadding[0], m_viewXOffset[0], m_viewYOffset[0]);
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
    for(unsigned int i = 0; i < m_gpsDatas.size(); ++i)
    {
        if (m_gpsDatas[i]->getMinUtmX() < minX) minX = m_gpsDatas[i]->getMinUtmX();
        if (m_gpsDatas[i]->getMaxUtmX() > maxX) maxX = m_gpsDatas[i]->getMaxUtmX();
        if (m_gpsDatas[i]->getMinUtmY() < minY) minY = m_gpsDatas[i]->getMinUtmY();
        if (m_gpsDatas[i]->getMaxUtmY() > maxY) maxY = m_gpsDatas[i]->getMaxUtmY();
        if (m_gpsDatas[i]->getMinLon() < minLon) minLon = m_gpsDatas[i]->getMinLon();
        if (m_gpsDatas[i]->getMaxLon() > maxLon) maxLon = m_gpsDatas[i]->getMaxLon();
        if (m_gpsDatas[i]->getMinLat() < minLat) minLat = m_gpsDatas[i]->getMinLon();
        if (m_gpsDatas[i]->getMaxLat() > maxLat) maxLat = m_gpsDatas[i]->getMaxLon();
    }

    bool isRegionOn = m_settings->isRegionsOn();
    if(isRegionOn)
    {
        GpsData::setGlobalValues(minX, maxX, minY, maxY, 0.0);
    }
    else
    {
        if(m_settings->isMeridianAuto())
            GpsData::setGlobalValues(minX, maxX, minY, maxY, (minLon + (maxLon - minLon)/2));
        else
        {
            double lon0 = 0.0;
            lon0 = m_settings->getMeridian();
            GpsData::setGlobalValues(minX, maxX, minY, maxY, lon0);
        }
    }

    for(unsigned int i = 0; i < m_gpsDatas.size(); ++i)
    {
        m_gpsDatas[i]->calculateUtmPointsGlobalLon(isRegionOn);
        m_gpsDatas[i]->normalizeUtmPointsGlobal();
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

    ofSetColor(0xffffff);
    std::string str = "FPS: "+ofToString((double)fpsToShow, 1);
    ofDrawBitmapString(str, 30.0, ofGetHeight()-30 );
}

void DrawingLifeApp::loadCurrentPointImages()
{
    for(unsigned int i = 0; i < m_imageList.size(); ++i)
    {
        ofImage tmpImg;
        std::string str = m_imageList[i].path;
        float width = m_imageList[i].width;
        float height= m_imageList[i].height;

        if(tmpImg.loadImage(str))
        {
			tmpImg.resize(width, height);
            m_images.push_back(tmpImg);
            tmpImg.clear();
        }
        else
        {
            m_imageAsCurrentPoint = false;
            break;
        }
    }
}
