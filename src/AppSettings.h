#ifndef _APPSETTINGS_H_
#define _APPSETTINGS_H_

#include <string>
#include <vector>

#include "DrawingLifeIncludes.h"

class AppSettings
{
public:
    AppSettings(const std::string& path);
    ~AppSettings();

    bool initialized() const { return m_initialized; }

    void print();

    //bool shouldPrintSettings() const { return m_printSettings; }

    inline const std::string& getFontName(const std::string& id);
    inline int getFontSize(const std::string& id);

    const DrawingLifeFontMap& getFonts() const { return m_fonts; }

    int getColorForegroundR () const {return m_colorForegroundR; }
    int getColorForegroundG () const {return m_colorForegroundG; }
    int getColorForegroundB () const {return m_colorForegroundB; }

    int getColorBackgroundR () const { return m_colorBackgroundR; }
    int getColorBackgroundG () const { return m_colorBackgroundG; }
    int getColorBackgroundB () const { return m_colorBackgroundB; }

    int getColorViewboxR () const { return m_colorViewboxR; }
    int getColorViewboxG () const { return m_colorViewboxG; }
    int getColorViewboxB () const { return m_colorViewboxB; }

    int getColorInteractiveSegR () const { return m_colorInteractiveSegR; }
    int getColorInteractiveSegG () const { return m_colorInteractiveSegG; }
    int getColorInteractiveSegB () const { return m_colorInteractiveSegB; }
    int getColorInteractiveSegA () const { return m_colorInteractiveSegA; }

    int getAlphaTrack () const { return m_alphaTracks; }
    int getAlphaDot () const { return m_alphaDots; }
    int getAlphaLegend () const { return m_alphaLegend; }

    int getDotSize() const { return m_dotSize; }

    int getLogLevel() const { return m_logLevel; }
    int getWalkLength() const { return m_walkLength; }
    float getDrawSpeed() const { return m_drawSpeed; }
    int getFrameRate() const { return m_frameRate; }
    bool isDebugMode() const { return m_debugMode; }
    bool loadGpsOnStart() const { return m_loadOnStart; }
    bool isFullscreen() const { return m_fullscreen; }
    bool isCurrentPointImage() const { return m_imageAsCurrentPoint; }
    bool hideCursor() const { return m_hideCursor; }

    bool isBoundingBoxAuto() const { return m_boundingBoxAuto; }
    double getBoundingBoxSize() const { return m_boundingBoxSize; }
    double getBoundingBoxPadding() const { return m_boundingBoxPadding; }
    bool showBoundingBox() const { return m_boundingBoxShow; }

    bool isMeridianAuto() const { return m_meridianAuto; }
    double getMeridian() const { return m_meridianVal; }

    const std::string& getDatabasePath() const { return m_databasePath; }

    int getQueryType() const { return m_queryType; }
    int getQueryYearStart() const { return m_queryYearStart; }
    int getQueryYearEnd() const { return m_queryYearEnd; }
    const std::string& getQueryCity() const { return m_queryCity; }

    unsigned int getNumPersons() const { return m_numPersons; }

    const StringVec& getNames() const { return m_names; }
    const std::vector<ofColor>& getNameColors() const { return m_nameColors; }
    const StringVec& getSqlFilePaths() const { return m_sqlFilePaths; }
    const std::vector<CurrentPointImageData>& getCurrentImageData() const
    { return m_currImageData; }

    bool isInteractiveMode() const { return m_interactiveMode; }
    bool drawTraced() const { return m_interactiveTraced; }

    bool showInfo() const { return m_showInfo; }

    static const size_t NUM_REGIONS = 5;
    const GpsRegion* getRegions() const { return m_regions; }
    bool isRegionsOn() const { return m_regionsOn; }

    bool isLoopOn() const { return m_loop; }

    bool isMultiMode() const { return m_multiMode; }
    bool isMultiModeInfo() const { return m_multiModeInfo; }

    int getSleepTime() const { return m_sleepTime; }

    bool useSpeed() const { return m_useSpeed; }

    double getSpeedThreshold() const { return m_speedThreshold; }

    const ofColor& getSpeedColorUnder() const { return m_speedColorUnder; }
    const ofColor& getSpeedColorAbove() const { return m_speedColorAbove; }

    const std::vector<LocationImageData>& getLocationImageData() const
    { return m_locationImgData; }
    
    bool isBoundingBoxFixed() const { return m_boundingBoxFixed; }
    double getBoundingBoxLat() const { return m_boundingBoxLat; } 
    double getBoundingBoxLon() const { return m_boundingBoxLon; }
    
    bool isZoomAnimation() const { return m_isZoomAnimation; }
    const ZoomAnimFrameVec& getZoomAnimFrames() const { return m_zoomAnimationFrames; }

