#include "DrawingLifeIncludes.h"

#include "GeoUtils.h"

#include <proj_api.h>

//------------------------------------------------------------------------------

bool projInitialized = false;
projPJ pjMerc, pjLonLat;

static const char* projMercStr =
    "+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m +nadgrids=@null +wktext  +no_defs";

static const char* projLonLatStr =
    "+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs";

//------------------------------------------------------------------------------

void initProj()
{
    if (!projInitialized)
    {
        pjMerc = pj_init_plus(projMercStr);
        pjLonLat = pj_init_plus(projLonLatStr);
        projInitialized = pjMerc && pjLonLat;
        if (projInitialized)
        {
            ofLogVerbose() << "Proj4 version: " << PJ_VERSION;
        }
        else
        {
            ofLogError() << "Could not initialize Proj4!";
            ofExit(1);
        }
    }
}

//------------------------------------------------------------------------------

void transformPoint(projPJ& src, projPJ& dst, double& x, double& y)
{
    const int pjErrno = pj_transform(src, dst, 1, 1, &x, &y, NULL);

    if (pjErrno != 0)
    {
        ofLogError() << "Proj4 transform error: " << pj_strerrno(pjErrno);
        x = y = 0.0;
    }
}

//------------------------------------------------------------------------------

UtmPoint GeoUtils::LonLat2Utm(const double lon, const double lat)
{
    initProj();
    double x = lon * DEG_TO_RAD;
    double y = lat * DEG_TO_RAD;
    transformPoint(pjLonLat, pjMerc, x, y);
    return UtmPoint(x, y);
}

//------------------------------------------------------------------------------

ofxPoint<double> GeoUtils::Utm2LonLat(const double x, const double y)
{
    initProj();
    double lon = x;
    double lat = y;
    transformPoint(pjMerc, pjLonLat, lon, lat);
    return ofxPoint<double>(lon * RAD_TO_DEG, lat * RAD_TO_DEG);
}

//------------------------------------------------------------------------------
