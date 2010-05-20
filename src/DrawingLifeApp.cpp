/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#include "DrawingLifeApp.h"
#include "GpsData.h"
#include <vector>
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
    m_numPerson(0)
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
}
void DrawingLifeApp::setup()
{
    ofBackground((BACKGROUND >> 16) & 0xFF, (BACKGROUND >> 8) & 0xFF, (BACKGROUND) & 0xFF);
    this->setViewAspectRatio();

    ofSetFrameRate(60);
    ofEnableAlphaBlending();

    // reading settings from xml file
    m_settings.loadFile("AppSettings.xml");

	m_fontTitle.loadFont(m_settings.getValue("settings:font", "mono.ttf"), 50);
    m_fontAuthor.loadFont(m_settings.getValue("settings:font", "mono.ttf"),24);
    m_fontText.loadFont(m_settings.getValue("settings:font", "mono.ttf"), 18);

    ofSetLogLevel(m_settings.getAttribute("settings:log", "level", 0));
//    ofSetLogLevel(OF_LOG_VERBOSE);

    // db path must be absolute path for DBReader (true as second parameter)
    m_dbPath = ofToDataPath(m_settings.getValue("settings:database", "test.sqlite"), true);

    DBG_VAL(m_dbPath);
//    m_gpsData = new GpsData();

    string city = m_settings.getValue("data:defaultcity", "London");

    m_settings.pushTag("data");
    m_settings.pushTag("person");
    m_numPerson = m_settings.getNumTags("name");
    for(int i = 0; i < m_numPerson; ++i)
    {
        m_names.push_back(m_settings.getValue("name", "", i));
        m_gpsDatas.push_back(new GpsData());
        DBG_VAL(m_names[i]);
    }
    m_settings.popTag();
    m_settings.popTag();
    DBG_VAL(m_numPerson);

    if (m_settings.getValue("settings:loadgpsonstart",1) == 1)
    {
        for(unsigned int i = 0; i < m_gpsDatas.size(); ++i)
        {
            loadGpsDataCity(m_names, city);
            if(m_gpsDatas[i]->getTotalGpsPoints() == 0)
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
    for(unsigned int i = 0; i < m_gpsDatas.size(); ++i)
    {
        m_gpsDatas[i]->update();
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
            fillViewAreaUTM( VIEWBOX);
            //---------------------------------------------------------------------------
            ofFill();
            ofSetColor(0xE5A93F);
            ofRect(10,10,320,120);
            ofSetColor(0x000000);
            for(unsigned int i = 0; i < m_gpsDatas.size(); ++i)
            {
                ofDrawBitmapString(m_gpsDatas[i]->getCurrentGpsInfo(),30,30);
                // -----------------------------------------------------------------------------
                // Draw Gps data
                // -----------------------------------------------------------------------------
                ofSetColor(FOREGROUND);
                ofNoFill();
                glTranslated(m_zoomX, m_zoomY, m_zoomZ);
                m_gpsDatas[i]->draw();

            }
        }
        else
        {
            fillViewAreaUTM( VIEWBOX);
            // -----------------------------------------------------------------------------
            // Draw Gps data
            // -----------------------------------------------------------------------------
            ofSetColor(FOREGROUND);
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

    string title = APP_NAME_STR;
    title += " ";
    title += APP_VERSION_STR;
    m_fontTitle.drawString(title, ofGetWidth()/2 - 365, ofGetHeight()/2 - 100);

    m_fontAuthor.drawString(APP_AUTHOR_STR, ofGetWidth()/2 - 91, ofGetHeight()/2);

    m_fontText.drawString("Press key 0 - 9 to choose a life map.", ofGetWidth()/2 - 300, ofGetHeight()/2 + 250);
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
    //        if(m_dbReader->getGpsDataDayRange(*m_gpsData, "Dan", 2010, 2, m_currentSelectedDayStart, m_currentSelectedDayEnd))
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

}
// -----------------------------------------------------------------------------

void DrawingLifeApp::setViewAspectRatio()
{
    double width = ofGetWidth();
    double height = ofGetHeight();

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
}
void DrawingLifeApp::fillViewAreaUTM( int backgroundColor)
{
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
    switch (key)
    {
    case 'a':
        m_isAnimation = !m_isAnimation;
        break;
    case 'f':
        m_isFullscreen = !m_isFullscreen;
        ofSetFullscreen(m_isFullscreen);
        break;
    case 49:
        loadGpsDataCity(m_names, "Berlin");
        break;
    case 50:
        loadGpsDataCity(m_names, "London");
        break;
    case 51:
        loadGpsDataCity(m_names, "Barcelona");
        break;
    case 52:
        loadGpsDataCity(m_names, "Hamburg");
        break;
    case 53:
        loadGpsDataCity(m_names, "Vienna");
        break;
    case 54:
        loadGpsDataCity(m_names,"New York");
        break;
    case 55:
        loadGpsDataCity(m_names, "Tokyo");
        break;
    case 56:
        loadGpsDataCity(m_names, "San Francisco");
        break;
    case 57:
        loadGpsDataCity(m_names, "Bristol");
        break;
    case 48:
        loadGpsDataCity(m_names, "Banff");
        break;
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

