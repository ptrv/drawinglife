#ifndef VIEWHELPER_H
#define VIEWHELPER_H

#include "DrawingLifeIncludes.h"

class DrawingLifeApp;

class ViewHelper
{
public:

    static void drawFPS();
    static void drawKeyCommands(const ofColor& c);
    static void drawStartScreen(ofTrueTypeFont& fontTitle,
                                ofTrueTypeFont& fontAuthor);

    static void setViewAspectRatio(DrawingLifeApp& app);
    static void fillViewAreaUTM(DrawingLifeApp& app);

private:
    ViewHelper();
};

#endif // VIEWHELPER_H
