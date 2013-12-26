#include "AppSettings.h"
#include "ofxXmlSettings.h"
#include "ofxJSONElement.h"
#include <jsoncpp/json/json.h>

namespace JsonHelper
{
// -----------------------------------------------------------------------------
#define CATCH_JSON_PARSE_ERROR                                                  \
catch (std::exception& ex)                                                      \
{                                                                               \
    ofLogError() << "Json parse error with key: <" << key << ">"                \
                 << ", msg: " << ex.what();                                     \
    return defaultValue;                                                        \
}                                                                               \
// -----------------------------------------------------------------------------

std::string getString(const Json::Value& value, const std::string& key,
                      const std::string& defaultValue)
{
    if (!value.isObject())
    {
        return defaultValue;
    }
    try
    {
        return value.get(key, defaultValue).asString();
    }
    CATCH_JSON_PARSE_ERROR
}

std::string getString(const Json::Value& value, const std::string& defaultValue)
{
    if (!value.isString())
    {
        return defaultValue;
    }
    try
    {
        return value.asString();
    }
    catch (const std::exception& ex)
    {
        ofLogError() << "Json parse error: " << ex.what();
        return defaultValue;
    }
}

bool getBool(const Json::Value& value, const std::string& key,
             const bool defaultValue)
{
    if (!value.isObject())
    {
        return defaultValue;
    }
    try
    {
        return value.get(key, defaultValue).asBool();
    }
    CATCH_JSON_PARSE_ERROR
}

int getInt(const Json::Value& value, const std::string& key,
           const int defaultValue)
{
    if (!value.isObject())
    {
        return defaultValue;
    }
    try
    {
        return value.get(key, defaultValue).asInt();
    }
    CATCH_JSON_PARSE_ERROR
}

double getDouble(const Json::Value& value, const std::string& key,
                 const double defaultValue)
{
    if (!value.isObject())
    {
        return defaultValue;
    }
    try
    {
        return value.get(key, defaultValue).asDouble();
    }
    CATCH_JSON_PARSE_ERROR
}

}
// -----------------------------------------------------------------------------

static const int NUM_FONTS = 4;
static const std::string defaultFontIds[NUM_FONTS] = {"title", "author", "text",
                                                      "info"};
static const std::string defaultFontNames[NUM_FONTS] = {"mono.ttf", "mono.ttf",
                                                        "mono.ttf", "consola.ttf"};
static const int defaultFontSizes[NUM_FONTS] = {50, 24, 16, 50};

//static const char* settingsPath = "AppSettings.xml";

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
m_loadOnStart(false),
m_frameRate(30),
m_fullscreen(false),
m_imageAsCurrentPoint(false),
m_hideCursor(false),
m_boundingBoxAuto(true),
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
m_meridianVal(0.0),
m_showInfo(true),
m_regionsOn(true),
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
    else if (settingsFile.getExtension().compare("json") == 0)
    {
        m_initialized = loadJSON();
    }

    if(m_initialized && m_printSettings)
    {
        print();
    }


}

AppSettings::~AppSettings()
{
}

