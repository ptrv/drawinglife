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

namespace GeographicLib { class TransverseMercatorExact; };

//------------------------------------------------------------------------------

class GeoUtils
{
public:
    static UtmPoint LatLon2Utm(double meridian, double lat, double lon);
    static ofxPoint<double> Utm2LatLon(double meridian, double x, double y);

private:
    static const GeographicLib::TransverseMercatorExact& getTransverseMercatorExact();
};

//------------------------------------------------------------------------------

#endif
