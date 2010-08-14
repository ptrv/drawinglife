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
    m_zoomX(0.0),
    m_zoomY(0.0),
    m_zoomZ(0.0),
    m_startScreenMode(false),
    m_numPerson(0),
    m_timeline(NULL),
    m_drawSpeed(1),
	m_trackAlpha(64),
	m_dotAlpha(127),
	m_legendAlpha(255),
	m_startScreenDuration(5000),
	m_loadOnStart(0),
	m_frameRate(60)
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
}

void DrawingLifeApp::loadXmlSettings()
{
	// reading settings from xml file
    m_settings.loadFile("AppSettings.xml");

	m_fontTitle.loadFont(m_settings.getValue("ui:font1:name", "mono.ttf"),
						 m_settings.getValue("ui:font1:size1", 50));

    m_fontAuthor.loadFont(m_settings.getValue("ui:font1:name", "mono.ttf"),
                          m_settings.getValue("ui:font1:size2", 24));

    m_fontText.loadFont(m_settings.getValue("ui:font1", "mono.ttf"),
                        m_settings.getValue("ui:font1:size3", 18));

    m_fontInfo.loadFont(m_settings.getValue("ui:font2:name", "Standard0753.ttf"),
                        m_settings.getValue("ui:font2:size1", 8));

    m_colorForeground.r = m_settings.getAttribute("ui:color:foreround", "r", 255);
    m_colorForeground.g = m_settings.getAttribute("ui:color:foreround", "g", 255);
    m_colorForeground.b = m_settings.getAttribute("ui:color:foreround", "b", 255);

    m_colorBackground.r = m_settings.getAttribute("ui:color:background", "r", 0);
    m_colorBackground.g = m_settings.getAttribute("ui:color:background", "g", 0);
    m_colorBackground.b = m_settings.getAttribute("ui:color:background", "b", 0);

    m_colorViewbox.r = m_settings.getAttribute("ui:color:viewbox", "r", 50);
    m_colorViewbox.g = m_settings.getAttribute("ui:color:viewbox", "g", 50);
    m_colorViewbox.b = m_settings.getAttribute("ui:color:viewbox", "b", 50);

    m_logLevel = m_settings.getAttribute("settings:log", "level", 0);

	m_isDebugMode = m_settings.getValue("settings:debugmode", 1);

	m_trackAlpha = m_settings.getValue("ui:alpha:tracks", 64);
    m_dotAlpha = m_settings.getValue("ui:alpha:dots", 127);
    m_legendAlpha = m_settings.getValue("ui:alpha:legend", 255);
    // db path must be absolute path for DBReader (true as second parameter)

    m_dbPath = ofToDataPath(m_settings.getValue("data:database", "test.sqlite"), true);
    DBG_VAL(m_dbPath);

    m_dbQueryData.type = m_settings.getValue("dbquery:type", 4);
    m_dbQueryData.yearStart = m_settings.getValue("dbquery:time:yearstart", 2009);
    m_dbQueryData.yearEnd = m_settings.getValue("dbquery:time:yearend", 2010);
    m_dbQueryData.city = m_settings.getValue("dbquery:city", "Berlin");

    m_settings.pushTag("data");
    m_settings.pushTag("person");
    m_numPerson = m_settings.getNumTags("name");
    for(int i = 0; i < m_numPerson; ++i)
    {
        m_names.push_back(m_settings.getValue("name", "", i));
        m_gpsDatas.push_back(new GpsData());
		m_walks.push_back(new Walk());
		m_magicBoxes.push_back(new MagicBox());
        DBG_VAL(m_names[i]);

        m_viewXOffset.push_back(0);
        m_viewYOffset.push_back(0);
        m_viewMinDimension.push_back(0);
        m_viewPadding.push_back(15);
    }
    m_settings.popTag();
    m_settings.popTag();

	m_drawSpeed = m_settings.getValue("settings:drawspeed", 1)*m_numPerson;
    m_loadOnStart = m_settings.getValue("settings:loadgpsonstart",1);
    m_frameRate = m_settings.getValue("settings:framerate", 60);


}

