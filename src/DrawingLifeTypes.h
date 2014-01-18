/*==========================================================
 Copyright (c) avp::ptr, 2010
 ==========================================================*/

#ifndef _DRAWINGLIFETYPES_H_
#define _DRAWINGLIFETYPES_H_


struct ZoomAnimFrame
{
	float frameTime;
	int frameZoom;
	double frameCenterX;
	double frameCenterY;
	std::string timestamp;
	int gpsId;

};
/**
 * \brief Timeline value with time and id.
 *
 */
struct TimelineObject
{
    std::string timeString;
    time_t secs;
    int id;
    int gpsid;
    bool operator() (TimelineObject lhs, TimelineObject rhs)
    {
        return lhs.secs < rhs.secs;
    }
};

/**
 * \brief Data for the database query
 *
 */
struct DBQueryData
{
    int type;
    int dayStart;
    int dayEnt;
    int monthStart;
    int monthEnd;
    int yearStart;
    int yearEnd;
    std::string city;
};


/**
 * \brief Represents latitude and longitude value of an Gps coordinate.
 *
 */
struct GpsCoord
{
    double lat;
    double lon;
};

struct LocationImageData
{
    std::string name;
    std::string path;
    GpsCoord gps;
    float width;
    float height;
    int alpha;
    int anchorType;
    float anchorX;
    float anchorY;
    bool anchorShow;
};

struct CurrentImageData
{
    std::string path;
    float width;
    float height;
    int alpha;
};

struct GpsDataIndex
{
    int gpsPoint;
    int gpsSegment;
    int point;
};

struct GpsRegion
{
    double lon0;
    double minLon;
    double maxLon;
};

// -----------------------------------------------------------------------------
// templated version of openFramworks datatypes.
// -----------------------------------------------------------------------------

#include "ofxPoint.h"
#include "ofxRectangle.h"

//---------------------------------------------------------------------------

struct ViewDimensions
{
    ofxPoint<double> offset;
    double minDimension;
    double padding;
};

/**
 * \brief Represents x and y value of an UTM coordinate.
 *
 */
struct UtmPoint : public ofxPoint<double>
{
    UtmPoint() : ofxPoint<double>(){}
    UtmPoint(double utmX, double utmY) : ofxPoint<double>(utmX, utmY) {}
    double speed;
    double lon0;
};

//---------------------------------------------------------------------------
// typedefs
//---------------------------------------------------------------------------

class DBReader;
class GpsData;
class Walk;
class MagicBox;
class GpsSegment;
class GpsPoint;
class LocationImage;

typedef std::vector<std::string> StringVec;
typedef boost::scoped_ptr<DBReader> DBReaderPtr;
typedef std::vector<ZoomAnimFrame> ZoomAnimFrames;

typedef boost::shared_ptr<GpsData> GpsDataPtr;
typedef boost::weak_ptr<GpsData> GpsDataWeak;
typedef std::vector<GpsDataPtr> GpsDataVector;

typedef std::vector<GpsDataIndex> GpsDataIndexVector;

typedef boost::ptr_vector<Walk> WalkVector;

typedef boost::shared_ptr<MagicBox> MagicBoxPtr;
typedef boost::weak_ptr<MagicBox> MagicBoxWeak;
typedef std::vector<MagicBoxPtr> MagicBoxVector;

typedef std::vector<UtmPoint> UtmSegment;
typedef std::vector<std::vector<UtmPoint> > UtmDataVector;
typedef std::vector<GpsSegment> GpsSegmentVector;
typedef std::vector<GpsPoint> GpsPointVector;

typedef std::map<std::string, std::pair<std::string, int> > DrawingLifeFontMap;
typedef std::map<std::string, ofTrueTypeFont> DrawingLifeFonts;

typedef std::vector<ViewDimensions> ViewDimensionsVec;

typedef boost::shared_ptr<ofImage> ofImagePtr;
typedef std::vector<ofImagePtr> ofImagePtrVec;
typedef boost::weak_ptr<ofImage> ofImageWeak;
typedef boost::ptr_vector<LocationImage> LocationImageVec;


//---------------------------------------------------------------------------

#endif // _DRAWINGLIFETYPES_H_
