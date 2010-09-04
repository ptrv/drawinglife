/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#include "DrawingLifeApp.h"
#include "GpsData.h"
#include <vector>
#include <limits>

#if defined (WIN32)
#undef max
#endif

//--------------------------------------------------------------
DrawingLifeApp::DrawingLifeApp() :
    m_dbReader(NULL),
    m_isFullscreen(false),
    m_isDebugMode(false),
    m_isAnimation(true),
    m_showFps(false),
    m_zoomX(0.0),
    m_zoomY(0.0),
    m_zoomZ(0.0),
    m_startScreenMode(false),
    m_numPerson(0),
    m_timeline(NULL),
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
	m_interactiveMode(false)
{
}
DrawingLifeApp::~DrawingLifeApp()
{
    for(int i = 0; i < m_numPerson; ++i)
    {
        SAFE_DELETE(m_gpsDatas[i]);
		SAFE_DELETE(m_walks[i]);
		SAFE_DELETE(m_magicBoxes[i]);
    }
    SAFE_DELETE(m_timeline);
    for(unsigned int i = 0; i < m_images.size(); ++i)
    {
        m_images[i].clear();
    }
    m_images.clear();
}

void DrawingLifeApp::setup()
{
    // -----------------------------------------------------------------------------
    AppSettings& settings = AppSettings::instance();

    ofSetLogLevel(settings.getLogLevel());

	m_fontTitle.loadFont(settings.getFontTitleName(),
						 settings.getFontTitleSize());

    m_fontAuthor.loadFont(settings.getFontAuthorName(),
                          settings.getFontAuthorSize());

    m_fontText.loadFont(settings.getFontTextName(),
                        settings.getFontTextSize());

    m_fontInfo.loadFont(settings.getFontInfoName(),
                        settings.getFontInfoSize());

    m_isDebugMode = settings.isDebugMode();
    m_isFullscreen = settings.isFullscreen();
    m_hideCursor = settings.hideCursor();
    m_interactiveMode = settings.isInteractiveMode();

    m_dbQueryData.type = settings.getQueryType();
    m_dbQueryData.yearStart = settings.getQueryYearStart();
    m_dbQueryData.yearEnd = settings.getQueryYearEnd();
    m_dbQueryData.city = settings.getQueryCity();

    m_imageAsCurrentPoint = settings.isCurrentPointImage();

    if(m_imageAsCurrentPoint)
    {
        m_imageList = settings.getImageList();
    }

    m_numPerson = settings.getNumPerson();
    m_names = settings.getNames();
    for(int i = 0; i < m_numPerson; ++i)
    {
//        m_names.push_back(m_settings.getValue("name", "", i));
        m_gpsDatas.push_back(NULL);
		m_walks.push_back(NULL);
		m_magicBoxes.push_back(NULL);
        DBG_VAL(m_names[i]);

        m_viewXOffset.push_back(0);
        m_viewYOffset.push_back(0);
        m_viewMinDimension.push_back(0);
        m_viewPadding.push_back(15);
    }

    m_dbPath = settings.getDatabasePath();
    DBG_VAL(m_dbPath);

    ofBackground(settings.getColorBackgroundR(),
                 settings.getColorBackgroundG(),
                 settings.getColorBackgroundB());

    ofSetFrameRate(settings.getFrameRate());

    ofEnableAlphaBlending();


    DBG_VAL(m_numPerson);
    // -----------------------------------------------------------------------------
    this->setViewAspectRatio();
    // -----------------------------------------------------------------------------
    loadCurrentPointImages();

    m_timeline = new Timeline();

    if (settings.loadGpsOnStart() == 1)
    {
        bool dbOk = false;
        switch(m_dbQueryData.type)
        {
            case DB_QUERY_CITY:
            loadGpsDataCity(m_names, m_dbQueryData.city);
            break;
            case DB_QUERY_YEAR:
            dbOk = loadGpsDataYearRange(m_names, m_dbQueryData.yearStart, m_dbQueryData.yearEnd);
//            break;
        }
        for(int i = 0; i < m_numPerson; ++i)
        {
            if(dbOk)
            {
                if(m_walks[i]->getGpsData().getTotalGpsPoints() == 0)
                {
                    m_startScreenMode = true;
                }
            }
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
void DrawingLifeApp::update()
{
    if (m_isAnimation)
    {
        if(!m_interactiveMode)
        {
            if(m_timeline->getTimeline().size() > 0)
            {
                for(int i = 0; i < AppSettings::instance().getDrawSpeed(); ++i)
                {
                    int id = m_timeline->getNext();
                    if (id < m_numPerson)
                    {
                        m_walks[id]->update();
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
        if (m_isAnimation)
        {
            // -----------------------------------------------------------------------------
            // Draw rectangle with text.
            // -----------------------------------------------------------------------------
            fillViewAreaUTM();
            //---------------------------------------------------------------------------
            for(int i = 0; i < m_numPerson; ++i)
            {
                if (m_isDebugMode)
                {
                    ofSetColor(255, 255, 255, AppSettings::instance().getAlphaLegend());
					ofDrawBitmapString(m_walks[i]->getCurrentGpsInfoDebug(),30 + (ofGetWidth()/m_numPerson)*i,30);
                }
                else
                {
                    ofSetColor(255, 255, 255, AppSettings::instance().getAlphaLegend());
                    ofSetColor(0xffffff);
                    m_fontInfo.drawString(m_walks[i]->getCurrentGpsInfo(),
                                          m_viewPadding[i] + (ofGetWidth()/m_numPerson)*i ,
                                          m_viewYOffset[i] + 10);
                }
                // -----------------------------------------------------------------------------
                // Draw Gps data
                // -----------------------------------------------------------------------------
                ofSetColor(AppSettings::instance().getColorForegroundR(),
                           AppSettings::instance().getColorForegroundG(),
                           AppSettings::instance().getColorForegroundB(),
                           AppSettings::instance().getAlphaTrack());
                ofNoFill();
				m_walks[i]->draw();
            }
        }
        else
        {
            fillViewAreaUTM();
            // -----------------------------------------------------------------------------
            // Draw Gps data
            // -----------------------------------------------------------------------------
            ofSetColor(AppSettings::instance().getColorForegroundR(),
                       AppSettings::instance().getColorForegroundG(),
                       AppSettings::instance().getColorForegroundB(),
                       AppSettings::instance().getAlphaTrack());
            ofNoFill();
            for(int i = 0; i < m_numPerson; ++i)
            {
				m_walks[i]->drawAll();
            }
        }
        if(m_showFps)
            fpsDisplay();
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
// -----------------------------------------------------------------------------
// Retrieving new GpsData
// -----------------------------------------------------------------------------
bool DrawingLifeApp::loadGpsDataCity(vector<string> names, string city)
{
    m_startScreenMode = false;

    for( int ii = 0; ii < m_numPerson; ++ii)
    {
        SAFE_DELETE(m_gpsDatas[ii]);
        SAFE_DELETE(m_magicBoxes[ii]);
		SAFE_DELETE(m_walks[ii]);
    }

    bool dbOk = false;

    // get GpsData from database
    for(int ii = 0; ii < m_numPerson; ++ii)
    {
        m_gpsDatas[ii] = new GpsData();

        m_magicBoxes[ii] = new MagicBox(AppSettings::instance().getBoundingBoxSize(),
                                        AppSettings::instance().getBoundingBoxPadding());

        m_walks[ii] = new Walk(AppSettings::instance().getWalkLength(), AppSettings::instance().getNameColors()[ii]);

		m_walks[ii]->setViewBounds(ofGetWidth(), ofGetHeight(), m_viewXOffset[ii], m_viewYOffset[ii], m_viewMinDimension[ii], m_viewPadding[ii]);
        m_walks[ii]->reset();

        m_dbReader = new DBReader(m_dbPath);
        if (m_dbReader->setupDbConnection())
        {
            // -----------------------------------------------------------------------------
            // DB query
            dbOk = m_dbReader->getGpsDataCity(*m_gpsDatas[ii], names[ii], city);
            if(dbOk)
            {
                ofLog(OF_LOG_SILENT, "--> GpsData load ok!");
                ofLog(OF_LOG_SILENT, "--> Total data: %d GpsSegments, %d GpsPoints!\n",
                      m_gpsDatas[ii]->getSegments().size(),
                      m_gpsDatas[ii]->getTotalGpsPoints());
				m_walks[ii]->setGpsData(m_gpsDatas[ii]);
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
        for (unsigned int i = 0; i < m_gpsDatas[ii]->getSegments().size(); ++i)
        {
            for (unsigned int j = 0; j < m_gpsDatas[ii]->getSegments()[i].getPoints().size(); ++j)
            {
                ++maxPoints;
            }
        }

        DBG_VAL(city);
        ofLog(OF_LOG_VERBOSE, "minLon: %lf, maxLon: %lf, minLat: %lf, maxLat: %lf",
          m_gpsDatas[ii]->getMinUtmX(),
          m_gpsDatas[ii]->getMaxUtmX(),
          m_gpsDatas[ii]->getMinUtmY(),
          m_gpsDatas[ii]->getMaxUtmY());
        ofLog(OF_LOG_VERBOSE, "Central Meridian: %lf", m_gpsDatas[ii]->getProjectionCentralMeridian());
    }
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

        Walk::setTrackAlpha(AppSettings::instance().getAlphaDot());

        Walk::setDotSize(AppSettings::instance().getDotSize());

        for(unsigned int i = 0; i < m_walks.size(); ++i)
        {
            m_walks[i]->setDotColors();

            if(!AppSettings::instance().isBoundingBoxEnabled())
                m_walks[i]->setMagicBoxStatic(m_magicBoxes[i]);
            else
                m_walks[i]->setMagicBox(m_magicBoxes[i]);

            if (m_imageAsCurrentPoint && (int)m_images.size() >= m_numPerson)
            {
                m_walks[i]->setCurrentPointImage(m_images[i]);
            }
        }
    }
    return dbOk;
}
bool DrawingLifeApp::loadGpsDataYearRange(std::vector<string> names, int yearStart, int yearEnd)
{
    m_startScreenMode = false;

    // get GpsData from database
    for( int ii = 0; ii < m_numPerson; ++ii)
    {
        SAFE_DELETE(m_gpsDatas[ii]);
        SAFE_DELETE(m_magicBoxes[ii]);
		SAFE_DELETE(m_walks[ii]);
    }

    bool dbOk = false;

    for(int ii = 0; ii < m_numPerson; ++ii)
    {
        m_gpsDatas[ii] = new GpsData();

        m_magicBoxes[ii] = new MagicBox(AppSettings::instance().getBoundingBoxSize(),
                                        AppSettings::instance().getBoundingBoxPadding());

        m_walks[ii] = new Walk(AppSettings::instance().getWalkLength(), AppSettings::instance().getNameColors()[ii]);

		m_walks[ii]->setViewBounds(ofGetWidth(), ofGetHeight(), m_viewXOffset[ii], m_viewYOffset[ii], m_viewMinDimension[ii], m_viewPadding[ii]);
        m_walks[ii]->reset();

        m_dbReader = new DBReader(m_dbPath);

        if (m_dbReader->setupDbConnection())
        {
            // -----------------------------------------------------------------------------
            // DB query
            dbOk = m_dbReader->getGpsDataYearRange(*m_gpsDatas[ii], names[ii], yearStart, yearEnd);
            if(dbOk)
            {
                ofLog(OF_LOG_SILENT, "--> GpsData set %d: load ok!", ii+1);
                ofLog(OF_LOG_SILENT, "--> Total data: %d GpsSegments, %d GpsPoints!\n",
                      m_gpsDatas[ii]->getSegments().size(),
                      m_gpsDatas[ii]->getTotalGpsPoints());
				m_walks[ii]->setGpsData(m_gpsDatas[ii]);
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
        for (unsigned int i = 0; i < m_gpsDatas[ii]->getSegments().size(); ++i)
        {
            for (unsigned int j = 0; j < m_gpsDatas[ii]->getSegments()[i].getPoints().size(); ++j)
            {
                ++maxPoints;
            }
        }

        ofLog(OF_LOG_VERBOSE, "Start year: %d, end year: %d", yearStart, yearEnd);
        ofLog(OF_LOG_VERBOSE, "minLon: %lf, maxLon: %lf, minLat: %lf, maxLat: %lf",
          m_gpsDatas[ii]->getMinUtmX(),
          m_gpsDatas[ii]->getMaxUtmX(),
          m_gpsDatas[ii]->getMinUtmY(),
          m_gpsDatas[ii]->getMaxUtmY());
        ofLog(OF_LOG_VERBOSE, "Central Meridian: %lf\n", m_gpsDatas[ii]->getProjectionCentralMeridian());
    }
    ofLog(OF_LOG_VERBOSE, "------------------------\n");
    if (m_gpsDatas.size() > 0 && dbOk)
    {
        m_timeline->setTimeline(m_gpsDatas);

//        ofstream myfile;
//        myfile.open ("timeline_output.txt");
//        for(unsigned int i = 0; i < m_timeline->getTimeline().size(); ++i)
//        {
////            ofLog(OF_LOG_VERBOSE, "%s : %d : %li", m_timeline->getTimeline()[i].timeString.c_str(),
////                                                    m_timeline->getTimeline()[i].id,
////                                                    (long)(m_timeline->getTimeline()[i].secs));
//            myfile << m_timeline->getTimeline()[i].timeString.c_str()
//            << " : " << m_timeline->getTimeline()[i].id
//            << " : " << (long)(m_timeline->getTimeline()[i].secs) << "\n";
//
//        }
//        myfile.close();

        this->calculateGlobalMinMaxValues();

        Walk::setTrackAlpha(AppSettings::instance().getAlphaDot());

        Walk::setDotSize(AppSettings::instance().getDotSize());

        for(unsigned int i = 0; i < m_walks.size(); ++i)
        {
            m_walks[i]->setDotColors();

            if(!AppSettings::instance().isBoundingBoxEnabled())
                m_walks[i]->setMagicBoxStatic(m_magicBoxes[i]);
            else
                m_walks[i]->setMagicBox(m_magicBoxes[i]);

            if (m_imageAsCurrentPoint && (int)m_images.size() >= m_numPerson)
            {
                m_walks[i]->setCurrentPointImage(m_images[i]);
            }
        }
    }
    return dbOk;
}
// -----------------------------------------------------------------------------

void DrawingLifeApp::setViewAspectRatio()
{
    double width = ofGetWidth()/(int)m_numPerson;
    double height = ofGetHeight();

    for(int i = 0; i < m_numPerson; ++i)
    {
        // Reset for view padding.
        m_viewXOffset[i] = 0;
        m_viewYOffset[i] = 0;

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

        m_viewXOffset[i] += width * i;
//        m_viewYOffset += m_viewPadding;
    }
}
void DrawingLifeApp::fillViewAreaUTM()
{
    for(int i = 0; i < m_numPerson; ++i)
    {
        // Normalized value range from 0 to 1.
        double x = m_walks[i]->getScaledUtmX(0);
        double y = m_walks[i]->getScaledUtmY(0);
        double w = m_walks[i]->getScaledUtmX(1) - x;
        double h = m_walks[i]->getScaledUtmY(1) - y;
        ofFill();
        ofSetColor(AppSettings::instance().getColorViewboxR(),
                   AppSettings::instance().getColorViewboxG(),
                   AppSettings::instance().getColorViewboxB());
        ofRect(x, y, w, h);

    }
}

//--------------------------------------------------------------
void DrawingLifeApp::keyPressed  (int key)
{
    //DBG_VAL(key);
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
    case 'c':
        for(unsigned int i = 0; i < m_walks.size(); ++i)
        {
            m_walks[i]->setDotColors();
        }
        break;
    case 49:
    if(m_dbQueryData.type == DB_QUERY_CITY)
    {
        loadGpsDataCity(m_names, "Berlin");
    }
    else if(m_dbQueryData.type != DB_QUERY_CITY)
    {
        for(unsigned int i = 0; i < m_magicBoxes.size(); ++i)
        {
            m_magicBoxes[i]->toggleZoomLevel(1);
        }
    }
    break;
    case 50:
    if(m_dbQueryData.type == DB_QUERY_CITY)
    {
        loadGpsDataCity(m_names, "London");
    }
    else
    {
        for(unsigned int i = 0; i < m_magicBoxes.size(); ++i)
        {
            m_magicBoxes[i]->toggleZoomLevel(2);
        }
    }
    break;
    case 51:
    if(m_dbQueryData.type == DB_QUERY_CITY)
    {
        loadGpsDataCity(m_names, "Barcelona");
    }
    else
    {
        for(unsigned int i = 0; i < m_magicBoxes.size(); ++i)
        {
            m_magicBoxes[i]->toggleZoomLevel(3);
        }
    }
    break;
    case 52:
    if(m_dbQueryData.type == DB_QUERY_CITY)
    {
        loadGpsDataCity(m_names, "Hamburg");
    }
    else
    {
        for(unsigned int i = 0; i < m_magicBoxes.size(); ++i)
        {
            m_magicBoxes[i]->toggleZoomLevel(4);
        }
    }
    break;
    case 53:
    if(m_dbQueryData.type == DB_QUERY_CITY)
        loadGpsDataCity(m_names, "Vienna");
        break;
    case 54:
    if(m_dbQueryData.type == DB_QUERY_CITY)
        loadGpsDataCity(m_names,"New York");
        break;
    case 55:
    if(m_dbQueryData.type == DB_QUERY_CITY)
        loadGpsDataCity(m_names, "Tokyo");
        break;
    case 56:
    if(m_dbQueryData.type == DB_QUERY_CITY)
        loadGpsDataCity(m_names, "San Francisco");
        break;
    case 57:
    if(m_dbQueryData.type == DB_QUERY_CITY)
        loadGpsDataCity(m_names, "Bristol");
        break;
    case 48:
    if(m_dbQueryData.type == DB_QUERY_CITY)
        loadGpsDataCity(m_names, "Banff");
        break;
    case 'w':
        break;
    case 's':
        DBG_VAL(m_zoomZ);
        break;
    case OF_KEY_UP:
        for(unsigned int i = 0; i < m_magicBoxes.size(); ++i)
        {
            m_magicBoxes[i]->addToBoxSize(-500.0);
        }
        break;
    case OF_KEY_DOWN:
        for(unsigned int i = 0; i < m_magicBoxes.size(); ++i)
        {
            m_magicBoxes[i]->addToBoxSize(500.0);
        }
        break;
    case OF_KEY_RIGHT:
        if(m_interactiveMode)
        {
            for (unsigned int i=0; i < m_walks.size();++i)
            {
            	m_walks[i]->updateToNextSegment();
            }
        }
        break;
    case OF_KEY_LEFT:
        if(m_interactiveMode)
        {
            for (unsigned int i=0; i < m_walks.size();++i)
            {
            	m_walks[i]->updateToPreviousSegment();
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
    for(int i = 0; i < m_numPerson; ++i)
    {
        if(m_walks[i])
            m_walks[i]->setViewBounds(ofGetWidth(), ofGetHeight(), m_viewXOffset[i], m_viewYOffset[i], m_viewMinDimension[i], m_viewPadding[i]);
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
    GpsData::setGlobalValues(minX, maxX, minY, maxY, (minLon + (maxLon - minLon)/2));
    for(unsigned int i = 0; i < m_gpsDatas.size(); ++i)
    {
        m_gpsDatas[i]->calculateUtmPointsGlobalLon();
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
    ofDrawBitmapString(str, 20.0, ofGetHeight()-30 );
}

void DrawingLifeApp::loadCurrentPointImages()
{
    for(unsigned int i = 0; i < m_imageList.size(); ++i)
    {
        ofImage tmpImg;
        std::string str = "currentPointImages/"+m_imageList[i];
        if(tmpImg.loadImage(str))
        {
			tmpImg.resize(10, 10);
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
