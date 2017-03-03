#include "DrawingLifeIncludes.h"
#include "AppSettings.h"
#include "ofxXmlSettings.h"

//------------------------------------------------------------------------------

static const int NUM_FONTS = 4;
static const std::string defaultFontIds[NUM_FONTS] = {"title", "author", "text",
                                                      "info"};
static const std::string defaultFontNames[NUM_FONTS] = {"mono.ttf", "mono.ttf",
                                                        "mono.ttf", "consola.ttf"};
static const int defaultFontSizes[NUM_FONTS] = {50, 24, 16, 50};

//static const char* settingsPath = "AppSettings.xml";

//------------------------------------------------------------------------------

AppSettings::AppSettings(const std::string& path)
:
m_initialized(false),
m_settingsFilePath(path),
m_colorForegroundR(255),
m_colorForegroundG(255),
m_colorForegroundB(255),
m_colorBackgroundR(0),
m_colorBackgroundG(0),
m_colorBackgroundB(0),
m_colorViewboxR(0),
m_colorViewboxG(0),
m_colorViewboxB(0),
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
m_loadOnStart(true),
m_frameRate(30),
m_fullscreen(false),
m_imageAsCurrentPoint(false),
m_hideCursor(false),
m_boundingBoxCropMode(true),
m_boundingBoxSize(3000.0),
m_boundingBoxPadding(500.0),
m_boundingBoxShow(false),
m_databasePath(""),
m_queryType(0),
m_queryYearStart(0),
m_queryYearEnd(0),
m_queryCity(""),
m_numPersons(0),
m_interactiveMode(false),
m_interactiveTraced(true),
m_showInfo(true),
m_loop(true),
m_multiMode(false),
m_multiModeInfo(false),
m_sleepTime(0),
m_useSpeed(false),
m_speedThreshold(0.0),
m_grabScreen(false)
{
    ofFile settingsFile = ofFile(ofToDataPath(m_settingsFilePath));

    if (settingsFile.getExtension().compare("xml") == 0)
    {
        m_initialized = loadXML();
    }

    if (m_initialized && m_printSettings)
    {
        print();
    }
}

//------------------------------------------------------------------------------

AppSettings::~AppSettings()
{
}

