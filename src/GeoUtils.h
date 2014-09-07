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

class GeoUtils
{
public:
    static UtmPoint LatLon2Utm(double lat, double lon);
    static ofxPoint<double> Utm2LatLon(double x, double y);
};

//------------------------------------------------------------------------------

#endif