void DrawingLifeApp::setup()
{
	loadXmlSettings();
    ofBackground(m_colorBackground.r, m_colorBackground.g, m_colorBackground.b);

    ofSetFrameRate(60);
    ofEnableAlphaBlending();

    ofSetLogLevel(m_logLevel);

    DBG_VAL(m_numPerson);
    // -----------------------------------------------------------------------------
    this->setViewAspectRatio();
    // -----------------------------------------------------------------------------
    m_timeline = new Timeline();

    if (m_loadOnStart == 1)
    {
        switch(m_dbQueryData.type)
        {
            case DB_QUERY_CITY:
            loadGpsDataCity(m_names, m_dbQueryData.city);
            break;
            case DB_QUERY_YEAR:
            loadGpsDataYearRange(m_names, m_dbQueryData.yearStart, m_dbQueryData.yearEnd);
            break;
        }
        for(int i = 0; i < m_numPerson; ++i)
        {
			if(m_walks[i]->getGpsData().getTotalGpsPoints() == 0)
            {
                m_startScreenMode = true;
            }
        }
    }
    else
    {
        m_startScreenMode = true;
    }
}

//--------------------------------------------------------------
void DrawingLifeApp::update()
{
    if (m_isAnimation)
    {
        for(int i = 0; i < m_drawSpeed; ++i)
        {
            int id = m_timeline->getNext();
            if (id < m_numPerson)
            {
				m_walks[id]->update();
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
//                    ofSetColor(0xffffff);
                    ofSetColor(255, 255, 255, m_legendAlpha);
					ofDrawBitmapString(m_walks[i]->getCurrentGpsInfoDebug(),30 + (ofGetWidth()/m_numPerson)*i,30);
                }
                else
                {
                    ofSetColor(255, 255, 255, m_legendAlpha);
                    ofSetColor(0xffffff);
                    m_fontInfo.drawString(m_walks[i]->getCurrentGpsInfo(),
                                          m_viewPadding[i] + (ofGetWidth()/m_numPerson)*i ,
                                          m_viewYOffset[i] + 10);
                }
                // -----------------------------------------------------------------------------
                // Draw Gps data
                // -----------------------------------------------------------------------------
                ofSetColor(m_colorForeground.r, m_colorForeground.g, m_colorForeground.b, m_trackAlpha);
                ofNoFill();
                glPushMatrix();
                glTranslated(m_zoomX, m_zoomY, m_zoomZ);
				m_walks[i]->draw();
                glPopMatrix();
            }
        }
        else
        {
            fillViewAreaUTM();
            // -----------------------------------------------------------------------------
            // Draw Gps data
            // -----------------------------------------------------------------------------
            ofSetColor(m_colorForeground.r, m_colorForeground.g, m_colorForeground.b, m_trackAlpha);
            ofNoFill();
            glTranslated(m_zoomX, m_zoomY, m_zoomZ);
            for(int i = 0; i < m_numPerson; ++i)
            {
				m_walks[i]->drawAll();
            }
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

//    m_fontText.drawString("Press key 0 - 9 to choose a life map.", ofGetWidth()/2 - 300, ofGetHeight()/2 + 250);
}
// -----------------------------------------------------------------------------
// Retrieving new GpsData
// -----------------------------------------------------------------------------
void DrawingLifeApp::loadGpsDataCity(vector<string> names, string city)
{
    m_startScreenMode = false;

    // get GpsData from database
    for(int ii = 0; ii < m_numPerson; ++ii)
    {
        SAFE_DELETE(m_gpsDatas[ii]);
        m_gpsDatas[ii] = new GpsData();

        SAFE_DELETE(m_magicBoxes[ii]);
        m_magicBoxes[ii] = new MagicBox();

		SAFE_DELETE(m_walks[ii]);
        m_walks[ii] = new Walk();

		m_walks[ii]->setViewBounds(ofGetWidth(), ofGetHeight(), m_viewXOffset[ii], m_viewYOffset[ii], m_viewMinDimension[ii], m_viewPadding[ii]);
        m_walks[ii]->reset();

        m_dbReader = new DBReader(m_dbPath);
        if (m_dbReader->setupDbConnection())
        {
            // -----------------------------------------------------------------------------
            // DB query
            if(m_dbReader->getGpsDataCity(*m_gpsDatas[ii], names[ii], city))
            {
                ofLog(OF_LOG_SILENT, "--> GpsData load ok!");
                ofLog(OF_LOG_SILENT, "--> Total data: %d GpsSegments, %d GpsPoints!",
                      m_gpsDatas[ii]->getSegments().size(),
                      m_gpsDatas[ii]->getTotalGpsPoints());
				m_walks[ii]->setGpsData(m_gpsDatas[ii]);
				m_walks[ii]->setMagicBox(m_magicBoxes[ii]);
            }
            else
            {
                ofLog(OF_LOG_SILENT, "--> No GpsData loaded!");
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

        Walk::setTrackAlpha(m_dotAlpha);
        for(unsigned int i = 0; i < m_walks.size(); ++i)
        {
            m_walks[i]->setDotColors();
        }

    }
}
void DrawingLifeApp::loadGpsDataYearRange(std::vector<string> names, int yearStart, int yearEnd)
{
    m_startScreenMode = false;

    // get GpsData from database
    for(int ii = 0; ii < m_numPerson; ++ii)
    {
        SAFE_DELETE(m_gpsDatas[ii]);
        m_gpsDatas[ii] = new GpsData();

        SAFE_DELETE(m_magicBoxes[ii]);
        m_magicBoxes[ii] = new MagicBox();

		SAFE_DELETE(m_walks[ii]);
        m_walks[ii] = new Walk();

		m_walks[ii]->setViewBounds(ofGetWidth(), ofGetHeight(), m_viewXOffset[ii], m_viewYOffset[ii], m_viewMinDimension[ii], m_viewPadding[ii]);
        m_walks[ii]->reset();

        m_dbReader = new DBReader(m_dbPath);
        if (m_dbReader->setupDbConnection())
        {
            // -----------------------------------------------------------------------------
            // DB query
            if(m_dbReader->getGpsDataYearRange(*m_gpsDatas[ii], names[ii], yearStart, yearEnd))
            {
                ofLog(OF_LOG_SILENT, "--> GpsData load ok!");
                ofLog(OF_LOG_SILENT, "--> Total data: %d GpsSegments, %d GpsPoints!",
                      m_gpsDatas[ii]->getSegments().size(),
                      m_gpsDatas[ii]->getTotalGpsPoints());
				m_walks[ii]->setGpsData(m_gpsDatas[ii]);
				m_walks[ii]->setMagicBox(m_magicBoxes[ii]);
            }
            else
            {
                ofLog(OF_LOG_SILENT, "--> No GpsData loaded!");
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

        ofLog(m_logLevel, "Start year: %d, end year: %d", yearStart, yearEnd);
        ofLog(OF_LOG_VERBOSE, "minLon: %lf, maxLon: %lf, minLat: %lf, maxLat: %lf",
          m_gpsDatas[ii]->getMinUtmX(),
          m_gpsDatas[ii]->getMaxUtmX(),
          m_gpsDatas[ii]->getMinUtmY(),
          m_gpsDatas[ii]->getMaxUtmY());
        ofLog(OF_LOG_VERBOSE, "Central Meridian: %lf", m_gpsDatas[ii]->getProjectionCentralMeridian());
    }
    if (m_gpsDatas.size() > 0)
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

        Walk::setTrackAlpha(m_dotAlpha);
        for(unsigned int i = 0; i < m_walks.size(); ++i)
        {
            m_walks[i]->setDotColors();
        }

    }
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
        ofSetColor(m_colorViewbox.r, m_colorViewbox.g, m_colorViewbox.b);
        ofRect(x, y, w, h);

    }
}

//--------------------------------------------------------------
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
        break;
    case 'c':
        for(unsigned int i = 0; i < m_walks.size(); ++i)
        {
            m_walks[i]->setDotColors();
        }
        break;
    case 49:
    if(m_dbQueryData.type == DB_QUERY_CITY)
        loadGpsDataCity(m_names, "Berlin");
        break;
    case 50:
    if(m_dbQueryData.type == DB_QUERY_CITY)
        loadGpsDataCity(m_names, "London");
        break;
    case 51:
    if(m_dbQueryData.type == DB_QUERY_CITY)
        loadGpsDataCity(m_names, "Barcelona");
        break;
    case 52:
    if(m_dbQueryData.type == DB_QUERY_CITY)
        loadGpsDataCity(m_names, "Hamburg");
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
        break;
    case OF_KEY_LEFT:

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
