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
    static UtmPoint LonLat2Utm(double lon, double lat);
    static ofxPoint<double> Utm2LonLat(double x, double y);
};

//------------------------------------------------------------------------------

#endif
