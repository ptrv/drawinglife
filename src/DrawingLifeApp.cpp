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
//    m_settings(NULL),
    m_dbReader(NULL),
    //m_gpsData(NULL),
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
    m_counter(NULL),
    m_drawCycle(0)
{
    m_viewXOffset = 0;
    m_viewYOffset = 0;
    m_viewMinDimension = 0;
    m_viewPadding = 15;

}
DrawingLifeApp::~DrawingLifeApp()
{
    for(unsigned int i = 0; i < m_gpsDatas.size(); ++i)
    {
        SAFE_DELETE(m_gpsDatas[i]);
    }
    SAFE_DELETE(m_timeline);
    SAFE_DELETE(m_counter);
}
void DrawingLifeApp::setup()
{
    m_counter = new TimedCounter(0,std::numeric_limits<int>::max(),1000);

    ofBackground((BACKGROUND >> 16) & 0xFF, (BACKGROUND >> 8) & 0xFF, (BACKGROUND) & 0xFF);

    ofSetFrameRate(60);
    ofEnableAlphaBlending();

    // reading settings from xml file
    m_settings.loadFile("AppSettings.xml");

	m_fontTitle.loadFont(m_settings.getValue("ui:font1", "mono.ttf"), 50);
    m_fontAuthor.loadFont(m_settings.getValue("ui:font1", "mono.ttf"),24);
    m_fontText.loadFont(m_settings.getValue("ui:font1", "mono.ttf"), 18);
    m_fontInfo.loadFont(m_settings.getValue("ui:font2", "Standard0753.ttf"), 8);

    ofSetLogLevel(m_settings.getAttribute("settings:log", "level", 0));
//    ofSetLogLevel(OF_LOG_VERBOSE);

    m_isDebugMode = m_settings.getValue("settings:debugmode", 1);

    // db path must be absolute path for DBReader (true as second parameter)
    m_dbPath = ofToDataPath(m_settings.getValue("data:database", "test.sqlite"), true);
    DBG_VAL(m_dbPath);

    string city = m_settings.getValue("data:defaultcity", "London");

    m_settings.pushTag("data");
    m_settings.pushTag("person");
    m_numPerson = m_settings.getNumTags("name");
    for(int i = 0; i < m_numPerson; ++i)
    {
        m_names.push_back(m_settings.getValue("name", "", i));
        m_gpsDatas.push_back(new GpsData());
        DBG_VAL(m_names[i]);

//        m_viewXOffset.push_back(0);
//        m_viewYOffset.push_back(0);
//        m_viewMinDimension.push_back(0);
//        m_viewPadding.push_back(15);
    }
    m_settings.popTag();
    m_settings.popTag();
    DBG_VAL(m_numPerson);
    // -----------------------------------------------------------------------------
    this->setViewAspectRatio();
    // -----------------------------------------------------------------------------
    m_timeline = new Timeline();

    m_drawSpeed = m_settings.getValue("settings:drawspeed", 1);

    if (m_settings.getValue("settings:loadgpsonstart",1) == 1)
    {
        loadGpsDataCity(m_names, city);

        for(unsigned int i = 0; i < m_gpsDatas.size(); ++i)
        {
            if(m_gpsDatas[i]->getTotalGpsPoints() == 0)
            {
                m_startScreenMode = true;
            }
        }
        m_counter->startCount();
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
            if (m_timeline->isLast())
            {
                ++m_drawCycle;
                if(m_drawCycle == 2)
                {
                    m_counter->startCount();
                    m_drawCycle = 0;
                }
            }
            if (id < (int)m_gpsDatas.size())
            {
                m_gpsDatas[id]->update();
            }
        }
    }
   	if(m_counter->isCounting()){
		m_counter->update();
		if(m_counter->hasChanged())
			cout << "counter changed:" << m_counter->getCurrentCount() << endl;
		if(m_counter->isCountComplete())
		{
			cout << "counter complete!" << endl;
			m_counter->startCount();
			cout << "start counter!" << endl;
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
            //fillViewAreaUTM( VIEWBOX);
            //---------------------------------------------------------------------------
            if (m_gpsDatas.size() > 0)
            {
                ofSetColor(0xffffff);
                m_fontInfo.drawString((m_gpsDatas[0]->getCurrentGpsInfo()).substr(0,10), 30, 30);
                m_fontInfo.drawString("----------", 30, 40);
            }
            for(unsigned int i = 0; i < m_gpsDatas.size(); ++i)
            {
                if (m_isDebugMode)
                {
//                    ofFill();
//                    int color = 0xE5A93F;
//                    ofSetColor((color >> 16) & 0xff,(color >> 8) & 0xff, color & 0xff, 160);
//                    ofRect(10 + (ofGetWidth()/m_numPerson)*i,10,350,200);
                    ofSetColor(0xffffff);
                    ofDrawBitmapString(m_gpsDatas[i]->getCurrentGpsInfoDebug(),30 + (ofGetWidth()/m_numPerson)*i,30);
                }
                else
                {
                    ofSetColor(m_gpsDatas[i]->getDotColor().r,
                               m_gpsDatas[i]->getDotColor().g,
                               m_gpsDatas[i]->getDotColor().b,
                               m_gpsDatas[i]->getDotColor().a );
                    ofFill();
                    ofCircle(15, 77 + 30*i,5);
                    ofSetColor(0xffffff);
//                    m_fontInfo.drawString(m_gpsDatas[i]->getCurrentGpsInfo(),
//                                          m_viewPadding + (ofGetWidth()/m_numPerson)*i ,
//                                          m_viewYOffset + 10);
                    m_fontInfo.drawString((m_gpsDatas[i]->getCurrentGpsInfo()).substr(11), 30 , 80 + 30*i);
                }
                // -----------------------------------------------------------------------------
                // Draw Gps data
                // -----------------------------------------------------------------------------
                //ofSetColor(FOREGROUND);
                ofSetColor((FOREGROUND >> 16) & 0xff, (FOREGROUND >> 8) & 0xff, FOREGROUND & 0xff, 64);
                ofNoFill();
                glPushMatrix();
                glTranslated(m_zoomX, m_zoomY, m_zoomZ);
                m_gpsDatas[i]->draw();
                glPopMatrix();
            }
        }
        else
        {
            //fillViewAreaUTM( VIEWBOX);
            // -----------------------------------------------------------------------------
            // Draw Gps data
            // -----------------------------------------------------------------------------
//            ofSetColor(FOREGROUND);
            ofSetColor((FOREGROUND >> 16) & 0xff, (FOREGROUND >> 8) & 0xff, FOREGROUND & 0xff, 100);
            ofNoFill();
            glTranslated(m_zoomX, m_zoomY, m_zoomZ);
            for(unsigned int i = 0; i < m_gpsDatas.size(); ++i)
            {
                m_gpsDatas[i]->draw(false);

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

//    std::string title = APP_NAME_STR;
//    title += " ";
//    title += APP_VERSION_STR;
//    m_fontTitle.drawString(title, ofGetWidth()/2 - 365, ofGetHeight()/2 - 100);
//
//    m_fontAuthor.drawString(APP_AUTHOR_STR, ofGetWidth()/2 - 91, ofGetHeight()/2);
//
//    m_fontText.drawString("Press key 0 - 9 to choose a life map.", ofGetWidth()/2 - 300, ofGetHeight()/2 + 250);
    std::string title = "The Monday Walks";
    m_fontTitle.drawString(title, ofGetWidth()/2 - 380, ofGetHeight()/2 - 100);
    std::string author = "plan b + ";
    author += APP_AUTHOR_STR;
    m_fontAuthor.drawString(author, ofGetWidth()/2 - 190, ofGetHeight()/2 + 70);

    m_fontText.drawString("Movement in Urban Space, 2010", ofGetWidth()/2 - 250, ofGetHeight()/2 + 250);
}
// -----------------------------------------------------------------------------
// Retrieving new GpsData
// -----------------------------------------------------------------------------
void DrawingLifeApp::loadGpsDataCity(vector<string> names, string city)
{
    m_startScreenMode = false;
    // get GpsData from database
//    m_gpsData->clear();

    for(unsigned int ii = 0; ii < m_gpsDatas.size(); ++ii)
    {
        SAFE_DELETE(m_gpsDatas[ii]);
        m_gpsDatas[ii] = new GpsData();
        m_gpsDatas[ii]->setViewBounds(ofGetWidth(), ofGetHeight(), m_viewXOffset, m_viewYOffset, m_viewMinDimension, m_viewPadding);
        m_gpsDatas[ii]->reset();

        m_dbReader = new DBReader(m_dbPath);
        if (m_dbReader->setupDbConnection())
        {
            // -----------------------------------------------------------------------------
            // DB query
            // TODO: Query needs to match with original database query used in the setup function.
//            if(m_dbReader->getGpsDataYear(*m_gpsDatas[ii], names[ii], 2007))
            if(m_dbReader->getGpsDataCity(*m_gpsDatas[ii], names[ii], city))
            {
                ofLog(OF_LOG_SILENT, "--> GpsData load ok!");
                ofLog(OF_LOG_SILENT, "--> Total data: %d GpsSegments, %d GpsPoints!",
                      m_gpsDatas[ii]->getSegments().size(),
                      m_gpsDatas[ii]->getTotalGpsPoints());
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
    //            stringstream message;
    //            //message << "Value i " << i << ", j " << j << ", k " << k <<": ";
    //            message << "GpsPoint nr " << maxPoints << ": ";
    //            message << m_gpsData->getSegments()[i].getPoints()[j].getUtmY();
    //            message << ", ";
    //            message << m_gpsData->getSegments()[i].getPoints()[j].getUtmX();
    //            message << ", ";
    //            message << m_gpsData->getSegments()[i].getPoints()[j].getElevation();
    //            message << ", ";
    //            message << m_gpsData->getSegments()[i].getPoints()[j].getTimestamp();
    //            message << ", ";
    //            message << m_gpsData->getSegments()[i].getSegmentNum();
    //            ofLog(OF_LOG_NOTICE, message.str() );
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

    }


}
// -----------------------------------------------------------------------------

void DrawingLifeApp::setViewAspectRatio()
{
//    double width = ofGetWidth()/(int)m_numPerson;
    double width = ofGetWidth();
    double height = ofGetHeight();

//    for(int i = 0; i < m_numPerson; ++i)
//    {
//        // Reset for view padding.
//        m_viewXOffset[i] = 0;
//        m_viewYOffset[i] = 0;
//
//        // Set square view area and center.
//        if (height < width)
//        {
//            m_viewMinDimension[i] = height;
//            m_viewXOffset[i] = (width - height) / 2.0;
//        }
//        else if (width < height)
//        {
//            m_viewMinDimension[i] = width;
//            m_viewYOffset[i] = (height - width) / 2.0;
//        }
//        else
//        {
//            m_viewMinDimension[i] = width;
//        }
//
//        // Left and top indentation.
//        m_viewXOffset[i] += m_viewPadding[i];
//        m_viewYOffset[i] += m_viewPadding[i];
//
//        m_viewXOffset[i] += width * i;
////        m_viewYOffset += m_viewPadding;
        // Reset for view padding.
        m_viewXOffset = 0;
        m_viewYOffset = 0;

        // Set square view area and center.
        if (height < width)
        {
            m_viewMinDimension = height;
            m_viewXOffset = (width - height) / 2.0;
        }
        else if (width < height)
        {
            m_viewMinDimension = width;
            m_viewYOffset = (height - width) / 2.0;
        }
        else
        {
            m_viewMinDimension = width;
        }

        // Left and top indentation.
        m_viewXOffset += m_viewPadding;
        m_viewYOffset += m_viewPadding;

//        m_viewXOffset += width * i;
//        m_viewYOffset += m_viewPadding;
//    }
}
void DrawingLifeApp::fillViewAreaUTM( int backgroundColor)
{
//    for(int i = 0; i < m_numPerson; ++i)
//    {
//        // Normalized value range from 0 to 1.
//        double x = m_gpsDatas[i]->getScaledUtmX(0);
//        double y = m_gpsDatas[i]->getScaledUtmY(0);
//        double w = m_gpsDatas[i]->getScaledUtmX(1) - x;
//        double h = m_gpsDatas[i]->getScaledUtmY(1) - y;
//        ofFill();
//        ofSetColor( backgroundColor);
//        ofRect(x, y, w, h);
//
//    }
    if(m_gpsDatas.size() > 0)
    {
        // Normalized value range from 0 to 1.
        double x = m_gpsDatas[0]->getScaledUtmX(0);
        double y = m_gpsDatas[0]->getScaledUtmY(0);
        double w = m_gpsDatas[0]->getScaledUtmX(1) - x;
        double h = m_gpsDatas[0]->getScaledUtmY(1) - y;
        ofFill();
        ofSetColor( backgroundColor);
        ofRect(x, y, w, h);

    }
}

//--------------------------------------------------------------
void DrawingLifeApp::keyPressed  (int key)
{
    DBG_VAL(key);
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
    case 32:
        loadGpsDataCity(m_names, "Leipzig");
        if (!m_counter->isCounting())
        {
            m_counter->startCount();
        }

        break;
//    case 49:
//        loadGpsDataCity(m_names, "Berlin");
//        break;
//    case 50:
//        loadGpsDataCity(m_names, "London");
//        break;
//    case 51:
//        loadGpsDataCity(m_names, "Barcelona");
//        break;
//    case 52:
//        loadGpsDataCity(m_names, "Hamburg");
//        break;
//    case 53:
//        loadGpsDataCity(m_names, "Vienna");
//        break;
//    case 54:
//        loadGpsDataCity(m_names,"New York");
//        break;
//    case 55:
//        loadGpsDataCity(m_names, "Tokyo");
//        break;
//    case 56:
//        loadGpsDataCity(m_names, "San Francisco");
//        break;
//    case 57:
//        loadGpsDataCity(m_names, "Bristol");
//        break;
//    case 48:
//        loadGpsDataCity(m_names, "Banff");
//        break;
    case 'w':
        if(m_zoomZ > 590 && m_zoomZ < 598)
        {
            m_zoomZ += 1;
        }
        else if (m_zoomZ >= 598)
        {
            m_zoomZ += 0.05;
        }
        else if (m_zoomZ < 599)
        {
            m_zoomZ += 10;
        }
        DBG_VAL(m_zoomZ);
        break;
    case 's':
        if(m_zoomZ > 590)
        {
            m_zoomZ -= 1;
        }
        else
        {
            m_zoomZ -= 10;
        }
        DBG_VAL(m_zoomZ);
        break;
    case OF_KEY_UP:
        m_zoomY += 50;
        break;
    case OF_KEY_DOWN:
        m_zoomY -= 50;
        break;
    case OF_KEY_RIGHT:
        m_zoomX -= 50;
        break;
    case OF_KEY_LEFT:
        m_zoomX += 50;
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
    for(unsigned int i = 0; i < m_gpsDatas.size(); ++i)
    {
        m_gpsDatas[i]->setViewBounds(ofGetWidth(), ofGetHeight(), m_viewXOffset, m_viewYOffset, m_viewMinDimension, m_viewPadding);
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
