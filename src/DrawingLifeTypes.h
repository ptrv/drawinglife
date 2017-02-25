/*==========================================================
 Copyright (c) avp::ptr, 2010
 ==========================================================*/

#ifndef _DRAWINGLIFETYPES_H_
#define _DRAWINGLIFETYPES_H_

//------------------------------------------------------------------------------

#include <string>

struct ZoomAnimFrame
{
	float frameTime;
	int frameZoom;
	double frameCenterX;
	double frameCenterY;
	std::string timestamp;
	int gpsId;

};

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

struct GpsCoord
{
    double lat;
    double lon;
};

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

struct CurrentPointImageData
{
    std::string path;
    float width;
    float height;
    int alpha;
};

//------------------------------------------------------------------------------

struct GpsDataIndex
{
    GpsDataIndex(int p, int s, int n)
        : gpsPoint(p), gpsSegment(s), numPoints(n) {}
    int gpsPoint;
    int gpsSegment;
    int numPoints;
};

//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// templated version of openFramworks datatypes.
//------------------------------------------------------------------------------

#include "ofxPoint.h"
#include "ofxRectangle.h"

//------------------------------------------------------------------------------

struct ViewDimensions
{
    ofxPoint<double> offset;
    double minDimension;
    double padding;
};

//------------------------------------------------------------------------------

struct UtmPoint : public ofxPoint<double>
{
    UtmPoint() : ofxPoint<double>(){}
    UtmPoint(double utmX, double utmY) : ofxPoint<double>(utmX, utmY) {}
    double speed;
};

//------------------------------------------------------------------------------
// typedefs
//------------------------------------------------------------------------------

#include <ofImage.h>
#include <ofSoundPlayer.h>

#include <vector>
#include <boost/scoped_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

class DBReader;
class GpsData;
class Walk;
class MagicBox;
class GpsSegment;
class GpsPoint;
class LocationImage;
class Timeline;

typedef std::vector<std::string> StringVec;
typedef boost::scoped_ptr<DBReader> DBReaderPtr;
typedef std::vector<ZoomAnimFrame> ZoomAnimFrameVec;

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

typedef std::map<std::string, std::pair<std::string, int> > DrawingLifeFontsRaw;
typedef std::map<std::string, ofTrueTypeFont> DrawingLifeFonts;

typedef std::vector<ViewDimensions> ViewDimensionsVec;

typedef boost::shared_ptr<ofImage> ofImagePtr;
typedef std::vector<ofImagePtr> ofImagePtrVec;
typedef boost::weak_ptr<ofImage> ofImageWeak;
typedef boost::ptr_vector<LocationImage> LocationImageVec;

typedef boost::ptr_vector<ofSoundPlayer> ofSoundPlayerVec;

typedef boost::shared_ptr<Timeline> TimelinePtr;
typedef boost::weak_ptr<Timeline> TimelineWeak;
typedef std::vector<TimelineObject> TimelineObjectVec;

class  ofVec2f;
typedef std::vector<ofVec2f> tPoints;

struct ColorSlice
{
    ColorSlice(const ofColor& c, const int startIndex)
        : color(c), start(startIndex), total(1)
    {
        if (start > 0)
        {
            --start;
            ++total;
        }
    }
    ofColor color;
    int start;
    int total;
};

typedef std::vector<ColorSlice> tColorSlices;

struct PointsAndColors
{
    tPoints points;
    tColorSlices colors;

    void add(const ofVec2f& pt, const ofColor& c)
    {
        points.push_back(pt);

        if (colors.empty() || colors.back().color != c)
        {
            const int startIndex = static_cast<int>(points.size()) - 1;
            colors.push_back(ColorSlice(c, startIndex));
        }
        else
        {
            ++colors.back().total;
        }
    }
};

//---------------------------------------------------------------------------

#endif // _DRAWINGLIFETYPES_H_
