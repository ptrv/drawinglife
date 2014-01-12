#include "Utils.h"

#include "DrawingLifeIncludes.h"
#include "DrawingLifeApp.h"
#include "GpsData.h"

#if defined (WIN32)
#undef max
#undef min
#endif
#include <limits>

Utils::Utils()
{
}

void Utils::calculateGlobalMinMaxValues(DrawingLifeApp& app)
{
    ofxPoint<double> minXY = getPointDoubleMax();
    ofxPoint<double> maxXY = getPointDoubleMin();
    ofxPoint<double> minLonLat = getPointDoubleMax();
    ofxPoint<double> maxLonLat = getPointDoubleMin();

    GpsDataVector& gpsDatas = app.getGpsDataVector();
    const AppSettings& settings = app.getAppSettings();

    BOOST_FOREACH(const GpsDataPtr gpsData, gpsDatas)
    {
        if (gpsData->getMinUtmX() < minXY.x) minXY.x = gpsData->getMinUtmX();
        if (gpsData->getMaxUtmX() > maxXY.x) maxXY.x = gpsData->getMaxUtmX();
        if (gpsData->getMinUtmY() < minXY.y) minXY.y = gpsData->getMinUtmY();
        if (gpsData->getMaxUtmY() > maxXY.y) maxXY.y = gpsData->getMaxUtmY();
        if (gpsData->getMinLon() < minLonLat.x) minLonLat.x = gpsData->getMinLon();
        if (gpsData->getMaxLon() > maxLonLat.x) maxLonLat.x = gpsData->getMaxLon();
        if (gpsData->getMinLat() < minLonLat.y) minLonLat.y = gpsData->getMinLon();
        if (gpsData->getMaxLat() > maxLonLat.y) maxLonLat.y = gpsData->getMaxLon();
    }

    bool isRegionOn = settings.isRegionsOn();
    if (isRegionOn)
    {
        GpsData::setGlobalValues(minXY, maxXY, 0.0);
    }
    else
    {
        if (settings.isMeridianAuto())
            GpsData::setGlobalValues(minXY, maxXY,
                                     minLonLat.x + (maxLonLat.x - minLonLat.x) / 2);
        else
        {
            double lon0 = 0.0;
            lon0 = settings.getMeridian();
            GpsData::setGlobalValues(minXY, maxXY, lon0);
        }
    }

    BOOST_FOREACH(const GpsDataPtr gpsData, gpsDatas)
    {
        gpsData->calculateUtmPointsGlobalLon(isRegionOn);
        gpsData->normalizeUtmPointsGlobal();
    }
}

static double timeThen = 0.0;
static double timeSum = 0.0;
static float fpsToShow = 0.0;
float Utils::getFPS()
{
    float timeNow = ofGetElapsedTimef();
    double diff = timeNow - timeThen;
    timeThen = timeNow;

    timeSum += diff;
    if (timeSum > 0.5)
    {
        fpsToShow = 1.0/diff;
        timeSum = 0.0;
    }
    return fpsToShow;
}

static unsigned currentFrame = 0;
void Utils::grabScreen()
{
    static char fileNameStr[255];
    sprintf(fileNameStr, "output/output_%.4u.png", currentFrame);
    ofSaveScreen(fileNameStr);
    ++currentFrame;
}

ofxPoint<double> Utils::getPointDoubleMin()
{
    return ofxPoint<double>(-std::numeric_limits<double>::max(),
                            -std::numeric_limits<double>::max());
}

ofxPoint<double> Utils::getPointDoubleMax()
{
    return ofxPoint<double>(std::numeric_limits<double>::max(),
                            std::numeric_limits<double>::max());
}
