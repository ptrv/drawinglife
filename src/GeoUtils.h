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
    static const TransverseMercatorExact& getTransversMercatorExact()
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
