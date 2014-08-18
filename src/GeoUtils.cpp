#include "DrawingLifeIncludes.h"

#include "GeoUtils.h"

#include "GeographicLib/TransverseMercatorExact.hpp"

//------------------------------------------------------------------------------

using namespace GeographicLib;

//------------------------------------------------------------------------------

UtmPoint GeoUtils::LatLon2Utm(double meridian, double lat, double lon)
{
    const TransverseMercatorExact& TMS = getTransverseMercatorExact();
    Math::real x, y, gamma, k;
    TMS.Forward(Math::real(meridian), lat, lon, x, y, gamma, k);
    return UtmPoint(x, y);
}

ofxPoint<double> GeoUtils::Utm2LatLon(double meridian, double x, double y)
{
    const TransverseMercatorExact& TMS = getTransverseMercatorExact();
    Math::real lat , lon, gamma, k;
    TMS.Reverse(Math::real(meridian), x, y, lat, lon, gamma, k);
    return ofxPoint<double>(lon, lat);
}

const TransverseMercatorExact& GeoUtils::getTransverseMercatorExact()
{
#if GEOGRAPHICLIB_VERSION >= 1003700
    return TransverseMercatorExact::UTM();
#else
    return TransverseMercatorExact::UTM;
#endif
}
