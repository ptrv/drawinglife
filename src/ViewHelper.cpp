#include "ViewHelper.h"
#include "DrawingLifeIncludes.h"
#include "Utils.h"
#include "DrawingLifeApp.h"

ViewHelper::ViewHelper()
{
}

void ViewHelper::drawFPS()
{
    float fps = Utils::getFPS();
    ofSetHexColor(0xffffff);
    std::string str = "FPS: "+ofToString(static_cast<double>(fps), 1);
    ofDrawBitmapString(str, 30.0, ofGetHeight()-30 );

}

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

void ViewHelper::drawStartScreen(ofTrueTypeFont& fontTitle,
                                 ofTrueTypeFont& fontAuthor)
{
    ofSetColor(255,255,255);

    std::string title = APP_NAME_STR;
    title += " ";
    title += APP_VERSION_STR;
    fontTitle.drawString(title, ofGetWidth()/2 - 365, ofGetHeight()/2 - 100);

    fontAuthor.drawString(APP_AUTHOR_STR, ofGetWidth()/2 - 91, ofGetHeight()/2);
    fontAuthor.drawString("plan b", ofGetWidth()/2 - 60, ofGetHeight()/2 + 60);
}

void ViewHelper::setViewAspectRatio(DrawingLifeApp& app)
{
    const AppSettings& settings = app.getAppSettings();
    const bool isMultiMode = settings.isMultiMode();
    const size_t numPerson = settings.getNumPerson();

    ViewAspectRatioData& viewAspectRatioData = app.getViewAspectRatioData();
    double width;
    double height;

    if(isMultiMode)
    {
        width = ofGetWidth();
    }
    else
    {
        width = ofGetWidth() / static_cast<int>(numPerson);
    }

    height = ofGetHeight();

    for(size_t i = 0; i < numPerson; ++i)
    {
        // Reset for view padding.
        viewAspectRatioData.offsetX[i] = 0;
        viewAspectRatioData.offsetY[i] = 0;

//        if(false)
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
            viewAspectRatioData.minDimension[i] = height;
            viewAspectRatioData.offsetX[i] = (width - height) / 2.0;
        }
        else if (width < height)
        {
            viewAspectRatioData.minDimension[i] = width;
            viewAspectRatioData.offsetY[i] = (height - width) / 2.0;
        }
        else
        {
            viewAspectRatioData.minDimension[i] = width;
        }

        // Left and top indentation.
        viewAspectRatioData.offsetX[i] += viewAspectRatioData.padding[i];
        viewAspectRatioData.offsetY[i] += viewAspectRatioData.padding[i];

        if(!isMultiMode)
        {
            viewAspectRatioData.offsetX[i] += width * i;
        }

//        m_viewYOffset += m_viewPadding;
    }
}

void ViewHelper::fillViewAreaUTM(DrawingLifeApp& app)
{
    const AppSettings& settings = app.getAppSettings();
    const size_t numPerson = settings.getNumPerson();
    const ofColor foregroundColor(settings.getColorViewboxR(),
                                  settings.getColorViewboxG(),
                                  settings.getColorViewboxB());
    const bool isMultiMode = settings.isMultiMode();

    const WalkVector& walks = app.getWalkVector();

    for(size_t i = 0; i < numPerson; ++i)
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
        if(isMultiMode)
        {
            break;
        }

    }
}
