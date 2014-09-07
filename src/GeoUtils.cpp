#include "DrawingLifeIncludes.h"

#include "GeoUtils.h"

#include <proj_api.h>

//------------------------------------------------------------------------------

bool projInitialized = false;
projPJ pjMerc, pjLatLon;

static const char* projMercStr =
    "+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m +nadgrids=@null +wktext  +no_defs";

static const char* projLatLonStr =
    "+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs";
//------------------------------------------------------------------------------

void transformPoint(projPJ& src, projPJ& dst, double& x, double& y)
{
    if (!projInitialized)
    {
        pjMerc = pj_init_plus(projMercStr);
        pjLatLon = pj_init_plus(projLatLonStr);
        projInitialized = pjMerc && pjLatLon;
        if (!projInitialized)
        {
            ofLogError() << "Could not initialize Proj4!";
            ofExit(1);
        }
    }

    if (pj_transform(src, dst, 1, 1, &x, &y, NULL) != 0)
    {
        ofLogError() << "Proj4 transform failed!";
        x = y = 0.0;
    }
}

//------------------------------------------------------------------------------

UtmPoint GeoUtils::LatLon2Utm(const double lat, const double lon)
{
    double x = lon * DEG_TO_RAD;
    double y = lat * DEG_TO_RAD;
    transformPoint(pjLatLon, pjMerc, x, y);
    return UtmPoint(x, y);
}

//------------------------------------------------------------------------------

ofxPoint<double> GeoUtils::Utm2LatLon(const double x, const double y)
{
    double lon = x;
    double lat = y;
    transformPoint(pjMerc, pjLatLon, lon, lat);
    return ofxPoint<double>(lon * RAD_TO_DEG, lat * RAD_TO_DEG);
}

//------------------------------------------------------------------------------
