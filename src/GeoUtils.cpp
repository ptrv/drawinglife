#include "DrawingLifeIncludes.h"

#include "GeoUtils.h"

#include "GeographicLib/TransverseMercator.hpp"

//------------------------------------------------------------------------------

using namespace GeographicLib;

//------------------------------------------------------------------------------

UtmPoint GeoUtils::LatLon2Utm(const double meridian,
                              const double lat, const double lon)
{
    const TransverseMercator& TMS = getTransverseMercator();
    Math::real x, y, gamma, k;
    TMS.Forward(Math::real(meridian), lat, lon, x, y, gamma, k);
    return UtmPoint(x, y);
}

ofxPoint<double> GeoUtils::Utm2LatLon(const double meridian,
                                      const double x, const double y)
{
    const TransverseMercator& TMS = getTransverseMercator();
    Math::real lat, lon, gamma, k;
    TMS.Reverse(Math::real(meridian), x, y, lat, lon, gamma, k);
    return ofxPoint<double>(lon, lat);
}

const TransverseMercator& GeoUtils::getTransverseMercator()
{
#if GEOGRAPHICLIB_VERSION >= 1003700
    return TransverseMercator::UTM();
#else
    return TransverseMercator::UTM;
#endif
}
