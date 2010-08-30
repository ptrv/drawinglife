#include "AppSettings.h"
#include "ofxXmlSettings.h"

AppSettings::AppSettings()
:
m_fontTitleName(""),
m_fontTitleSize(0),
m_fontAuthorName(""),
m_fontAuthorSize(0),
m_fontTextName(""),
m_fontTextSize(0),
m_fontInfoName(""),
m_fontInfoSize(0),
m_colorForegroundR(0),
m_colorForegroundG(0),
m_colorForegroundB(0),
m_colorBackgroundR(0),
m_colorBackgroundG(0),
m_colorBackgroundB(0),
m_colorViewboxR(0),
m_colorViewboxG(0),
m_colorViewboxB(0),
m_alphaTracks(0),
m_alphaDots(0),
m_alphaLegend(0),
m_logLevel(0),
m_debugMode(false),
m_walkLength(0),
m_drawSpeed(0),
m_loadOnStart(false),
m_frameRate(0),
m_fullscreen(false),
m_imageAsCurrentPoint(false),
m_hideCursor(false),
m_boundingBoxSize(0.0),
m_boundingBoxPadding(0.0),
m_databasePath(""),
m_queryType(0),
m_queryYearStart(0),
m_queryYearEnd(0),
m_queryCity(""),
m_numPerson(0)
{
    ofxXmlSettings m_xml;

	// reading settings from xml file
	std::string settingsFile = "AppSettings.xml";
    if(m_xml.loadFile(settingsFile))
    {
//        std::cout << settingsFile << " loaded!" << std::endl;
        ofLog(OF_LOG_SILENT, "%s loaded!", settingsFile.c_str());
    }
    else
    {
        ofLog(OF_LOG_WARNING, "Loading %s failed!", settingsFile.c_str());
    }

    m_xml.pushTag("ui");
    m_xml.pushTag("fonts");
    m_fontTitleName = m_xml.getAttribute("font", "name","mono.ttf", 0);
    m_fontTitleSize = m_xml.getAttribute("font", "size", 50, 0);
    m_fontAuthorName = m_xml.getAttribute("font", "name", "mono.ttf", 1);
    m_fontAuthorSize = m_xml.getAttribute("font", "size", 24, 1);
    m_fontTextName = m_xml.getAttribute("font", "name", "mono.ttf", 2);
    m_fontTextSize = m_xml.getAttribute("font", "size", 16, 2);
    m_fontInfoName = m_xml.getAttribute("font", "name", "consola.ttf", 3);
    m_fontInfoSize = m_xml.getAttribute("font", "size", 11, 3);
    m_xml.popTag();
    m_xml.popTag();

    m_colorForegroundR = m_xml.getAttribute("ui:color:foreround", "r", 255);
    m_colorForegroundG = m_xml.getAttribute("ui:color:foreround", "g", 255);
    m_colorForegroundB = m_xml.getAttribute("ui:color:foreround", "b", 255);

    m_colorBackgroundR = m_xml.getAttribute("ui:color:background", "r", 0);
    m_colorBackgroundG = m_xml.getAttribute("ui:color:background", "g", 0);
    m_colorBackgroundB = m_xml.getAttribute("ui:color:background", "b", 0);

    m_colorViewboxR = m_xml.getAttribute("ui:color:viewbox", "r", 50);
    m_colorViewboxG = m_xml.getAttribute("ui:color:viewbox", "g", 50);
    m_colorViewboxB = m_xml.getAttribute("ui:color:viewbox", "b", 50);


    m_alphaTracks = m_xml.getValue("ui:alpha:tracks", 64);
    m_alphaDots = m_xml.getValue("ui:alpha:dots", 127);
    m_alphaLegend = m_xml.getValue("ui:alpha:legend", 255);

    m_logLevel = m_xml.getAttribute("settings:log", "level", 0);
    m_debugMode = m_xml.getValue("settings:debugmode", 0) == 1 ? true : false;

    m_walkLength = m_xml.getValue("settings:walklength", 10000);

    m_boundingBoxSize = m_xml.getValue("settings:boundingbox:size", 3000.0);
    m_boundingBoxPadding = m_xml.getValue("settings:boundingbox:padding", 500.0);

    m_databasePath = ofToDataPath(m_xml.getValue("data:database", "test.sqlite"), true);

    m_queryType = m_xml.getValue("dbquery:type", 4);
    m_queryYearStart = m_xml.getValue("dbquery:time:yearstart", 2009);
    m_queryYearEnd = m_xml.getValue("dbquery:time:yearend", 2010);
    m_queryCity = m_xml.getValue("dbquery:city", "Berlin");

    m_xml.pushTag("data");
    m_xml.pushTag("person");

    m_numPerson = m_xml.getNumTags("name");
    for(int i = 0; i < m_numPerson; ++i)
    {
        m_names.push_back(m_xml.getValue("name", "", i));
    }
    m_xml.popTag();
    m_xml.popTag();

    m_drawSpeed = m_xml.getValue("settings:drawspeed", 1)*m_numPerson;
    m_loadOnStart = m_xml.getValue("settings:loadgpsonstart",1);
    m_frameRate = m_xml.getValue("settings:framerate", 30);
    m_fullscreen = m_xml.getValue("settings:fullscreen", 0) == 1 ? true : false;
    m_imageAsCurrentPoint = m_xml.getValue("ui:imageascurrent", 0) == 1 ? true : false;
    m_hideCursor = m_xml.getValue("settings:hidecursor", 0) == 1 ? true : false;

    if(m_imageAsCurrentPoint)
    {
        m_xml.pushTag("ui");
        m_xml.pushTag("images");

        int numImgTags = m_xml.getNumTags("image");
        for(int i = 0; i < numImgTags; ++i)
        {
            m_imagePaths.push_back(m_xml.getValue("image", "", i));
        }

        m_xml.popTag();
        m_xml.popTag();
    }
}