    double getZoomAnimationDamp() const { return m_zoomAnimationDamp; }
    double getZoomAnimationAttraction() const { return m_zoomAnimationAttraction;}
    double getZoomAnimationDampCenter() const { return m_zoomAnimationDampCenter; }
    double getZoomAnimationAttractionCenter() const { return m_zoomAnimationAttractionCenter;}

    int getZoomAnimationCriteria() const { return m_zoomAnimationAttractionCriteria; }
    bool getUseOnlyZ() const { return m_zoomanimationUseOnlyZ; }

    bool isSoundActive() const { return m_isSoundActive; }
    const StringVec& getSoundFiles() const { return m_soundFiles; }

    const std::string& getVertexShaderSource() const { return m_vertexShaderSource; }
    const std::string& getFragmentShaderSource() const { return m_fragmentShaderSource; }
    bool useShader() const { return m_useShader; }

    bool getIsGrabScreen() const { return m_grabScreen; }

private:

    bool loadXML();
    bool loadJSON();

    bool m_initialized;

    std::string m_settingsFilePath;
    // -----------------------------------------------------------------------------
    // Fonts
    // -----------------------------------------------------------------------------

    DrawingLifeFontMap m_fonts;

    // -----------------------------------------------------------------------------
    // Colors
    // -----------------------------------------------------------------------------
    int m_colorForegroundR;
    int m_colorForegroundG;
    int m_colorForegroundB;

    int m_colorBackgroundR;
    int m_colorBackgroundG;
    int m_colorBackgroundB;

    int m_colorViewboxR;
    int m_colorViewboxG;
    int m_colorViewboxB;

    int m_colorInteractiveSegR;
    int m_colorInteractiveSegG;
    int m_colorInteractiveSegB;
    int m_colorInteractiveSegA;

    int m_alphaTracks;
    int m_alphaDots;
    int m_alphaLegend;

    int m_dotSize;

    // -----------------------------------------------------------------------------
    // Settings
    // -----------------------------------------------------------------------------
    bool m_printSettings;
    int m_logLevel;
    bool m_debugMode;
    int m_walkLength;
    float m_drawSpeed;
    bool m_loadOnStart;
    int m_frameRate;

    bool m_fullscreen;
    bool m_imageAsCurrentPoint;
    bool m_hideCursor;

    bool m_boundingBoxAuto;
    double m_boundingBoxSize;
    double m_boundingBoxPadding;
    bool m_boundingBoxShow;
    // -----------------------------------------------------------------------------
    // Database
    // -----------------------------------------------------------------------------
    std::string m_databasePath;

    int m_queryType;
    int m_queryYearStart;
    int m_queryYearEnd;
    std::string m_queryCity;

    // -----------------------------------------------------------------------------
    // Data
    // -----------------------------------------------------------------------------
    unsigned int m_numPersons;
    StringVec m_names;
    std::vector<ofColor> m_nameColors;
    StringVec m_sqlFilePaths;
    std::vector<CurrentPointImageData> m_currImageData;

    bool m_interactiveMode;
    bool m_interactiveTraced;

    bool m_meridianAuto;
    double m_meridianVal;

    bool m_showInfo;

    GpsRegion m_regions[NUM_REGIONS];
    bool m_regionsOn;

    bool m_loop;

    bool m_multiMode;
    bool m_multiModeInfo;

    int m_sleepTime;

    std::vector<LocationImageData> m_locationImgData;

    bool m_useSpeed;
    double m_speedThreshold;
    ofColor m_speedColorUnder;
    ofColor m_speedColorAbove;
    
    bool m_boundingBoxFixed;
    double m_boundingBoxLat;
    double m_boundingBoxLon;

    bool m_isZoomAnimation;
    ZoomAnimFrameVec m_zoomAnimationFrames;
    double m_zoomAnimationDamp;
    double m_zoomAnimationAttraction;
    double m_zoomAnimationDampCenter;
    double m_zoomAnimationAttractionCenter;

    bool m_isSoundActive;
    StringVec m_soundFiles;

    int m_zoomAnimationAttractionCriteria;
    bool m_zoomanimationUseOnlyZ;

    std::string m_vertexShaderSource;
    std::string m_fragmentShaderSource;
    bool m_useShader;

    bool m_grabScreen;
};

//------------------------------------------------------------------------------

const std::string& AppSettings::getFontName(const string& id)
{
    return m_fonts[id].first;
}

//------------------------------------------------------------------------------

int AppSettings::getFontSize(const string &id)
{
    return m_fonts[id].second;
}

//------------------------------------------------------------------------------

#endif // _APPSETTINGS_H_
