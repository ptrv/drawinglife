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

//------------------------------------------------------------------------------

void Utils::calculateGlobalMinMaxValues(DrawingLifeApp& app)
{
    ofxPoint<double> minXY = getPointDoubleMax();
    ofxPoint<double> maxXY = getPointDoubleMin();
    ofxPoint<double> minLonLat = getPointDoubleMax();
    ofxPoint<double> maxLonLat = getPointDoubleMin();

    const GpsDataVector& gpsDatas = app.getGpsDataVector();
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

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

static unsigned currentFrame = 0;
void Utils::grabScreen()
{
    static char fileNameStr[255];
    sprintf(fileNameStr, "output/output_%.4u.png", currentFrame);
    ofSaveScreen(fileNameStr);
    ++currentFrame;
}

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

const std::string Utils::getCurrentGpsInfo(const GpsData *gpsData,
                                           const Walk *walk)
{
    std::string gpsInfo;
    if (walk && gpsData && gpsData->getTotalGpsPoints() > 0)
    {
        std::string timeString = walk->getCurrentTimestamp();
        int year, month, day, hour, min, sec;
//        sscanf(timeString.c_str(), "%d-%d-%dT%d:%d:%dZ",
//               &year, &month, &day, &hour, &min, &sec);
        sscanf(timeString.c_str(), "%d-%d-%d %d:%d:%d",
               &year, &month, &day, &hour, &min, &sec);
        char buf[25];
        sprintf(buf, "%02d.%02d.%d %02d:%02d:%02d",
                day, month, year, hour, min, sec);
        gpsInfo = walk->getGpsLocationCurrent() + " " + string(buf);
    }
    return gpsInfo;
}

const std::string Utils::getCurrentGpsInfoDebug(const GpsData *gpsData,
                                                const Walk *walk,
                                                const MagicBox *box)
{
    std::string gpsInfoDebug;
    if (gpsData && walk && box)
    {
        GpsPoint boxCenter = box->getCenterGps();
        gpsInfoDebug += "Longitude         : " + ofToString(walk->getCurrentLongitude(), 7) + "\n" +
        "Latitude          : " + ofToString(walk->getCurrentLatitude(), 7) + "\n" +
        "Elevation         : " + ofToString(walk->getCurrentElevation(), 7) + "\n" +
        "UTM X             : " + ofToString(walk->getCurrentUtmX(), 7) + "\n" +
        "UTM Y             : " + ofToString(walk->getCurrentUtmY(), 7) + "\n" +
        "Time              : " + walk->getCurrentTimestamp() + "\n" +
        "Location          : " + walk->getGpsLocationCurrent() + "\n" +
        "Central meridian  : " + ofToString(gpsData->getLon0(), 7) + "\n" +
        "Meridian global   : " + ofToString(GpsData::getLon0Glogal(), 7) + "\n" +
        "Min/Max latitude  : " + ofToString(gpsData->getMinLat(), 7) + " / " + ofToString(gpsData->getMaxLat(), 7) + "\n" +
        "Min/Max longitude : " + ofToString(gpsData->getMinLon(), 7) + " / " + ofToString(gpsData->getMaxLon(), 7) + "\n" +
        "Min/Max UTM X     : " + ofToString(gpsData->getMinUtmX(), 7) + " / " + ofToString(gpsData->getMaxUtmX(), 7) + "\n" +
        "Min/Max UTM Y     : " + ofToString(gpsData->getMinUtmY(), 7) + " / " + ofToString(gpsData->getMaxUtmY(), 7) + "\n" +
        "Currrent pt.      : " + ofToString(walk->getCurrentPointNum()) + "\n" +
        "Segment nr.       : " + ofToString(walk->getCurrentSegmentNum()) + "\n" +
        "Total pts.        : " + ofToString(gpsData->getTotalGpsPoints()) + "\n" +
    //	"Viewbox center    : " + ofToString(m_magicBox->getCenter().x,7) + " / " + ofToString(m_magicBox->getCenter().y, 7) + "\n" +
        "Viewbox center    : " + ofToString(boxCenter.getLatitude(),7) + " / " + ofToString(boxCenter.getLongitude(), 7) + "\n" +
        "Viewbox size      : " + ofToString(box->getSize(),7) + "\n" +
        "Person            : " + gpsData->getUser();
    }
    return gpsInfoDebug;
}

//------------------------------------------------------------------------------
