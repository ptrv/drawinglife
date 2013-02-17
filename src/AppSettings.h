#ifndef _APPSETTINGS_H_
#define _APPSETTINGS_H_

#include <string>
#include <vector>

#include "DrawingLifeIncludes.h"

class AppSettings
{
public:
    AppSettings(std::string path);
    ~AppSettings();


    void print();

    //bool shouldPrintSettings() const { return m_printSettings; }

    const std::string& getFontTitleName() const { return m_fontTitleName; }
    int getFontTitleSize() const { return m_fontTitleSize; }
    const std::string& getFontAuthorName() const { return m_fontAuthorName; }
    int getFontAuthorSize() const { return m_fontAuthorSize; }
    const std::string& getFontTextName() const { return m_fontTextName; }
    int getFontTextSize() const { return m_fontTextSize; }
    const std::string& getFontInfoName() const { return m_fontInfoName; }
    int getFontInfoSize() const { return m_fontInfoSize; }

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

    bool isMeridianAuto() const { return m_meridianAuto; }
    double getMeridian() const { return m_meridianVal; }

    const std::string& getDatabasePath() const { return m_databasePath; }

    int getQueryType() const { return m_queryType; }
    int getQueryYearStart() const { return m_queryYearStart; }
    int getQueryYearEnd() const { return m_queryYearEnd; }
    const std::string& getQueryCity() const { return m_queryCity; }

    unsigned int getNumPerson() const { return m_numPerson; }

    const std::vector<std::string >& getNames() const { return m_names; }
    const std::vector<ofColor >& getNameColors() const { return m_nameColors; }
    const std::vector<std::string >& getSqlFilePaths() const { return m_sqlFilePaths; }
    const std::vector<CurrentImageData >& getCurrentImageData() const { return m_currImageData; }

    bool isInteractiveMode() const { return m_interactiveMode; }
    bool drawTraced() const { return m_interactiveTraced; }

    bool showInfo() const { return m_showInfo; }

    const GpsRegion* getRegions() const { return m_regions; }
    bool isRegionsOn() const { return m_regionsOn; }

    bool isLoopOn() const { return m_loop; }

    bool isMultiMode() const { return m_multiMode; }
    bool isMultiModeInfo() const { return m_multiModeInfo; }

    int getSleepTime() const { return m_sleepTime; }

    bool useSpeed() const { return m_useSpeed; }

    double getSpeedThreshold() { return m_speedThreshold; }

    const ofColor& getSpeedColorUnder() const { return m_speedColorUnder; }
    const ofColor& getSpeedColorAbove() const { return m_speedColorAbove; }

    const std::vector<LocationImageData>& getLocationImageData() const { return m_locationImgData; }
    
    bool isBoundingBoxFixed() const { return m_boundingBoxFixed; };
    double getBoundingBoxLat() const { return m_boundingBoxLat; } 
    double getBoundingBoxLon() const { return m_boundingBoxLon; }
    
    bool isZoomAnimation() const { return m_isZoomAnimation; }
    const std::vector<ZoomAnimFrame>& getZoomAnimFrames() const { return m_zoomAnimationFrames; }

    double getZoomAnimationDamp() const { return m_zoomAnimationDamp; }
    double getZoomAnimationAttraction() const { return m_zoomAnimationAttraction;}
    double getZoomAnimationDampCenter() const { return m_zoomAnimationDampCenter; }
    double getZoomAnimationAttractionCenter() const { return m_zoomAnimationAttractionCenter;}

    int getZoomAnimationCriteria() const { return m_zoomAnimationAttractionCriteria; }
    bool getUseOnlyZ() const { return m_zoomanimationUseOnlyZ; }

    bool isSoundActive() const { return m_isSoundActive; }
    const std::vector<std::string>& getSoundFiles() const { return m_soundFiles; }
private:
    std::string m_settingsFilePath;
    // -----------------------------------------------------------------------------
    // Fonts
    // -----------------------------------------------------------------------------
    std::string m_fontTitleName;
    int m_fontTitleSize;
    std::string m_fontAuthorName;
    int m_fontAuthorSize;
    std::string m_fontTextName;
    int m_fontTextSize;
    std::string m_fontInfoName;
    int m_fontInfoSize;

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
    unsigned int m_numPerson;
    std::vector<std::string > m_names;
    std::vector<ofColor > m_nameColors;
    std::vector<std::string > m_sqlFilePaths;
    std::vector<CurrentImageData > m_currImageData;

    bool m_interactiveMode;
    bool m_interactiveTraced;

    bool m_meridianAuto;
    double m_meridianVal;

    bool m_showInfo;

    GpsRegion m_regions[5];
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
    std::vector<ZoomAnimFrame> m_zoomAnimationFrames;
    double m_zoomAnimationDamp;
    double m_zoomAnimationAttraction;
    double m_zoomAnimationDampCenter;
    double m_zoomAnimationAttractionCenter;

    bool m_isSoundActive;
    std::vector<std::string> m_soundFiles;

    int m_zoomAnimationAttractionCriteria;
    bool m_zoomanimationUseOnlyZ;
};

#endif // _APPSETTINGS_H_