//------------------------------------------------------------------------------
// Load XML
//------------------------------------------------------------------------------
bool AppSettings::loadXML()
{
    ofxXmlSettings m_xml;

    if (m_xml.loadFile(m_settingsFilePath))
    {
        ofLog(OF_LOG_SILENT, "Settings file: "+m_settingsFilePath+" loaded!\n");
    }
    else
    {
        ofLog(OF_LOG_SILENT, "Loading "+m_settingsFilePath+" failed!\n");
        return false;
    }

    m_xml.pushTag("drawinglife");
    m_xml.pushTag("ui");
    m_xml.pushTag("fonts");
    for (size_t i = 0; i < NUM_FONTS; ++i)
    {
        std::string id = defaultFontIds[i];
        std::string fontName = m_xml.getAttribute("font", "name",
                                                  defaultFontNames[i], i);
        int fontSize = m_xml.getAttribute("font", "size",
                                          defaultFontSizes[i], i);

        m_fonts[id] = std::make_pair(fontName, fontSize);
    }
    m_xml.popTag();

    m_xml.pushTag("color");

    m_colorForegroundR = m_xml.getAttribute("foreground", "r", 255);
    m_colorForegroundG = m_xml.getAttribute("foreground", "g", 255);
    m_colorForegroundB = m_xml.getAttribute("foreground", "b", 255);

    m_colorBackgroundR = m_xml.getAttribute("background", "r", 0);
    m_colorBackgroundG = m_xml.getAttribute("background", "g", 0);
    m_colorBackgroundB = m_xml.getAttribute("background", "b", 0);

    m_colorViewboxR = m_xml.getAttribute("viewbox", "r", 0);
    m_colorViewboxG = m_xml.getAttribute("viewbox", "g", 0);
    m_colorViewboxB = m_xml.getAttribute("viewbox", "b", 0);

    m_colorInteractiveSegR = m_xml.getAttribute("interactiveseg", "r", 255);
    m_colorInteractiveSegG = m_xml.getAttribute("interactiveseg", "g", 255);
    m_colorInteractiveSegB = m_xml.getAttribute("interactiveseg", "b", 0);
    m_colorInteractiveSegA = m_xml.getAttribute("interactiveseg", "a", 255);

    m_xml.popTag();

    m_xml.pushTag("alpha");

    m_alphaTracks = m_xml.getValue("tracks", 64);
    m_alphaDots = m_xml.getValue("dots", 127);
    m_alphaLegend = m_xml.getValue("legend", 255);

    m_xml.popTag();

    m_dotSize = m_xml.getValue("dotsize", 5);

    m_xml.popTag();

    m_xml.pushTag("settings");

    m_logLevel = m_xml.getAttribute("log", "level", 0);
    if (m_logLevel < 0 || m_logLevel > 5)
    {
        m_logLevel = 0;
    }

    m_debugMode = m_xml.getValue("debugmode", 0) == 1;

    m_walkLength = m_xml.getValue("walklength", 10000);

    m_xml.pushTag("boundingbox");

    std::string cropModeTag = "cropmode";
    if (m_xml.getNumTags(cropModeTag) == 0)
    {
        cropModeTag = "auto";
    }
    m_boundingBoxCropMode = m_xml.getValue(cropModeTag, 1) == 1;

    m_boundingBoxSize = m_xml.getValue("size", 3000.0);
    m_boundingBoxPadding = m_xml.getValue("padding", 500.0);
    m_boundingBoxStatic = m_xml.getValue("static", 0) == 1;
    m_boundingBoxLat = m_xml.getAttribute("position", "lat", 52.542);
    m_boundingBoxLon = m_xml.getAttribute("position", "lon", 13.413);
    m_boundingBoxShow = m_xml.getValue("show", 0) == 1;

    m_xml.popTag();
    m_xml.popTag();

    m_databasePath = ofToDataPath(m_xml.getValue("data:database", "test.sqlite"), true);

    m_queryType = m_xml.getValue("dbquery:type", 4);
    m_queryYearStart = m_xml.getValue("dbquery:time:yearstart", 2009);
    m_queryYearEnd = m_xml.getValue("dbquery:time:yearend", 2010);
    m_queryCity = m_xml.getValue("dbquery:city", "Berlin");

    m_xml.pushTag("data");
    m_xml.pushTag("person");

    m_numPersons = m_xml.getNumTags("name");
    for (unsigned int i = 0; i < m_numPersons; ++i)
    {
        m_names.push_back(m_xml.getValue("name", "", i));
        ofColor tmpColor;
        tmpColor.r = (float)m_xml.getAttribute("name", "r", ofRandom(0, 255), i);
        tmpColor.g = (float)m_xml.getAttribute("name", "g", ofRandom(0, 255), i);
        tmpColor.b = (float)m_xml.getAttribute("name", "b", ofRandom(0, 255), i);
        tmpColor.a = (float)m_xml.getAttribute("name", "a", ofRandom(0, 255), i);
        m_nameColors.push_back(tmpColor);

        std::string sqlFilePath = m_xml.getAttribute("name", "sql", "", i);
        m_sqlFilePaths.push_back(sqlFilePath);
    }
    m_xml.popTag();
    m_xml.popTag();

    m_drawSpeed = m_xml.getValue("settings:drawspeed", 1.0f);
    m_loadOnStart = m_xml.getValue("settings:loadgpsonstart",1);
    m_frameRate = m_xml.getValue("settings:framerate", 30);
    m_fullscreen = m_xml.getValue("settings:fullscreen", 0) == 1;
    m_imageAsCurrentPoint = m_xml.getValue("ui:imageascurrent", 0) == 1;
    m_hideCursor = m_xml.getValue("settings:hidecursor", 0) == 1;

    if (m_imageAsCurrentPoint)
    {
        m_xml.pushTag("ui");
        m_xml.pushTag("currentpointimages");

        int numImgTags = m_xml.getNumTags("image");
        for (int i = 0; i < numImgTags; ++i)
        {
            m_xml.pushTag("image", i);
            CurrentPointImageData cid;
            cid.path = m_xml.getValue("path", "");
            cid.width = m_xml.getValue("width", 0.0);
            cid.height = m_xml.getValue("height", 0.0);
            cid.alpha = m_xml.getValue("alpha", 255);
            m_currImageData.push_back(cid);
            m_xml.popTag();
        }

        m_xml.popTag();
        m_xml.popTag();
    }

    m_interactiveMode = m_xml.getValue("settings:interactivemode:enabled", 0) == 1;
    m_interactiveTraced = m_xml.getValue("settings:interactivemode:traced", 1) == 1;

    m_showInfo = m_xml.getValue("settings:showinfo", 1) == 1;

    m_loop = m_xml.getValue("settings:loop", 1) == 1;

    m_multiMode = m_xml.getValue("settings:multimode", 0) == 1;
    m_multiModeInfo = m_xml.getValue("settings:multimodeinfo", 0) == 1;

    m_sleepTime = m_xml.getValue("settings:sleeptime", 0);

    m_useSpeed = m_xml.getValue("settings:usespeed", 0) == 1;
    m_speedThreshold = m_xml.getAttribute("settings:usespeed", "threshold", 0.0);

    m_speedColorUnder.r = m_xml.getAttribute("ui:speedcolors:underthreshold", "r", 255);
    m_speedColorUnder.g = m_xml.getAttribute("ui:speedcolors:underthreshold", "g", 255);
    m_speedColorUnder.b = m_xml.getAttribute("ui:speedcolors:underthreshold", "b", 255);
    m_speedColorUnder.a = m_xml.getAttribute("ui:speedcolors:underthreshold", "a", 255);

    m_speedColorAbove.r = m_xml.getAttribute("ui:speedcolors:abovethreshold", "r", 255);
    m_speedColorAbove.g = m_xml.getAttribute("ui:speedcolors:abovethreshold", "g", 255);
    m_speedColorAbove.b = m_xml.getAttribute("ui:speedcolors:abovethreshold", "b", 255);
    m_speedColorAbove.a = m_xml.getAttribute("ui:speedcolors:abovethreshold", "a", 255);

    m_xml.pushTag("ui");
    m_xml.pushTag("locationimages");
    int numlocImgTags = m_xml.getNumTags("image");
    m_locationImgData.clear();
    for (int i = 0; i < numlocImgTags; ++i)
    {
        m_xml.pushTag("image",i);
        LocationImageData lid;
        lid.path = m_xml.getValue("path", "");
        lid.name = m_xml.getValue("name", "");
        lid.gps.lat = m_xml.getValue("lat", 0.0);
        lid.gps.lon = m_xml.getValue("lon", 0.0);
        lid.width = m_xml.getValue("width", 0.0);
        lid.height = m_xml.getValue("height", 0.0);
        lid.alpha = m_xml.getValue("alpha", 255);
        lid.anchorType = m_xml.getValue("anchor:type", 1);
        lid.anchorX = m_xml.getValue("anchor:posx", 0.5);
        lid.anchorY = m_xml.getValue("anchor:posy", 0.5);
        lid.anchorShow = m_xml.getValue("anchor:show", 0) == 1;

        m_locationImgData.push_back(lid);
        m_xml.popTag();
    }

    m_xml.popTag();
    m_xml.popTag();

    m_xml.pushTag("zoomanimation");
    m_isZoomAnimation = m_xml.getValue("active", 0) == 1;
    m_zoomAnimationDamp = m_xml.getValue("dampzoom", 0.2);
    m_zoomAnimationAttraction = m_xml.getValue("attractionzoom", 0.2);
    m_zoomAnimationDampCenter = m_xml.getValue("dampcenter", 0.2);
    m_zoomAnimationAttractionCenter = m_xml.getValue("attractioncenter", 0.2);
    m_zoomAnimationAttractionCriteria = m_xml.getValue("type", 1);
    m_zoomanimationUseOnlyZ = m_xml.getValue("onlyz", 0) == 1;
    m_xml.pushTag("frames");
    int numZoomFrames = m_xml.getNumTags("frame");
    for (int i = 0; i < numZoomFrames; ++i)
    {
    	ZoomAnimFrame zaf;
    	zaf.frameTime = m_xml.getAttribute("frame", "time", 0.0, i);
    	zaf.frameZoom = m_xml.getAttribute("frame", "zoom", 0, i);
    	zaf.frameCenterX = m_xml.getAttribute("frame", "lon", 0.0, i);
    	zaf.frameCenterY = m_xml.getAttribute("frame", "lat", 0.0, i);
    	zaf.gpsId = m_xml.getAttribute("frame", "gpsid", -1, i);
    	zaf.timestamp = m_xml.getAttribute("frame", "timestamp", "", i);
    	m_zoomAnimationFrames.push_back(zaf);
    }

    m_xml.popTag();
    m_xml.popTag();

    m_xml.pushTag("sound");
    m_isSoundActive = m_xml.getValue("active", 0) == 1;

    m_xml.pushTag("soundfiles");
    int numSoundFiles = m_xml.getNumTags("soundfile");
    for (int i = 0; i < numSoundFiles; ++i) {
		m_soundFiles.push_back(m_xml.getAttribute("soundfile", "src", "", i));
	}
    m_xml.popTag();
	m_xml.popTag();

    m_vertexShaderSource = m_xml.getValue("shader:vertex", "");
    m_fragmentShaderSource = m_xml.getValue("shader:fragment", "");
    m_useShader = m_xml.getValue("shader:enabled", 0) == 1;

    m_printSettings = m_xml.getValue("settings:printvalues", 0) == 1;

    m_grabScreen = m_xml.getValue("settings:grabscreen", 0) == 1;

    m_timelineRt = m_xml.getValue("settings:timeline:realtime", 0) == 1;
    m_timelineSpeed = m_xml.getValue("settings:timeline:speed", 100);
    m_timelineSkipTime = m_xml.getValue("settings:timeline:skiptime", 60);

    m_xml.popTag();

    return true;
}

