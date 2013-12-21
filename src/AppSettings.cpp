#include "AppSettings.h"
#include "ofxXmlSettings.h"
#include "ofxJSONElement.h"
#include <jsoncpp/json/json.h>

//static const char* settingsPath = "AppSettings.xml";

AppSettings::AppSettings(const std::string& path)
:
m_initialized(false),
m_settingsFilePath(path),
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
    m_debugMode = m_xml.getValue("settings:debugmode", 0) == 1 ? true : false;

    m_walkLength = m_xml.getValue("settings:walklength", 10000);

    m_boundingBoxAuto = m_xml.getValue("settings:boundingbox:auto", 1) == 1 ? true : false;
    m_boundingBoxSize = m_xml.getValue("settings:boundingbox:size", 3000.0);
    m_boundingBoxPadding = m_xml.getValue("settings:boundingbox:padding", 500.0);
    m_boundingBoxFixed = m_xml.getValue("settings:boundingbox:static", 0) == 1 ? true : false;
    m_boundingBoxLat = m_xml.getAttribute("settings:boundingbox:position", "lat", 52.542);
    m_boundingBoxLon = m_xml.getAttribute("settings:boundingbox:position", "lon", 13.413);
    
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
    m_fullscreen = m_xml.getValue("settings:fullscreen", 0) == 1 ? true : false;
    m_imageAsCurrentPoint = m_xml.getValue("ui:imageascurrent", 0) == 1 ? true : false;
    m_hideCursor = m_xml.getValue("settings:hidecursor", 0) == 1 ? true : false;

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

    m_interactiveMode = m_xml.getValue("settings:interactivemode:enabled", 0) == 1 ? true : false;
    m_interactiveTraced = m_xml.getValue("settings:interactivemode:traced", 1) == 1 ? true : false;

    m_showInfo = m_xml.getValue("settings:showinfo", 1) == 1? true : false;

    m_regionsOn = m_xml.getValue("settings:meridian:regions", 1) == 1 ? true : false;

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

    m_loop = m_xml.getValue("settings:loop", 1) == 1 ? true : false;

    m_multiMode = m_xml.getValue("settings:multimode", 0) == 1 ? true : false;
    m_multiModeInfo = m_xml.getValue("settings:multimodeinfo", 0) == 1 ? true : false;

    m_sleepTime = m_xml.getValue("settings:sleeptime", 0);

    m_useSpeed = m_xml.getValue("settings:usespeed", 0) == 1 ? true : false;
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
        lid.anchorShow = m_xml.getValue("anchor:show", 0) == 1 ? true : false;

        m_locationImgData.push_back(lid);
        m_xml.popTag();
    }

    m_xml.popTag();
    m_xml.popTag();

    m_xml.pushTag("zoomanimation");
    m_isZoomAnimation = m_xml.getValue("active", 0) == 1 ? true : false;
    m_zoomAnimationDamp = m_xml.getValue("dampzoom", 0.2);
    m_zoomAnimationAttraction = m_xml.getValue("attractionzoom", 0.2);
    m_zoomAnimationDampCenter = m_xml.getValue("dampcenter", 0.2);
    m_zoomAnimationAttractionCenter = m_xml.getValue("attractioncenter", 0.2);
    m_zoomAnimationAttractionCriteria = m_xml.getValue("type", 1);
    m_zoomanimationUseOnlyZ = m_xml.getValue("onlyz", 0) == 1 ? true : false;
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
    m_isSoundActive = m_xml.getValue("active", 0) == 1 ? true : false;

    m_xml.pushTag("soundfiles");
    int numSoundFiles = m_xml.getNumTags("soundfile");
    for (int i = 0; i < numSoundFiles; ++i) {
		m_soundFiles.push_back(m_xml.getAttribute("soundfile", "src", "", i));
	}
    m_xml.popTag();
	m_xml.popTag();

    m_vertexShaderSource = m_xml.getValue("shader:vertex", "");
    m_fragmentShaderSource = m_xml.getValue("shader:fragment", "");
    m_useShader = m_xml.getValue("shader:enabled", 0) == 1 ? true : false;

    m_printSettings = m_xml.getValue("settings:printvalues", 0) == 1 ? true : false;

    m_grabScreen = m_xml.getValue("settings:grabscreen", 0) == 1 ? true : false;

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

