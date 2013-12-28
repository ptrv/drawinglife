#include "Utils.h"

#include "DrawingLifeIncludes.h"
#include "DrawingLifeApp.h"
#include "GpsData.h"

Utils::Utils()
{
}

void Utils::calculateGlobalMinMaxValues(DrawingLifeApp& app)
{
    double minX = numeric_limits<double>::max();
    double maxX = -numeric_limits<double>::max();
    double minY = numeric_limits<double>::max();
    double maxY = -numeric_limits<double>::max();
    double minLon = numeric_limits<double>::max();
    double maxLon = -numeric_limits<double>::max();
    double minLat = numeric_limits<double>::max();
    double maxLat = -numeric_limits<double>::max();

    GpsDataVector& gpsDatas = app.getGpsDataVector();
    const AppSettings& settings = app.getAppSettings();

    BOOST_FOREACH(const GpsData& gpsData, gpsDatas)
    {
        if (gpsData.getMinUtmX() < minX) minX = gpsData.getMinUtmX();
        if (gpsData.getMaxUtmX() > maxX) maxX = gpsData.getMaxUtmX();
        if (gpsData.getMinUtmY() < minY) minY = gpsData.getMinUtmY();
        if (gpsData.getMaxUtmY() > maxY) maxY = gpsData.getMaxUtmY();
        if (gpsData.getMinLon() < minLon) minLon = gpsData.getMinLon();
        if (gpsData.getMaxLon() > maxLon) maxLon = gpsData.getMaxLon();
        if (gpsData.getMinLat() < minLat) minLat = gpsData.getMinLon();
        if (gpsData.getMaxLat() > maxLat) maxLat = gpsData.getMaxLon();
    }

    bool isRegionOn = settings.isRegionsOn();
    if(isRegionOn)
    {
        GpsData::setGlobalValues(minX, maxX, minY, maxY, 0.0);
    }
    else
    {
        if(settings.isMeridianAuto())
            GpsData::setGlobalValues(minX, maxX, minY, maxY,
                                     (minLon + (maxLon - minLon)/2));
        else
        {
            double lon0 = 0.0;
            lon0 = settings.getMeridian();
            GpsData::setGlobalValues(minX, maxX, minY, maxY, lon0);
        }
    }

    BOOST_FOREACH(GpsData& gpsData, gpsDatas)
    {
        gpsData.calculateUtmPointsGlobalLon(isRegionOn);
        gpsData.normalizeUtmPointsGlobal();
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
