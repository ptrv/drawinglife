/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#include "DrawingLifeIncludes.h"
#include "GpsData.h"
#include "GeoUtils.h"

//------------------------------------------------------------------------------

GpsData::GpsData(const AppSettings& settings)
:
m_settings(settings),
m_gpsDataId(0),
m_user(""),
m_minLonLat(0.0, 0.0),
m_maxLonLat(0.0, 0.0),
m_minUtm(0.0, 0.0),
m_maxUtm(0.0, 0.0)
{
	m_segments.reserve(1000); // TODO good amount.

    fnGetLongitude = boost::bind(&GpsPoint::getLongitude, _1);
    fnGetLatitude = boost::bind(&GpsPoint::getLatitude, _1);
    fnGetElevation = boost::bind(&GpsPoint::getElevation, _1);

    fnGetUtmX = boost::bind(&UtmPoint::x, _1);
    fnGetUtmY = boost::bind(&UtmPoint::y, _1);
}

//------------------------------------------------------------------------------

GpsData::~GpsData()
{
    ofLogVerbose(Logger::GPS_DATA, "destroying");
}

//------------------------------------------------------------------------------
// Setters
//------------------------------------------------------------------------------

void GpsData::setGpsData(const GpsSegmentVector& segments,
                         const ofxPoint<double>& minLonLat,
                         const ofxPoint<double>& maxLonLat,
                         const std::string& user)
{
	++m_gpsDataId;
	m_segments.clear();
	m_segments = segments;
    m_minLonLat = minLonLat;
    m_maxLonLat = maxLonLat;
    // -------------------------------------------------------------------------
    m_minUtm = GeoUtils::LonLat2Utm(m_minLonLat.x, m_minLonLat.y);
    m_maxUtm = GeoUtils::LonLat2Utm(m_maxLonLat.x, m_maxLonLat.y);
    m_user = user;
    calculateUtmPointsWithIndex();
    normalizeUtmPoints();
}

//------------------------------------------------------------------------------

void GpsData::clear()
{
	m_gpsDataId = 0;
	m_segments.clear();
    m_minLonLat = ofxPoint<double>(0.0, 0.0);
    m_maxLonLat = ofxPoint<double>(0.0, 0.0);
    m_minUtm = ofxPoint<double>(0.0, 0.0);
    m_maxUtm = ofxPoint<double>(0.0, 0.0);
	m_user = "";
}

//------------------------------------------------------------------------------
// Getters
//------------------------------------------------------------------------------

double GpsData::getData(const size_t segmentIndex, const size_t pointIndex,
                        const tFnGetGpsData& fnGetGpsData) const
{
    try
    {
        const GpsPointVector& points = m_segments.at(segmentIndex).getPoints();
        const GpsPoint point = points.at(pointIndex);
        return fnGetGpsData(point);
    }
    catch (const std::out_of_range&)
    {
        return -1000.0;
    }
}

//------------------------------------------------------------------------------

double GpsData::getLongitude(const size_t segmentIndex,
                             const size_t pointIndex) const
{
    return getData(segmentIndex, pointIndex, fnGetLongitude);
}

//------------------------------------------------------------------------------

double GpsData::getLatitude(const size_t segmentIndex,
                            const size_t pointIndex) const
{
    return getData(segmentIndex, pointIndex, fnGetLatitude);
}

//------------------------------------------------------------------------------

double GpsData::getElevation(const size_t segmentIndex,
                             const size_t pointIndex) const
{
    return getData(segmentIndex, pointIndex, fnGetElevation);
}

//------------------------------------------------------------------------------

double GpsData::getUtmData(size_t segmentIndex, size_t pointIndex,
                           const UtmDataVector& utmVec,
                           const tFnGetUtmData& fnGetUtmData) const
{
    try
    {
        const UtmSegment& utmSegment = utmVec.at(segmentIndex);
        const UtmPoint&  utmPoint = utmSegment.at(pointIndex);
        return fnGetUtmData(utmPoint);
    }
    catch (const std::out_of_range&)
    {
        return 0.0;
    }
}

//------------------------------------------------------------------------------

double GpsData::getUtmX(const size_t segmentIndex,
                        const size_t pointIndex) const
{
    return getUtmData(segmentIndex, pointIndex, m_utmPoints, fnGetUtmX);
}

//------------------------------------------------------------------------------

double GpsData::getUtmY(const size_t segmentIndex,
                        const size_t pointIndex) const
{
    return getUtmData(segmentIndex, pointIndex, m_utmPoints, fnGetUtmY);
}

//------------------------------------------------------------------------------

UtmPoint GpsData::getUtm(const size_t segmentIndex,
                         const size_t pointIndex) const
{
    try
    {
        const UtmSegment& utmSegment = m_utmPoints.at(segmentIndex);
        return utmSegment.at(pointIndex);
    }
    catch (const std::out_of_range&)
    {
        return UtmPoint();
    }
}

//------------------------------------------------------------------------------

double GpsData::getNormalizedUtmX(const size_t segmentIndex,
                                  const size_t pointIndex) const
{
    return getUtmData(segmentIndex, pointIndex, m_normalizedUtmPoints, fnGetUtmX);
}

//------------------------------------------------------------------------------

double GpsData::getNormalizedUtmY(const size_t segmentIndex,
                                  const size_t pointIndex) const
{
    return getUtmData(segmentIndex, pointIndex, m_normalizedUtmPoints, fnGetUtmY);
}

//------------------------------------------------------------------------------

