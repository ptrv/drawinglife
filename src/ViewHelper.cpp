#include "ViewHelper.h"
#include "DrawingLifeIncludes.h"
#include "Utils.h"
#include "DrawingLifeApp.h"

//------------------------------------------------------------------------------

ViewHelper::ViewHelper()
{
}

//------------------------------------------------------------------------------

void ViewHelper::drawFPS()
{
    float fps = Utils::getFPS();
    ofSetHexColor(0xffffff);
    std::string str = "FPS: "+ofToString(static_cast<double>(fps), 1);
    ofDrawBitmapString(str, 30.0, ofGetHeight()-30 );

}

//------------------------------------------------------------------------------

void ViewHelper::drawKeyCommands(const ofColor& c)
{
    ofSetColor(c);
    std::stringstream stream;

    stream << "a           : draw all gps points\n";
    stream << "d           : debug mode\n";
    stream << "f           : toggle fullscreen\n";
    stream << "p           : show fps\n";
    stream << "k           : show key commands\n";
    stream << "+           : zoom in\n";
    stream << "-           : zoom out\n";
    stream << "left arrow  : move view left\n";
    stream << "right arrow : move view right\n";
    stream << "up arrow    : move view up\n";
    stream << "down arrow  : move view down\n";
    stream << "1 - 4       : set zoom level 1 - 4\n";
    stream << "0 - 9       : load gps data with a city (when querying city)\n";
    stream << "t           : toggle traced drawing (interactive mode)\n";
    stream << "space       : go to next segment (interactive mode)\n";
    stream << "backspace   : go to previous segment (interactive mode)\n";

   ofDrawBitmapString(stream.str(), 30, ofGetHeight() - 320);
}

//------------------------------------------------------------------------------

void ViewHelper::drawStartScreen(const ofTrueTypeFont& fontTitle,
                                 const ofTrueTypeFont& fontAuthor)
{
    ofSetColor(255,255,255);

    std::string title = APP_NAME_STR;
    title += " ";
    title += APP_VERSION_STR;

    const_cast<ofTrueTypeFont&>(fontTitle).drawString(title,
                                                      ofGetWidth()/2 - 365,
                                                      ofGetHeight()/2 - 100);

    const_cast<ofTrueTypeFont&>(fontAuthor).drawString(APP_AUTHOR_STR,
                                                       ofGetWidth()/2 - 91,
                                                       ofGetHeight()/2);
    const_cast<ofTrueTypeFont&>(fontAuthor).drawString("plan b",
                                                       ofGetWidth()/2 - 60,
                                                       ofGetHeight()/2 + 60);
}

//------------------------------------------------------------------------------

void ViewHelper::setViewAspectRatio(DrawingLifeApp& app)
{
    const AppSettings& settings = app.getAppSettings();
    const bool isMultiMode = settings.isMultiMode();
    const size_t numPersons = settings.getNumPersons();

    ViewDimensionsVec& viewDimensionsVec = app.getViewDimensionsVec();
    double width;
    double height;

    if (isMultiMode)
    {
        width = ofGetWidth();
    }
    else
    {
        width = ofGetWidth() / static_cast<int>(numPersons);
    }

    height = ofGetHeight();

    for (size_t i = 0; i < numPersons; ++i)
    {
        ViewDimensions& viewDimensions = viewDimensionsVec[i];

        // Reset for view padding.
        viewDimensions.offset.set(0, 0);

//        if (false)
//        {
//            m_viewMinDimension[personIndex] = height;
//
//            m_viewXOffset[personIndex] += m_viewPadding[personIndex];
//            m_viewYOffset[personIndex] += m_viewPadding[personIndex];
//        }
//        else
//        {
        // Set square view area and center.
        if (height < width)
        {
            viewDimensions.minDimension = height;
            viewDimensions.offset.x = (width - height) / 2.0;
        }
        else if (width < height)
        {
            viewDimensions.minDimension = width;
            viewDimensions.offset.y = (height - width) / 2.0;
        }
        else
        {
            viewDimensions.minDimension = width;
        }

        // Left and top indentation.
        viewDimensions.offset.x += viewDimensions.padding;
        viewDimensions.offset.y += viewDimensions.padding;

        if (!isMultiMode)
        {
            viewDimensions.offset.x += width * i;
        }

//        m_viewYOffset += m_viewPadding;
    }
}

//------------------------------------------------------------------------------

void ViewHelper::fillViewAreaUTM(DrawingLifeApp& app)
{
    const AppSettings& settings = app.getAppSettings();
    const size_t numPerson = settings.getNumPersons();
    const ofColor foregroundColor(settings.getColorViewboxR(),
                                  settings.getColorViewboxG(),
                                  settings.getColorViewboxB());
    const bool isMultiMode = settings.isMultiMode();

    const WalkVector& walks = app.getWalkVector();

    for (size_t i = 0; i < numPerson; ++i)
    {
        // Normalized value range from 0 to 1.
        const Walk& walk = walks[i];
        double x = walk.getScaledUtmX(0);
        double y = walk.getScaledUtmY(0);
        double w = walk.getScaledUtmX(1) - x;
        double h = walk.getScaledUtmY(1) - y;
        ofFill();
        ofSetColor(foregroundColor);
        ofRect(x, y, w, h);
        if (isMultiMode)
        {
            break;
        }

    }
}

//------------------------------------------------------------------------------

void ViewHelper::drawInfo(const AppSettings& settings,
                          const GpsData& gpsData,
                          const Walk& walk,
                          const ViewDimensions& viewDimensions,
                          const ofTrueTypeFont& font,
                          const size_t currentPerson)
{
    ofSetColor(255, 255, 255, settings.getAlphaLegend());
    ofSetHexColor(0xffffff);
    const std::string& infoText = Utils::getCurrentGpsInfo(gpsData, walk);
    const unsigned numPersons = settings.getNumPersons();
    const int infoX = viewDimensions.padding
            + (ofGetWidth() / numPersons) * currentPerson;
    const int infoY = viewDimensions.offset.y + 10;
    const_cast<ofTrueTypeFont&>(font).drawString(infoText, infoX, infoY);
}

//------------------------------------------------------------------------------

void ViewHelper::drawInfoDebug(const AppSettings& settings,
                               const MagicBox& magicBox,
                               const GpsData& gpsData,
                               const Walk& walk,
                               const size_t currentPerson)
{
    ofSetColor(255, 255, 255, settings.getAlphaLegend());

    const size_t numPersons = settings.getNumPersons();
    const int debugTextX = 30 + (ofGetWidth() / numPersons) * currentPerson;
    const int debugTextY = 30;
    ofDrawBitmapString(Utils::getCurrentGpsInfoDebug(gpsData, walk, magicBox),
                       debugTextX, debugTextY);
}

//------------------------------------------------------------------------------

void ViewHelper::drawInfoMultiMode(const AppSettings& settings,
                                   const Timeline& timeline,
                                   const ViewDimensions& viewDimensions,
                                   const ofTrueTypeFont& font)
{
    ofSetColor(255, 255, 255, settings.getAlphaLegend());
    ofSetHexColor(0xffffff);
    const std::string& infoText = timeline.getCurrentTime();
    const_cast<ofTrueTypeFont&>(font).drawString(infoText,
                                                 viewDimensions.padding,
                                                 viewDimensions.offset.y + 10);

}

//------------------------------------------------------------------------------
