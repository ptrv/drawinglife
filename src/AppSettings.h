#ifndef _APPSETTINGS_H_
#define _APPSETTINGS_H_

#include <string>
#include <vector>

#define DEF_SINGLETON( NAME )    \
public:                        \
static NAME& instance()      \
{                            \
   static NAME _instance;    \
   return _instance;         \
}                            \
private:                       \
NAME();               \
NAME( const NAME& );

class AppSettings
{
    DEF_SINGLETON(AppSettings);
public:

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

    int getAlphaTrack () const { return m_alphaTracks; }
    int getAlphaDot () const { return m_alphaDots; }
    int getAlphaLegend () const { return m_alphaLegend; }

    int getLogLevel() const { return m_logLevel; }
    int getWalkLength() const { return m_walkLength; }
    int getDrawSpeed() const { return m_drawSpeed; }
    int getFrameRate() const { return m_frameRate; }
    bool isDebugMode() const { return m_debugMode; }
    bool loadGpsOnStart() const { return m_loadOnStart; }
    bool isFullscreen() const { return m_fullscreen; }
    bool isCurrentPointImage() const { return m_imageAsCurrentPoint; }
    bool hideCursor() const { return m_hideCursor; }

    double getBoundingBoxSize() const { return m_boundingBoxSize; }
    double getBoundingBoxPadding() const { return m_boundingBoxPadding; }

    const std::string& getDatabasePath() const { return m_databasePath; }

    int getQueryType() const { return m_queryType; }
    int getQueryYearStart() const { return m_queryYearStart; }
    int getQueryYearEnd() const { return m_queryYearEnd; }
    const std::string& getQueryCity() const { return m_queryCity; }

    int getNumPerson() const { return m_numPerson; }

    const std::vector<std::string >& getNames() const { return m_names; }
    const std::vector<std::string >& getImageList() const { return m_imagePaths; }

private:

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

    int m_alphaTracks;
    int m_alphaDots;
    int m_alphaLegend;

    // -----------------------------------------------------------------------------
    // Settings
    // -----------------------------------------------------------------------------
    bool m_printSettings;
    int m_logLevel;
    bool m_debugMode;
    int m_walkLength;
    int m_drawSpeed;
    bool m_loadOnStart;
    int m_frameRate;

    bool m_fullscreen;
    bool m_imageAsCurrentPoint;
    bool m_hideCursor;

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
    int m_numPerson;
    std::vector<std::string > m_names;
    std::vector<std::string > m_imagePaths;

};

#endif // _APPSETTINGS_H_