bool AppSettings::loadXML()
{
    ofxXmlSettings m_xml;

    if(m_xml.loadFile(m_settingsFilePath))
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
    m_xml.popTag();

    m_colorForegroundR = m_xml.getAttribute("ui:color:foreground", "r", 255);
    m_colorForegroundG = m_xml.getAttribute("ui:color:foreground", "g", 255);
    m_colorForegroundB = m_xml.getAttribute("ui:color:foreground", "b", 255);

    m_colorBackgroundR = m_xml.getAttribute("ui:color:background", "r", 0);
    m_colorBackgroundG = m_xml.getAttribute("ui:color:background", "g", 0);
    m_colorBackgroundB = m_xml.getAttribute("ui:color:background", "b", 0);

    m_colorViewboxR = m_xml.getAttribute("ui:color:viewbox", "r", 0);
    m_colorViewboxG = m_xml.getAttribute("ui:color:viewbox", "g", 0);
    m_colorViewboxB = m_xml.getAttribute("ui:color:viewbox", "b", 0);

    m_colorInteractiveSegR = m_xml.getAttribute("ui:color:interactiveseg", "r", 255);
    m_colorInteractiveSegG = m_xml.getAttribute("ui:color:interactiveseg", "g", 255);
    m_colorInteractiveSegB = m_xml.getAttribute("ui:color:interactiveseg", "b", 0);
    m_colorInteractiveSegA = m_xml.getAttribute("ui:color:interactiveseg", "a", 255);

    m_alphaTracks = m_xml.getValue("ui:alpha:tracks", 64);
    m_alphaDots = m_xml.getValue("ui:alpha:dots", 127);
    m_alphaLegend = m_xml.getValue("ui:alpha:legend", 255);

    m_dotSize = m_xml.getValue("ui:dotsize", 5);

    m_logLevel = m_xml.getAttribute("settings:log", "level", 0);
    m_debugMode = m_xml.getValue("settings:debugmode", 0) == 1;

    m_walkLength = m_xml.getValue("settings:walklength", 10000);

    m_boundingBoxAuto = m_xml.getValue("settings:boundingbox:auto", 1) == 1;
    m_boundingBoxSize = m_xml.getValue("settings:boundingbox:size", 3000.0);
    m_boundingBoxPadding = m_xml.getValue("settings:boundingbox:padding", 500.0);
    m_boundingBoxFixed = m_xml.getValue("settings:boundingbox:static", 0) == 1;
    m_boundingBoxLat = m_xml.getAttribute("settings:boundingbox:position", "lat", 52.542);
    m_boundingBoxLon = m_xml.getAttribute("settings:boundingbox:position", "lon", 13.413);
    
    m_databasePath = ofToDataPath(m_xml.getValue("data:database", "test.sqlite"), true);

    m_queryType = m_xml.getValue("dbquery:type", 4);
    m_queryYearStart = m_xml.getValue("dbquery:time:yearstart", 2009);
    m_queryYearEnd = m_xml.getValue("dbquery:time:yearend", 2010);
    m_queryCity = m_xml.getValue("dbquery:city", "Berlin");

    m_meridianAuto = m_xml.getValue("settings:meridian:auto", 1) == 1;
    m_meridianVal = m_xml.getValue("settings:meridian:lon0", 0.0);

    m_xml.pushTag("data");
    m_xml.pushTag("person");

    m_numPerson = m_xml.getNumTags("name");
    for(unsigned int i = 0; i < m_numPerson; ++i)
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

    if(m_imageAsCurrentPoint)
    {
        m_xml.pushTag("ui");
        m_xml.pushTag("currentpointimages");

        int numImgTags = m_xml.getNumTags("image");
        for(int i = 0; i < numImgTags; ++i)
        {
            m_xml.pushTag("image", i);
            CurrentImageData cid;
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

    m_regionsOn = m_xml.getValue("settings:meridian:regions", 1) == 1;

    m_regions[0].lon0 = m_xml.getAttribute("settings:meridian:region1", "lon0", -119.0);
    m_regions[0].minLon = m_xml.getAttribute("settings:meridian:region1", "minlon", -180.0);
    m_regions[0].maxLon = m_xml.getAttribute("settings:meridian:region1", "maxlon", -100.0);
    m_regions[1].lon0 = m_xml.getAttribute("settings:meridian:region2", "lon0", -74.0);
    m_regions[1].minLon = m_xml.getAttribute("settings:meridian:region2", "minlon", -100.0);
    m_regions[1].maxLon = m_xml.getAttribute("settings:meridian:region2", "maxlon", -35.0);
    m_regions[2].lon0 = m_xml.getAttribute("settings:meridian:region3", "lon0", 12.0);
    m_regions[2].minLon = m_xml.getAttribute("settings:meridian:region3", "minlon", -35.0);
    m_regions[2].maxLon = m_xml.getAttribute("settings:meridian:region3", "maxlon", 65.0);
    m_regions[3].lon0 = m_xml.getAttribute("settings:meridian:region4", "lon0", 116.0);
    m_regions[3].minLon = m_xml.getAttribute("settings:meridian:region4", "minlon", 65.0);
    m_regions[3].maxLon = m_xml.getAttribute("settings:meridian:region4", "maxlon", 130.0);
    m_regions[4].lon0 = m_xml.getAttribute("settings:meridian:region5", "lon0", 146.0);
    m_regions[4].minLon = m_xml.getAttribute("settings:meridian:region5", "minlon", 130.0);
    m_regions[4].maxLon = m_xml.getAttribute("settings:meridian:region5", "maxlon", 180.0);

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
    for(int i = 0; i < numlocImgTags; ++i)
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
    for(int i = 0; i < numZoomFrames; ++i)
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

    m_xml.popTag();

    return true;
}

bool AppSettings::loadJSON()
{
    ofxJSONElement jsonRoot;

    bool parsingSuccessful = jsonRoot.open(ofToDataPath("AppSettings.json"));

    if (parsingSuccessful)
    {
        //----------------------------------------------------------------------
        // ui
        Json::Value& json = jsonRoot["ui"]["fonts"];

        for (size_t i = 0; i < NUM_FONTS; ++i)
        {
            std::string id = JsonHelper::getString(json[i], "id",
                                                   defaultFontIds[i]);
            std::string name = JsonHelper::getString(json[i], "name",
                                                     defaultFontNames[i]);
            int fontSize = JsonHelper::getInt(json[i], "size",
                                              defaultFontSizes[i]);

            m_fonts[id] = std::make_pair(name, fontSize);
        }

        json = jsonRoot["ui"]["colors"]["foreground"];
        m_colorForegroundR = JsonHelper::getInt(json, "r", 255);
        m_colorForegroundG = JsonHelper::getInt(json, "g", 255);
        m_colorForegroundB = JsonHelper::getInt(json, "b", 255);

        json = jsonRoot["ui"]["colors"]["background"];
        m_colorBackgroundR = JsonHelper::getInt(json, "r", 0);
        m_colorBackgroundG = JsonHelper::getInt(json, "g", 0);
        m_colorBackgroundB = JsonHelper::getInt(json, "b", 0);

        json = jsonRoot["ui"]["colors"]["viewbox"];
        m_colorViewboxR = JsonHelper::getInt(json, "r", 0);
        m_colorViewboxG = JsonHelper::getInt(json, "g", 0);
        m_colorViewboxB = JsonHelper::getInt(json, "b", 0);

        json = jsonRoot["ui"]["alpha"];
        m_alphaTracks = JsonHelper::getInt(json, "path", 64);
        m_alphaDots = JsonHelper::getInt(json, "dots", 127);
        m_alphaLegend = JsonHelper::getInt(json, "legend", 255);

        json = jsonRoot["ui"];
        m_dotSize = JsonHelper::getInt(json, "dotsize", 5);

        //----------------------------------------------------------------------
        // settings
        json = jsonRoot["settings"];
        m_logLevel = JsonHelper::getInt(json, "level_level", 0);
        m_debugMode = JsonHelper::getBool(json, "debugmode", false);
        m_loadOnStart = JsonHelper::getBool(json, "load_data_on_start", true);
        m_fullscreen = JsonHelper::getBool(json, "fullscreen", false);
        m_hideCursor = JsonHelper::getBool(json, "hide_cursor", false);
        m_showInfo = JsonHelper::getBool(json, "showinfo", true);
        m_loop = JsonHelper::getBool(json, "loop", true);
        m_sleepTime = JsonHelper::getInt(json, "sleep_time", 0);
        m_printSettings = JsonHelper::getBool(json, "print_values", false);
        m_grabScreen = JsonHelper::getBool(json, "grab_screen", false);

        //----------------------------------------------------------------------
        // animation
        json = jsonRoot["animation"];
        m_walkLength = JsonHelper::getInt(json, "path_length", 10000);
        m_drawSpeed = JsonHelper::getDouble(json, "speed", 1.0);
        m_frameRate = JsonHelper::getInt(json, "framerate", 30);

        //----------------------------------------------------------------------
        // bounding box
        json = jsonRoot["bounding_box"];
        m_boundingBoxAuto = JsonHelper::getBool(json, "auto", true);
        m_boundingBoxSize = JsonHelper::getDouble(json, "size", 3000.0);
        m_boundingBoxPadding = JsonHelper::getDouble(json, "padding", 500.0);
        m_boundingBoxFixed = JsonHelper::getBool(json, "static", false);
        json = jsonRoot["bounding_box"]["position"];
        m_boundingBoxLat = JsonHelper::getDouble(json, "latitude", 52.542);
        m_boundingBoxLon = JsonHelper::getDouble(json, "longitude", 13.413);

        //----------------------------------------------------------------------
        // database
        json = jsonRoot["database"];
        std::string dbPath = JsonHelper::getString(json, "path", "test.sqlite");
        m_databasePath = ofToDataPath(dbPath, true);

        //----------------------------------------------------------------------
        // db query
        json = jsonRoot["dbquery"];
        m_queryType = JsonHelper::getInt(json, "type", 4);
        json = jsonRoot["dbquery"]["time_range"];
        m_queryYearStart = JsonHelper::getInt(json, "year_start", 2009);
        m_queryYearEnd = JsonHelper::getInt(json, "year_end", 2010);

        m_queryCity = JsonHelper::getString(json, "city", "Berlin");

        //----------------------------------------------------------------------
        // regions
        json = jsonRoot["regions"];
        m_regionsOn = JsonHelper::getBool(json, "enabled", true);

        json = jsonRoot["regions"]["meridian"];
        m_meridianAuto = JsonHelper::getBool(json, "auto", true);
        m_meridianVal = JsonHelper::getDouble(json, "lon0", 0.0);

        json = jsonRoot["regions"]["coordinates"];
        double regions[5][3] = {{-119.0, -180.0, -100.0},
                                {-74.0, -100.0, -35.0},
                                {12.0, -35.0, 65.0},
                                {116.0, 65.0, 130.0},
                                {146.0, 130.0, 180.0}};

        for (size_t i = 0; i < MAX(5, json.size()); ++i)
        {
            Json::Value region = json[i];
            m_regions[i].lon0 = JsonHelper::getDouble(region, "lon0", regions[i][0]);
            m_regions[i].minLon = JsonHelper::getDouble(region, "minlon", regions[i][1]);
            m_regions[i].maxLon = JsonHelper::getDouble(region, "maxlon", regions[i][2]);
        }

        //----------------------------------------------------------------------
        // users
        json = jsonRoot["users"];
        m_numPerson = json.size();
        for(size_t i = 0; i < m_numPerson; ++i)
        {
            Json::Value person = json[i];
            m_names.push_back(JsonHelper::getString(person, "name", ""));
            ofColor tmpColor;
            Json::Value personColor = person["color"];
            tmpColor.r = JsonHelper::getDouble(personColor, "r", ofRandom(0, 255));
            tmpColor.g = JsonHelper::getDouble(personColor, "g", ofRandom(0, 255));
            tmpColor.b = JsonHelper::getDouble(personColor, "b", ofRandom(0, 255));
            tmpColor.a = JsonHelper::getDouble(personColor, "a", ofRandom(0, 255));
            m_nameColors.push_back(tmpColor);

            std::string sqlFilePath = JsonHelper::getString(person, "sql", "");
            m_sqlFilePaths.push_back(sqlFilePath);
        }

        //----------------------------------------------------------------------
        // current point images

        json = jsonRoot["current_point_images"];

        m_imageAsCurrentPoint = JsonHelper::getBool(json, "enabled", false);

        if(m_imageAsCurrentPoint)
        {
            json = jsonRoot["current_point_images"]["images"];
            for(size_t i = 0; i < json.size(); ++i)
            {
                Json::Value img = json[i];
                CurrentImageData cid;
                cid.path = JsonHelper::getString(img, "path", "");
                cid.width = JsonHelper::getDouble(img, "width", 0.0);
                cid.height = JsonHelper::getDouble(img, "height", 0.0);
                cid.alpha = JsonHelper::getInt(img, "alpha", 255);
                m_currImageData.push_back(cid);
            }
        }

        //----------------------------------------------------------------------
        // modes
        json = jsonRoot["modes"]["interactive_mode"];
        m_interactiveMode = JsonHelper::getBool(json, "enabled", false);
        m_interactiveTraced = JsonHelper::getBool(json, "traced", true);

        json = jsonRoot["modes"]["interactive_mode"]["current_segment"]["color"];
        m_colorInteractiveSegR = JsonHelper::getInt(json, "r", 255);
        m_colorInteractiveSegG = JsonHelper::getInt(json, "g", 255);
        m_colorInteractiveSegB = JsonHelper::getInt(json, "b", 0);
        m_colorInteractiveSegA = JsonHelper::getInt(json, "a", 255);

        json = jsonRoot["modes"]["multi_mode"];
        m_multiMode = JsonHelper::getBool(json, "enabled", false);
        m_multiModeInfo = JsonHelper::getBool(json, "show_info", false);

        json = jsonRoot["modes"]["speed_mode"];
        m_useSpeed = JsonHelper::getBool(json, "enabled", false);
        m_speedThreshold = JsonHelper::getDouble(json, "threshold", 0.0);

        json = jsonRoot["modes"]["speed_mode"]["colors"]["below_threshold"];
        m_speedColorUnder.r = JsonHelper::getInt(json, "r", 255);
        m_speedColorUnder.g = JsonHelper::getInt(json, "g", 255);
        m_speedColorUnder.b = JsonHelper::getInt(json, "b", 255);
        m_speedColorUnder.a = JsonHelper::getInt(json, "a", 255);

        json = jsonRoot["modes"]["speed_mode"]["colors"]["above_threshold"];
        m_speedColorAbove.r = JsonHelper::getInt(json, "r", 255);
        m_speedColorAbove.g = JsonHelper::getInt(json, "g", 255);
        m_speedColorAbove.b = JsonHelper::getInt(json, "b", 255);
        m_speedColorAbove.a = JsonHelper::getInt(json, "a", 255);

        json = jsonRoot["location_images"]["images"];
        m_locationImgData.clear();
        for(size_t i = 0; i < json.size(); ++i)
        {
            Json::Value img = json[i];
            LocationImageData lid;
            lid.path = JsonHelper::getString(img, "path", "");
            lid.name = JsonHelper::getString(img, "name", "");
            lid.gps.lat = JsonHelper::getDouble(img, "lat", 0.0);
            lid.gps.lon = JsonHelper::getDouble(img, "lon", 0.0);
            lid.width = JsonHelper::getDouble(img, "width", 0.0);
            lid.height = JsonHelper::getDouble(img, "height", 0.0);
            lid.alpha = JsonHelper::getInt(img, "alpha", 255);
            Json::Value anchor = img["anchor"];
            lid.anchorType = JsonHelper::getInt(anchor, "type", 1);
            lid.anchorX = JsonHelper::getDouble(anchor, "posx", 0.5);
            lid.anchorY = JsonHelper::getDouble(anchor, "posy", 0.5);
            lid.anchorShow = JsonHelper::getBool(anchor, "show", false);

            m_locationImgData.push_back(lid);
        }

        json = jsonRoot["zoom_animation"];
        m_isZoomAnimation = JsonHelper::getBool(json, "enabled", false);
        m_zoomAnimationAttractionCriteria = JsonHelper::getInt(json, "type", 1);
        m_zoomanimationUseOnlyZ = JsonHelper::getBool(json, "onlyz", false);

        json = jsonRoot["zoom_animation"]["z"];
        m_zoomAnimationDamp = JsonHelper::getDouble(json, "damp", 0.2);
        m_zoomAnimationAttraction = JsonHelper::getDouble(json, "attraction", 0.2);
        json = jsonRoot["zoom_animation"]["xy"];
        m_zoomAnimationDampCenter = JsonHelper::getDouble(json, "damp", 0.2);
        m_zoomAnimationAttractionCenter = JsonHelper::getDouble(json, "attraction", 0.2);

        json = jsonRoot["zoom_animation"]["frames"];
        for(size_t i = 0; i < json.size(); ++i)
        {
            Json::Value frame = json[i];
            ZoomAnimFrame zaf;
            zaf.frameTime = JsonHelper::getDouble(frame, "time", 0.0);
            zaf.frameZoom = JsonHelper::getInt(frame, "zoom", 0);
            zaf.frameCenterX = JsonHelper::getDouble(frame, "lon", 0.0);
            zaf.frameCenterY = JsonHelper::getDouble(frame, "lat", 0.0);
            zaf.gpsId = JsonHelper::getInt(frame, "gpsid", -1);
            zaf.timestamp = JsonHelper::getString(frame, "timestamp", "");
            m_zoomAnimationFrames.push_back(zaf);
        }

        //----------------------------------------------------------------------
        // sound
        json = jsonRoot["sound"];
        m_isSoundActive = JsonHelper::getBool(json, "enabled", false);

        json = jsonRoot["sound"]["soundfiles"];
        for (size_t i = 0; i < json.size(); ++i)
        {
            Json::Value soundFile = json[i];
            if (soundFile.isString())
            {
                m_soundFiles.push_back(soundFile.asString());
            }
        }

        //----------------------------------------------------------------------
        // shader
        json = jsonRoot["shader"];
        m_useShader = JsonHelper::getBool(json, "enabled", false);
        m_vertexShaderSource = JsonHelper::getString(json, "vertex", "");
        m_fragmentShaderSource = JsonHelper::getString(json, "fragment", "");

        return true;
    }
    else
    {
        cout  << "Failed to parse JSON" << endl;
        return false;
    }
}

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

    ofLog(OF_LOG_SILENT, "Meridian value: %lf, auto = %d",
          m_meridianVal, m_meridianAuto);

    ofLog(OF_LOG_SILENT, "Database path: %s", m_databasePath.c_str());

    ofLog(OF_LOG_SILENT, "Number of person: %d", m_numPerson);

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
    ofLog(OF_LOG_SILENT, "------------------------------\n");

}

//------------------------------------------------------------------------------

const std::string& AppSettings::getFontName(const string& id)
{
    return m_fonts[id].first;
}

int AppSettings::getFontSize(const string &id)
{
    return m_fonts[id].second;
}

//------------------------------------------------------------------------------
