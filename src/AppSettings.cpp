#include "AppSettings.h"
#include "ofxXmlSettings.h"

static const char* settingsPath = "AppSettings.xml";

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
m_colorForegroundR(255),
m_colorForegroundG(255),
m_colorForegroundB(255),
m_colorBackgroundR(0),
m_colorBackgroundG(0),
m_colorBackgroundB(0),
m_colorViewboxR(30),
m_colorViewboxG(30),
m_colorViewboxB(30),
m_colorInteractiveSegR(255),
m_colorInteractiveSegG(0),
m_colorInteractiveSegB(0),
m_colorInteractiveSegA(255),
m_alphaTracks(64),
m_alphaDots(127),
m_alphaLegend(255),
m_dotSize(5),
m_printSettings(false),
m_logLevel(0),
m_debugMode(false),
m_walkLength(0),
m_drawSpeed(1),
m_loadOnStart(false),
m_frameRate(30),
m_fullscreen(false),
m_imageAsCurrentPoint(false),
m_hideCursor(false),
m_boundingBoxEnabled(true),
m_boundingBoxSize(3000.0),
m_boundingBoxPadding(500.0),
m_databasePath(""),
m_queryType(0),
m_queryYearStart(0),
m_queryYearEnd(0),
m_queryCity(""),
m_numPerson(0),
m_interactiveMode(false),
m_interactiveTraced(true),
m_meridianAuto(true),
m_meridianVal(0.0)
{
    ofxXmlSettings m_xml;

    if(m_xml.loadFile(settingsPath))
    {
        ofLog(OF_LOG_SILENT, "Settings file: %s loaded!\n", settingsPath);
    }
    else
    {
        ofLog(OF_LOG_SILENT, "Loading %s failed!\n", settingsPath);
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

    m_colorInteractiveSegR = m_xml.getAttribute("ui:color:interactiveseg", "r", 255);
    m_colorInteractiveSegG = m_xml.getAttribute("ui:color:interactiveseg", "g", 255);
    m_colorInteractiveSegB = m_xml.getAttribute("ui:color:interactiveseg", "b", 0);
    m_colorInteractiveSegA = m_xml.getAttribute("ui:color:interactiveseg", "a", 255);

    m_alphaTracks = m_xml.getValue("ui:alpha:tracks", 64);
    m_alphaDots = m_xml.getValue("ui:alpha:dots", 127);
    m_alphaLegend = m_xml.getValue("ui:alpha:legend", 255);

    m_dotSize = m_xml.getValue("ui:dotsize", 5);

    m_logLevel = m_xml.getAttribute("settings:log", "level", 0);
    m_debugMode = m_xml.getValue("settings:debugmode", 0) == 1 ? true : false;

    m_walkLength = m_xml.getValue("settings:walklength", 10000);

    m_boundingBoxEnabled = m_xml.getValue("settings:boundingbox:enabled", 1) == 1 ? true : false;
    m_boundingBoxSize = m_xml.getValue("settings:boundingbox:size", 3000.0);
    m_boundingBoxPadding = m_xml.getValue("settings:boundingbox:padding", 500.0);

    m_databasePath = ofToDataPath(m_xml.getValue("data:database", "test.sqlite"), true);

    m_queryType = m_xml.getValue("dbquery:type", 4);
    m_queryYearStart = m_xml.getValue("dbquery:time:yearstart", 2009);
    m_queryYearEnd = m_xml.getValue("dbquery:time:yearend", 2010);
    m_queryCity = m_xml.getValue("dbquery:city", "Berlin");

    m_meridianAuto = m_xml.getValue("settings:meridian:auto", 1) == 1 ? true : false;
    m_meridianVal = m_xml.getValue("settings:meridian:lon0", 0.0);

    m_xml.pushTag("data");
    m_xml.pushTag("person");

    m_numPerson = m_xml.getNumTags("name");
    for(int i = 0; i < m_numPerson; ++i)
    {
        m_names.push_back(m_xml.getValue("name", "", i));
        ofColor tmpColor;
//        tmpColor.r = m_xml.getAttribute("name", "r", ofRandom(0, 255), i);
//        tmpColor.g = m_xml.getAttribute("name", "g", ofRandom(0, 255), i);
//        tmpColor.b = m_xml.getAttribute("name", "b", ofRandom(0, 255), i);
        tmpColor.r = (float)m_xml.getAttribute("name", "r", 255, i);
        tmpColor.g = (float)m_xml.getAttribute("name", "g", 255, i);
        tmpColor.b = (float)m_xml.getAttribute("name", "b", 255, i);
        m_nameColors.push_back(tmpColor);

        std::string sqlFilePath = m_xml.getAttribute("name", "sql", "", i);
        m_sqlFilePaths.push_back(sqlFilePath);
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

    m_interactiveMode = m_xml.getValue("settings:interactivemode:enabled", 0) == 1 ? true : false;
    m_interactiveTraced = m_xml.getValue("settings:interactivemode:traced", 1) == 1 ? true : false;

    m_printSettings = m_xml.getValue("settings:printvalues", 0) == 1 ? true : false;

	if(m_printSettings)
	{
		this->print();
	}
}

void AppSettings::print()
{
    ofLog(OF_LOG_SILENT, "------------------------------");
    ofLog(OF_LOG_SILENT, "All loaded app setting values:");
    ofLog(OF_LOG_SILENT, "------------------------------");
    ofLog(OF_LOG_SILENT, "Font: title | name = %s, size = %d", m_fontTitleName.c_str(), m_fontTitleSize);
    ofLog(OF_LOG_SILENT, "Font: author | name = %s, size = %d", m_fontAuthorName.c_str(), m_fontAuthorSize);
    ofLog(OF_LOG_SILENT, "Font: text | name = %s, size = %d", m_fontTextName.c_str(), m_fontTextSize);
    ofLog(OF_LOG_SILENT, "Font: info | name = %s, size = %d", m_fontInfoName.c_str(), m_fontInfoSize);

    ofLog(OF_LOG_SILENT, "Foreground color: r = %d, g = %d, b = %d", m_colorForegroundR, m_colorForegroundG, m_colorForegroundB);
    ofLog(OF_LOG_SILENT, "Background color: r = %d, g = %d, b = %d", m_colorBackgroundR, m_colorBackgroundG, m_colorBackgroundB);
    ofLog(OF_LOG_SILENT, "Viewbox color: r = %d, g = %d, b = %d", m_colorViewboxR, m_colorViewboxG, m_colorViewboxB);

    ofLog(OF_LOG_SILENT, "Alphas: tracks = %d, dots = %d, legend = %d", m_alphaTracks, m_alphaDots, m_alphaLegend);

    ofLog(OF_LOG_SILENT, "Debug mode: %d", m_debugMode);
    ofLog(OF_LOG_SILENT, "Interactive mode: %d, show only one segment: %d, seg color: r=%d, g=%d, b=%d, a=%d",
          m_interactiveMode,
          m_interactiveTraced,
          m_colorInteractiveSegR,
          m_colorInteractiveSegG,
          m_colorInteractiveSegB,
          m_colorInteractiveSegA);

    ofLog(OF_LOG_SILENT, "Start fullscreen: %d", m_fullscreen);
    ofLog(OF_LOG_SILENT, "Load Gps on start: %d", m_loadOnStart);
    ofLog(OF_LOG_SILENT, "Show image as current point: %d", m_imageAsCurrentPoint);
    ofLog(OF_LOG_SILENT, "Hide cursor: %d", m_hideCursor);
    ofLog(OF_LOG_SILENT, "Log level: %d", m_logLevel);
    ofLog(OF_LOG_SILENT, "Walk length: %d", m_walkLength);
    ofLog(OF_LOG_SILENT, "Draw speed: %d", m_drawSpeed);
    ofLog(OF_LOG_SILENT, "Frame rate: %d", m_frameRate);

    ofLog(OF_LOG_SILENT, "Bounding box: size = %lf, padding = %lf", m_boundingBoxSize, m_boundingBoxPadding);

    ofLog(OF_LOG_SILENT, "Meridian value: %lf, auto = %d", m_meridianVal, m_meridianAuto);

    ofLog(OF_LOG_SILENT, "Database path: %s", m_databasePath.c_str());

    ofLog(OF_LOG_SILENT, "Number of person: %d", m_numPerson);

    ofLog(OF_LOG_SILENT, "Query: type = %d, start year = %d, end year = %d, city = %s", m_queryType, m_queryYearStart, m_queryYearEnd, m_queryCity.c_str());
    for (unsigned int i=0; i < m_names.size();++i)
    {
    	ofLog(OF_LOG_SILENT, "Name %d: %s", i, m_names[i].c_str());
    	ofLog(OF_LOG_SILENT, "Name color %d: r=%d, g=%d, b=%d", i, (int)m_nameColors[i].r, (int)m_nameColors[i].g, (int)m_nameColors[i].b);
    }
    for (unsigned int i=0; i < m_imagePaths.size();++i)
    {
    	ofLog(OF_LOG_SILENT, "Current point image %d: %s", i, m_imagePaths[i].c_str());
    }
    ofLog(OF_LOG_SILENT, "------------------------------\n");

}
