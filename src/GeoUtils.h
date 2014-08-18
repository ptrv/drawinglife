//
//  GeoUtils.h
//  DrawingLife
//
//  Created by Peter Vasil on 18/08/14.
//
//

#ifndef DrawingLife_GeoUtils_h
#define DrawingLife_GeoUtils_h

//------------------------------------------------------------------------------

#include "GeographicLib/TransverseMercatorExact.hpp"

//------------------------------------------------------------------------------

using namespace GeographicLib;

//------------------------------------------------------------------------------

class GeoUtils
{
public:
    static UtmPoint LatLon2Utm(double meridian, double lat, double lon)
    {
        const TransverseMercatorExact& TMS = getTransverseMercatorExact();
        Math::real x, y, gamma, k;
        TMS.Forward(Math::real(meridian), lat, lon, x, y, gamma, k);
        return UtmPoint(x, y);
    }

    static ofxPoint<double> Utm2LatLon(double meridian, double x, double y)
    {
        const TransverseMercatorExact& TMS = getTransverseMercatorExact();
        Math::real lat , lon, gamma, k;
        TMS.Reverse(Math::real(meridian), x, y, lat, lon, gamma, k);
        return ofxPoint<double>(lon, lat);
    }

private:
    static const TransverseMercatorExact& getTransverseMercatorExact()
    {
#if GEOGRAPHICLIB_VERSION >= 1003700
        return TransverseMercatorExact::UTM();
#else
        return TransverseMercatorExact::UTM;
#endif
    }
};

//------------------------------------------------------------------------------

#endif