UtmPoint GpsData::getNormalizedUtm(const size_t segmentIndex,
                                   const size_t pointIndex) const
{
    try
    {
        const UtmSegment& normUtmSeg = m_normalizedUtmPoints.at(segmentIndex);
        return normUtmSeg.at(pointIndex);
    }
    catch (const std::out_of_range&)
    {
        return UtmPoint();
    }
}

//------------------------------------------------------------------------------

const std::string GpsData::getGpsLocation(const size_t segmentIndex,
                                          const size_t pointIndex) const
{
    try
    {
        const GpsPointVector& points = m_segments.at(segmentIndex).getPoints();
        return points.at(pointIndex).getLocation();
    }
    catch (const std::out_of_range&)
    {
        return std::string();
    }
}

//------------------------------------------------------------------------------

int GpsData::getTotalGpsPoints() const
{
	int num = 0;
    BOOST_FOREACH(const GpsSegment& rSegment, m_segments)
    {
        num += rSegment.getPoints().size();
    }
	return num;
}

//------------------------------------------------------------------------------

GpsPoint GpsData::getGpsPoint(const ofxPoint<double>& utmP)
{
    GpsPoint p;
    ofxPoint<double> latLon = GeoUtils::Utm2LonLat(utmP.x, utmP.y);
    p.setDataFromLatLon(latLon.y, latLon.x);

    return p;
}

//------------------------------------------------------------------------------

void GpsData::calculateUtmPointsWithIndex()
{
    m_indices.clear();
    int counter = 0;
    m_utmPoints.clear();
    m_utmPoints.reserve(m_segments.size());

    int i = 0;
    BOOST_FOREACH(const GpsSegment& segment, m_segments)
    {
        int j = 0;
        std::vector<UtmPoint> utmVec;
        utmVec.reserve(segment.getPoints().size());
        BOOST_FOREACH(const GpsPoint& point, segment.getPoints())
        {
            UtmPoint utmP = GeoUtils::LonLat2Utm(point.getLongitude(),
                                                 point.getLatitude());
            utmP.speed = point.getSpeed();

            utmVec.push_back(utmP);

            GpsDataIndex idx;
            idx.point = counter;
            idx.gpsPoint = j++;
            idx.gpsSegment = i;
            m_indices.push_back(idx);
            ++counter;
        }
        m_utmPoints.push_back(utmVec);
        ++i;
    }
}

//------------------------------------------------------------------------------
// Normalize Utm points.
//------------------------------------------------------------------------------

void GpsData::normalizeUtmPoints()
{
    setMinMaxRatioUTM();

    m_normalizedUtmPoints.clear();
    BOOST_FOREACH(const UtmSegment& utmSegment, m_utmPoints)
    {
        UtmSegment normalizedSegments;
        BOOST_FOREACH(const UtmPoint& utmPoint, utmSegment)
        {
            double x = utmPoint.x;
            double y = utmPoint.y;
            x = (x - m_minUtm.x) / (m_maxUtm.x - m_minUtm.x);
            y = (y - m_minUtm.y) / (m_maxUtm.y - m_minUtm.y);

            normalizedSegments.push_back(UtmPoint(x, y));
        }
        m_normalizedUtmPoints.push_back(normalizedSegments);
    }
}

//------------------------------------------------------------------------------
// Set min/max aspect ratio with UTM values.
//------------------------------------------------------------------------------
void GpsData::setMinMaxRatioUTM()
{
    setMinMaxValuesUTM();

    const double minLon = this->getMinUtmX();
    const double maxLon = this->getMaxUtmX();
    const double minLat = this->getMinUtmY();
    const double maxLat = this->getMaxUtmY();

    // Calculate horizontal and vertical range.
    const double deltaLon = maxLon - minLon;
    const double deltaLat = maxLat - minLat;

    const double deltaLatLonHalf = (deltaLat - deltaLon) / 2.0;
    const double deltaLonLatHalf = (deltaLon - deltaLat) / 2.0;

    // Aspect ratio is: width < height.
    if (deltaLon < deltaLat)
    {
        m_minUtm.x = minLon - deltaLatLonHalf;
        m_maxUtm.x = maxLon + deltaLatLonHalf;
        m_minUtm.y = minLat;
        m_maxUtm.y = maxLat;
    }
    // Aspect ratio is: width > height.
    else if (deltaLon > deltaLat)
    {
        m_minUtm.x = minLon;
        m_maxUtm.x = maxLon;
        m_minUtm.y = minLat - deltaLonLatHalf;
        m_maxUtm.y = maxLat + deltaLonLatHalf;
    }
    // Aspect ratio is: height == width.
    else
    {
        m_minUtm.x = minLon;
        m_maxUtm.x = maxLon;
        m_minUtm.y = minLat;
        m_maxUtm.y = maxLat;
    }
}

//------------------------------------------------------------------------------

void GpsData::setMinMaxValuesUTM()
{
    ofxPoint<double> minXY = Utils::getPointDoubleMax();
    ofxPoint<double> maxXY = Utils::getPointDoubleMin();

    BOOST_FOREACH(const UtmSegment& segments, m_utmPoints)
    {
        BOOST_FOREACH(const UtmPoint& utmPoint, segments)
        {
            const double x = utmPoint.x;
            const double y = utmPoint.y;
            minXY.x = MIN(x, minXY.x);
            maxXY.x = MAX(x, maxXY.x);
            minXY.y = MIN(y, minXY.y);
            maxXY.y = MAX(y, maxXY.y);
        }
    }

    m_minUtm = minXY;
    m_maxUtm = maxXY;
}

//------------------------------------------------------------------------------
