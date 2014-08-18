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

#if GEOGRAPHICLIB_VERSION >= 1003700
#define TRANSVERSE_MERCATOR() TransverseMercatorExact::UTM()
#else
#define TRANSVERSE_MERCATOR() TransverseMercatorExact::UTM
#endif

//------------------------------------------------------------------------------

#endif
