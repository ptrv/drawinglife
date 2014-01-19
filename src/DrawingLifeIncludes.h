/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#ifndef _DRAWINGLIFEINCLUDES_H_
#define _DRAWINGLIFEINCLUDES_H_

// -----------------------------------------------------------------------------

#define APP_NAME_STR "DrawingLife"
#define APP_AUTHOR_STR "avp::ptr"
#define APP_DATE "2013"
#define APP_VERSION_STR "5.0.0dev"

// -----------------------------------------------------------------------------
// boost
// -----------------------------------------------------------------------------

#include <boost/scoped_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/make_shared.hpp>

// -----------------------------------------------------------------------------
// include openFrameworks source tree.
// -----------------------------------------------------------------------------

#include "ofMain.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#include "Logger.h"

// -----------------------------------------------------------------------------

#include "DrawingLifeTypes.h"

// -----------------------------------------------------------------------------

#include "AppSettings.h"

// -----------------------------------------------------------------------------

#include "Utils.h"

// -----------------------------------------------------------------------------

namespace AppLogTag
{
static const char* APP          = "DrawingLifeApp";
static const char* DB_READER    = "DBReader";
static const char* SETTINGS     = "AppSettings";
static const char* GPS_DATA     = "GpsData";
static const char* WALK         = "Walk";
static const char* MAGIC_BOX    = "MagicBox";
static const char* DATA_LOADER  = "DataLoader";
};


// -----------------------------------------------------------------------------

#endif // _DRAWINGLIFEINCLUDES_H_