//        std::vector<DrawingLifeFont> defaultFonts;

//        defaultFonts.push_back(DrawingLifeFont("title", "mono.ttf", 50));
//        defaultFonts.push_back(DrawingLifeFont("author", "mono.ttf", 24));
//        defaultFonts.push_back(DrawingLifeFont("text", "mono.ttf", 16));
//        defaultFonts.push_back(DrawingLifeFont("info", "consola.ttf", 50));

//        for (size_t i = 0; i < json.size(); ++i)
//        {
//            std::string defaultId = boost::get<0>(defaultFonts[i]);
//            std::string defaultName = boost::get<1>(defaultFonts[i]);
//            int defaultSize = boost::get<2>(defaultFonts[i]);

//            std::string id = json[i].get("id", defaultId).asString();
//            std::string name = json[i].get("name", defaultName).asString();
//            int fontSize = json[i].get("size", defaultSize).asInt();

//            m_fonts.push_back(DrawingLifeFont(id, name, fontSize));
//        }

        if (json.size() == 4)
        {
            Json::UInt idx = 0;
            m_fontTitleName = json[idx].get("name", "mono.ttf").asString();
            m_fontTitleSize = json[idx].get("size", 50).asInt();
            ++idx;
            m_fontAuthorName = json[idx].get("name", "mono.ttf").asString();
            m_fontAuthorSize = json[idx].get("size", 24).asInt();
            ++idx;
            m_fontTextName = json[idx].get("name", "mono.ttf").asString();
            m_fontTextSize = json[idx].get("size", 16).asInt();
            ++idx;
            m_fontInfoName = json[idx].get("name", "consola.ttf").asString();
            m_fontInfoSize = json[idx].get("size", 11).asInt();
        }

        json = jsonRoot["ui"]["colors"];
        m_colorForegroundR = json["foreground"].get("r", 255).asInt();
        m_colorForegroundG = json["foreground"].get("g", 255).asInt();
        m_colorForegroundB = json["foreground"].get("b", 255).asInt();

        m_colorBackgroundR = json["background"].get("r", 0).asInt();
        m_colorBackgroundG = json["background"].get("g", 0).asInt();
        m_colorBackgroundB = json["background"].get("b", 0).asInt();

        m_colorViewboxR = json["viewbox"].get("r", 0).asInt();
        m_colorViewboxG = json["viewbox"].get("g", 0).asInt();
        m_colorViewboxB = json["viewbox"].get("b", 0).asInt();

        json = jsonRoot["ui"]["alpha"];
        m_alphaTracks = json.get("path", 64).asInt();
        m_alphaDots = json.get("dots", 127).asInt();
        m_alphaLegend = json.get("legend", 255).asInt();

        json = jsonRoot["ui"];
        m_dotSize = json.get("dotsize", 5).asInt();

        //----------------------------------------------------------------------
        // settings
        json = jsonRoot["settings"];
        m_logLevel = json.get("level_level", 0).asInt();
        m_debugMode = json.get("debugmode", false).asBool();
        m_loadOnStart = json.get("load_data_on_start", true).asBool();
        m_fullscreen = json.get("fullscreen", false).asBool();
        m_hideCursor = json.get("hide_cursor", false).asBool();
        m_showInfo = json.get("showinfo", true).asBool();
        m_loop = json.get("loop", true).asBool();
        m_sleepTime = json.get("sleep_time", 0).asInt();
        m_printSettings = json.get("print_values", false).asBool();
        m_grabScreen = json.get("grab_screen", false).asBool();

        //----------------------------------------------------------------------
        // animation
        json = jsonRoot["animation"];
        m_walkLength = json.get("walklength", 10000).asInt();
        m_drawSpeed = json.get("speed", 1.0f).asDouble();
        m_frameRate = json.get("framerate", 30).asInt();

        //----------------------------------------------------------------------
        // bounding box
        json = jsonRoot["bounding_box"];
        m_boundingBoxAuto = json.get("auto", true).asBool();
        m_boundingBoxSize = json.get("size", 3000.0).asDouble();
        m_boundingBoxPadding = json.get("padding", 500.0).asDouble();
        m_boundingBoxFixed = json.get("static", false).asBool();
        m_boundingBoxLat = json["position"].get("latitude", 52.542).asDouble();
        m_boundingBoxLon = json["position"].get("longitude", 13.413).asDouble();

        //----------------------------------------------------------------------
        // database
        json = jsonRoot["database"];
        std::string dbPath = json.get("path", "test.sqlite").asString();
        m_databasePath = ofToDataPath(dbPath, true);

        //----------------------------------------------------------------------
        // db query
        json = jsonRoot["dbquery"];
        m_queryType = json.get("type", 4).asInt();
        m_queryYearStart = json["time_range"].get("year_start", 2009).asInt();
        m_queryYearEnd = json["time_range"].get("year_end", 2010).asInt();
        m_queryCity = json.get("city", "Berlin").asString();

        //----------------------------------------------------------------------
        // regions
        json = jsonRoot["regions"];
        m_regionsOn = json.get("enabled", true).asBool();

        json = jsonRoot["regions"]["meridian"];
        m_meridianAuto = json.get("auto", true).asBool();
        m_meridianVal = json.get("lon0", 0.0).asDouble();

        json = jsonRoot["regions"]["coordinates"];
        double regions[5][3] = {{-119.0, -180.0, -100.0},
                                {-74.0, -100.0, -35.0},
                                {12.0, -35.0, 65.0},
                                {116.0, 65.0, 130.0},
                                {146.0, 130.0, 180.0}};

        for (size_t i = 0; i < MAX(5, json.size()); ++i)
        {
            Json::Value region = json[i];
            m_regions[i].lon0 = region.get("lon0", regions[i][0]).asDouble();
            m_regions[i].minLon = region.get("minlon", regions[i][1]).asDouble();
            m_regions[i].maxLon = region.get("maxlon", regions[i][2]).asDouble();
        }

        //----------------------------------------------------------------------
        // users
        json = jsonRoot["users"];
        m_numPerson = json.size();
        for(size_t i = 0; i < m_numPerson; ++i)
        {
            Json::Value person = json[i];
            m_names.push_back(person.get("name", "").asString());
            ofColor tmpColor;
            Json::Value personColor = person["color"];
            tmpColor.r = personColor.get("r", ofRandom(0, 255)).asDouble();
            tmpColor.g = personColor.get("g", ofRandom(0, 255)).asDouble();
            tmpColor.b = personColor.get("b", ofRandom(0, 255)).asDouble();
            tmpColor.a = personColor.get("a", ofRandom(0, 255)).asDouble();
            m_nameColors.push_back(tmpColor);

            std::string sqlFilePath = person.get("sql", "").asString();
            m_sqlFilePaths.push_back(sqlFilePath);
        }

        //----------------------------------------------------------------------
        // current point images

        json = jsonRoot["current_point_images"];

        m_imageAsCurrentPoint = json.get("enabled", false).asBool();

        if(m_imageAsCurrentPoint)
        {
            json = jsonRoot["current_point_images"]["images"];
            for(size_t i = 0; i < json.size(); ++i)
            {
                Json::Value img = json[i];
                CurrentImageData cid;
                cid.path = img.get("path", "").asString();
                cid.width = img.get("width", 0.0).asDouble();
                cid.height = img.get("height", 0.0).asDouble();
                cid.alpha = img.get("alpha", 255).asInt();
                m_currImageData.push_back(cid);
            }
        }

        //----------------------------------------------------------------------
        // modes
        json = jsonRoot["modes"]["interactive_mode"];
        m_interactiveMode = json.get("enabled", false).asBool();
        m_interactiveTraced = json.get("traced", true).asBool();

        json = jsonRoot["modes"]["interactive_mode"]["current_segment"]["color"];
        m_colorInteractiveSegR = json.get("r", 255).asInt();
        m_colorInteractiveSegG = json.get("g", 255).asInt();
        m_colorInteractiveSegB = json.get("b", 0).asInt();
        m_colorInteractiveSegA = json.get("a", 255).asInt();

        json = jsonRoot["modes"]["multi_mode"];
        m_multiMode = json.get("enabled", false).asBool();
        m_multiModeInfo = json.get("show_info", false).asBool();

        json = jsonRoot["modes"]["speed_mode"];
        m_useSpeed = json.get("enabled", false).asBool();
        m_speedThreshold = json.get("threshold", 0.0).asDouble();

        json = jsonRoot["modes"]["speed_mode"]["colors"]["below_threshold"];
        m_speedColorUnder.r = json.get("r", 255).asInt();
        m_speedColorUnder.g = json.get("g", 255).asInt();
        m_speedColorUnder.b = json.get("b", 255).asInt();
        m_speedColorUnder.a = json.get("a", 255).asInt();

        json = jsonRoot["modes"]["speed_mode"]["colors"]["above_threshold"];
        m_speedColorAbove.r = json.get("r", 255).asInt();
        m_speedColorAbove.g = json.get("g", 255).asInt();
        m_speedColorAbove.b = json.get("b", 255).asInt();
        m_speedColorAbove.a = json.get("a", 255).asInt();

        json = jsonRoot["location_images"]["images"];
        m_locationImgData.clear();
        for(size_t i = 0; i < json.size(); ++i)
        {
            Json::Value img = json[i];
            LocationImageData lid;
            lid.path = img.get("path", "").asString();
            lid.name = img.get("name", "").asString();
            lid.gps.lat = img.get("lat", 0.0).asDouble();
            lid.gps.lon = img.get("lon", 0.0).asDouble();
            lid.width = img.get("width", 0.0).asDouble();
            lid.height = img.get("height", 0.0).asDouble();
            lid.alpha = img.get("alpha", 255).asInt();
            Json::Value anchor = img["anchor"];
            lid.anchorType = anchor.get("type", 1).asInt();
            lid.anchorX = anchor.get("posx", 0.5).asDouble();
            lid.anchorY = anchor.get("posy", 0.5).asDouble();
            lid.anchorShow = anchor.get("show", false).asBool();

            m_locationImgData.push_back(lid);
        }

        json = jsonRoot["zoom_animation"];
        m_isZoomAnimation = json.get("enabled", false).asBool();
        m_zoomAnimationAttractionCriteria = json.get("type", 1).asInt();
        m_zoomanimationUseOnlyZ = json.get("onlyz", false).asBool();

        json = jsonRoot["zoom_animation"]["z"];
        m_zoomAnimationDamp = json.get("damp", 0.2).asDouble();
        m_zoomAnimationAttraction = json.get("attraction", 0.2).asDouble();
        json = jsonRoot["zoom_animation"]["xy"];
        m_zoomAnimationDampCenter = json.get("damp", 0.2).asDouble();
        m_zoomAnimationAttractionCenter = json.get("attraction", 0.2).asDouble();

        json = jsonRoot["zoom_animation"]["frames"];
        for(size_t i = 0; i < json.size(); ++i)
        {
            Json::Value frame = json[i];
            ZoomAnimFrame zaf;
            zaf.frameTime = frame.get("time", 0.0).asDouble();
            zaf.frameZoom = frame.get("zoom", 0).asInt();
            zaf.frameCenterX = frame.get("lon", 0.0).asDouble();
            zaf.frameCenterY = frame.get("lat", 0.0).asDouble();
            zaf.gpsId = frame.get("gpsid", -1).asInt();
            zaf.timestamp = frame.get("timestamp", "").asString();
            m_zoomAnimationFrames.push_back(zaf);
        }

        //----------------------------------------------------------------------
        // sound
        json = jsonRoot["sound"];
        m_isSoundActive = json.get("enabled", false).asBool();

        json = jsonRoot["sound"]["soundfiles"];
        for (size_t i = 0; i < json.size(); ++i)
        {
            m_soundFiles.push_back(json[i].asString());
        }

        //----------------------------------------------------------------------
        // shader
        json = jsonRoot["shader"];
        m_useShader = json.get("enabled", false).asBool();
        m_vertexShaderSource = json.get("vertex", "").asString();
        m_fragmentShaderSource = json.get("fragment", "").asString();

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
          m_fontTitleName.c_str(), m_fontTitleSize);
    ofLog(OF_LOG_SILENT, "Font: author | name = %s, size = %d",
          m_fontAuthorName.c_str(), m_fontAuthorSize);
    ofLog(OF_LOG_SILENT, "Font: text | name = %s, size = %d",
          m_fontTextName.c_str(), m_fontTextSize);
    ofLog(OF_LOG_SILENT, "Font: info | name = %s, size = %d",
          m_fontInfoName.c_str(), m_fontInfoSize);

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
