#ifndef VIEWHELPER_H
#define VIEWHELPER_H

#include "DrawingLifeIncludes.h"

class DrawingLifeApp;

class ViewHelper
{
public:

    static void drawFPS();
    static void drawKeyCommands(const ofColor& c);
    static void drawStartScreen(const ofTrueTypeFont& fontTitle,
                                const ofTrueTypeFont& fontAuthor);

    static void setViewAspectRatio(DrawingLifeApp& app);
    static void fillViewAreaUTM(DrawingLifeApp& app);

    static void drawInfo(const AppSettings& settings,
                         const GpsData& gpsData,
                         const Walk& walk,
                         const ViewDimensions& viewDimensions,
                         const ofTrueTypeFont& font,
                         const size_t currentPerson);
    static void drawInfoDebug(const AppSettings& settings,
                              const MagicBox& magicBox,
                              const GpsData& gpsData,
                              const Walk& walk,
                              const size_t currentPerson);
    static void drawInfoMultiMode(const AppSettings& settings,
                                  const Timeline& timeline,
                                  const ViewDimensions& viewDimensions,
                                  const ofTrueTypeFont& font);

private:
    ViewHelper();
};

#endif // VIEWHELPER_H