//------------------------------------------------------------------------------
// Print
//------------------------------------------------------------------------------
void AppSettings::print()
{
    ofLog(OF_LOG_SILENT, "------------------------------");
    ofLog(OF_LOG_SILENT, "All loaded app setting values:");
    ofLog(OF_LOG_SILENT, "------------------------------");
    ofLog(OF_LOG_SILENT, "Font: title | name = %s, size = %d",
          m_fonts["title"].first.c_str(), m_fonts["title"].second);
    ofLog(OF_LOG_SILENT, "Font: author | name = %s, size = %d",
          m_fonts["author"].first.c_str(), m_fonts["author"].second);
    ofLog(OF_LOG_SILENT, "Font: text | name = %s, size = %d",
          m_fonts["text"].first.c_str(), m_fonts["text"].second);
    ofLog(OF_LOG_SILENT, "Font: info | name = %s, size = %d",
          m_fonts["info"].first.c_str(), m_fonts["info"].second);

    ofLog(OF_LOG_SILENT, "Foreground color: r = %d, g = %d, b = %d",
          m_colorForegroundR, m_colorForegroundG, m_colorForegroundB);
    ofLog(OF_LOG_SILENT, "Background color: r = %d, g = %d, b = %d",
          m_colorBackgroundR, m_colorBackgroundG, m_colorBackgroundB);
    ofLog(OF_LOG_SILENT, "Viewbox color: r = %d, g = %d, b = %d",
          m_colorViewboxR, m_colorViewboxG, m_colorViewboxB);

    ofLog(OF_LOG_SILENT, "Alphas: tracks = %d, dots = %d, legend = %d",
          m_alphaTracks, m_alphaDots, m_alphaLegend);

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
    ofLog(OF_LOG_SILENT, "Show image as current point: %d",
          m_imageAsCurrentPoint);
    ofLog(OF_LOG_SILENT, "Hide cursor: %d", m_hideCursor);
    ofLog(OF_LOG_SILENT, "Show info: %d", m_showInfo);
    ofLog(OF_LOG_SILENT, "Log level: %d", m_logLevel);
    ofLog(OF_LOG_SILENT, "Walk length: %d", m_walkLength);
    ofLog(OF_LOG_SILENT, "Draw speed: %d", m_drawSpeed);
    ofLog(OF_LOG_SILENT, "Frame rate: %d", m_frameRate);

    ofLog(OF_LOG_SILENT, "Bounding box: size = %lf, padding = %lf",
          m_boundingBoxSize, m_boundingBoxPadding);

    ofLog(OF_LOG_SILENT, "Database path: %s", m_databasePath.c_str());

    ofLog(OF_LOG_SILENT, "Number of person: %d", m_numPersons);

    ofLog(OF_LOG_SILENT,
          "Query: type = %d, start year = %d, end year = %d, city = %s",
          m_queryType, m_queryYearStart, m_queryYearEnd, m_queryCity.c_str());
    for (unsigned int i=0; i < m_names.size();++i)
    {
    	ofLog(OF_LOG_SILENT, "Name %d: %s", i, m_names[i].c_str());
        ofLog(OF_LOG_SILENT, "Name color %d: r=%d, g=%d, b=%d", i,
              (int)m_nameColors[i].r,
              (int)m_nameColors[i].g,
              (int)m_nameColors[i].b);
    }
    for (unsigned int i=0; i < m_currImageData.size();++i)
    {
        ofLog(OF_LOG_SILENT, "Current point image %d: %s", i,
              m_currImageData[i].path.c_str());
    }

    ofLog(OF_LOG_SILENT, "Timeline realtime: %d", m_timelineRt);
    ofLog(OF_LOG_SILENT, "Timeline speed: %d", m_timelineSpeed);
    ofLog(OF_LOG_SILENT, "Timeline skip time withaout data: %d", m_timelineSkipTime);

    ofLog(OF_LOG_SILENT, "------------------------------\n");

}

//------------------------------------------------------------------------------
