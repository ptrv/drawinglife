/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#include "DrawingLifeIncludes.h"
#include "GpsData.h"
#include "GeoUtils.h"

#if defined (WIN32)
#undef max
#undef min
#endif
#include <limits>

//------------------------------------------------------------------------------

ofxPoint<double> GpsData::drawMaxima =
        ofxPoint<double>(-std::numeric_limits<double>::max(),
                         -std::numeric_limits<double>::max());
ofxPoint<double> GpsData::drawMinima =
        ofxPoint<double>(std::numeric_limits<double>::max(),
                         std::numeric_limits<double>::max());

double GpsData::m_lon0Global = 0.0;

double regionsLon0[5] = {-119.0, -74.0, 12.0, 116.0, 146.0};
double regionsMinLon[5] = {-180.0, -100.0, -35.0, 65.0, 130.0};
double regionsMaxLon[5] = {-100.0, -35.0, 65.0, 130.0, 180.0};

//------------------------------------------------------------------------------

GpsData::GpsData(const AppSettings& settings)
:
m_settings(settings),
m_gpsDataId(0),
m_user(""),
m_minLonLat(0.0, 0.0),
m_maxLonLat(0.0, 0.0),
m_minUtm(0.0, 0.0),
m_maxUtm(0.0, 0.0),
m_lon0(0.0)
{
	m_segments.reserve(1000); // TODO good amount.

    tGetLongitudeFn = boost::bind(&GpsPoint::getLongitude, _1);
    tGetLatitudeFn = boost::bind(&GpsPoint::getLatitude, _1);
    tGetElevationFn = boost::bind(&GpsPoint::getElevation, _1);

    tGetUtmXFn = boost::bind(&UtmPoint::x, _1);
    tGetUtmYFn = boost::bind(&UtmPoint::y, _1);
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
    const TransverseMercatorExact& TMS = GeoUtils::getTransversMercatorExact();
    Math::real minGamma, minK, maxGamma, maxK;
    // -------------------------------------------------------------------------
    // Calculating central meridian for projection.
    m_lon0 = m_minLonLat.x + (m_maxLonLat.x - m_minLonLat.x) / 2;
    // -------------------------------------------------------------------------
    TMS.Forward(Math::real(m_lon0),
                m_minLonLat.y, m_minLonLat.x,
                m_minUtm.x, m_minUtm.y,
                minGamma, minK);
    TMS.Forward(Math::real(m_lon0),
                m_maxLonLat.y, m_maxLonLat.x,
                m_maxUtm.x, m_maxUtm.y,
                maxGamma, maxK);
	m_user = user;
	calculateUtmPoints();
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
    return getData(segmentIndex, pointIndex, tGetLongitudeFn);
}

//------------------------------------------------------------------------------

double GpsData::getLatitude(const size_t segmentIndex,
                            const size_t pointIndex) const
{
    return getData(segmentIndex, pointIndex, tGetLatitudeFn);
}

//------------------------------------------------------------------------------

double GpsData::getElevation(const size_t segmentIndex,
                             const size_t pointIndex) const
{
    return getData(segmentIndex, pointIndex, tGetElevationFn);
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
    return getUtmData(segmentIndex, pointIndex, m_utmPoints, tGetUtmXFn);
}

//------------------------------------------------------------------------------

double GpsData::getUtmY(const size_t segmentIndex,
                        const size_t pointIndex) const
{
    return getUtmData(segmentIndex, pointIndex, m_utmPoints, tGetUtmYFn);
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
    return getUtmData(segmentIndex, pointIndex, m_normalizedUtmPoints, tGetUtmXFn);
}

//------------------------------------------------------------------------------

double GpsData::getNormalizedUtmY(const size_t segmentIndex,
                                  const size_t pointIndex) const
{
    return getUtmData(segmentIndex, pointIndex, m_normalizedUtmPoints, tGetUtmYFn);
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

UtmPoint GpsData::getUtmPointWithRegion(double lat, double lon,
                                        const AppSettings& settings)
{
    const GpsRegion* regions = settings.getRegions();
    UtmPoint utmP;
    Math::real gamma, k;
    const TransverseMercatorExact& TMS = GeoUtils::getTransversMercatorExact();

    if (settings.isRegionsOn())
    {
        double lon0 = 0.0;
        for (size_t i = 0; i < AppSettings::NUM_REGIONS; ++i)
        {
            if (lon >= regions[i].minLon && lon < regions[i].maxLon)
            {
                lon0 = regions[i].lon0;
                break;
            }
        }
        TMS.Forward(Math::real(lon0), lat, lon, utmP.x, utmP.y, gamma, k);
        utmP.lon0 = lon0;

    }
    else
    {

        TMS.Forward(Math::real(m_lon0Global), lat, lon, utmP.x, utmP.y,
                    gamma, k);
        utmP.lon0 = m_lon0Global;
    }

    return utmP;
}

//------------------------------------------------------------------------------

GpsPoint GpsData::getGpsPoint(const ofxPoint<double>& utmP)
{
    const TransverseMercatorExact& TMS = TransverseMercatorExact::UTM();
    Math::real gamma, k;

    GpsPoint p;
    double lat = 0.0;
    double lon = 0.0;
    // Works just for lon > -35 and lon < 65,
    // see AppSettings.xml settings->meridian->region3
    TMS.Reverse(Math::real(12.0), utmP.x, utmP.y, lat, lon, gamma, k);
    p.setDataFromLatLon(lat, lon);

    return p;
}

//------------------------------------------------------------------------------

void GpsData::calculateUtmPoints(double lon0)
{
    const TransverseMercatorExact& TMS = GeoUtils::getTransversMercatorExact();
    m_utmPoints.clear();
    m_utmPoints.reserve(m_segments.size());
    BOOST_FOREACH(const GpsSegment& rSegment, m_segments)
    {
        std::vector<UtmPoint> utmVec;
        utmVec.reserve(rSegment.getPoints().size());
        BOOST_FOREACH(const GpsPoint& rPoint, rSegment.getPoints())
        {
            Math::real gamma, k;
            UtmPoint utmP;
            TMS.Forward(Math::real(m_lon0Global),
                        rPoint.getLatitude(),
                        rPoint.getLongitude(),
                        utmP.x, utmP.y, gamma, k);
            utmVec.push_back(utmP);
        }
        m_utmPoints.push_back(utmVec);
    }
}

//------------------------------------------------------------------------------

void GpsData::calculateUtmPointsGlobalLon(bool /*regionsOn*/)
{
    m_indices.clear();
    int counter = 0;
    //const TransverseMercatorExact& TMS = TransverseMercatorExact::UTM;
    m_utmPoints.clear();
    m_utmPoints.reserve(m_segments.size());

//    const GpsRegion* regions = m_settings.getRegions();
    int i = 0;
    BOOST_FOREACH(const GpsSegment& segment, m_segments)
    {
        int j = 0;
        std::vector<UtmPoint> utmVec;
        utmVec.reserve( segment.getPoints().size());
        BOOST_FOREACH(const GpsPoint& point, segment.getPoints())
        {
            UtmPoint utmP = getUtmPointWithRegion(point.getLatitude(),
                                                  point.getLongitude(),
                                                  m_settings);

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

void GpsData::setGlobalValues(const ofxPoint<double>& minXY,
                              const ofxPoint<double>& maxXY,
                              double lon0)
{
    drawMaxima = maxXY;
    drawMinima = minXY;

    m_lon0Global = lon0;
    //normalizeUtmPointsGlobal();
}

//------------------------------------------------------------------------------
// Normalize Utm points.
//------------------------------------------------------------------------------

void GpsData::normalizeUtmPoints()
{
	setMinMaxRatioUTM();
    normalizeUtmPoints(m_normalizedUtmPoints);
}

//------------------------------------------------------------------------------

void GpsData::normalizeUtmPointsGlobal()
{
	setGlobalMinMaxRatioUTM();
    normalizeUtmPoints(m_normalizedUtmPointsGlobal);
}

//------------------------------------------------------------------------------

void GpsData::normalizeUtmPoints(UtmDataVector& utmDataVec)
{
    utmDataVec.clear();
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
        utmDataVec.push_back(normalizedSegments);
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
    if (deltaLon <	deltaLat)
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

void GpsData::setGlobalMinMaxRatioUTM()
{
    const ofxPoint<double> maxLonLat = drawMaxima;
    const ofxPoint<double> minLonLat = drawMinima;

    // Calculate horizontal and vertical range.
    const double deltaLon = maxLonLat.x - minLonLat.x;
    const double deltaLat = maxLonLat.y - minLonLat.y;

    const double deltaLatLonHalf = (deltaLat - deltaLon) / 2.0;
    const double deltaLonLatHalf = (deltaLon - deltaLat) / 2.0;

    // Aspect ratio is: width < height.
    if (deltaLon <	deltaLat)
    {
        drawMinima.x = minLonLat.x - deltaLatLonHalf;
        drawMaxima.x = maxLonLat.x + deltaLatLonHalf;
        drawMinima.y = minLonLat.x;
        drawMaxima.y = maxLonLat.y;
    }
    // Aspect ratio is: width > height.
    else if (deltaLon > deltaLat)
    {
        drawMinima.x = minLonLat.x;
        drawMaxima.x = maxLonLat.x;
        drawMinima.y = minLonLat.y - deltaLonLatHalf;
        drawMaxima.y = maxLonLat.y + deltaLonLatHalf;
    }
    // Aspect ratio is: height == width.
    else
    {
        drawMinima.x = minLonLat.x;
        drawMaxima.x = maxLonLat.x;
        drawMinima.y = minLonLat.x;
        drawMaxima.y = maxLonLat.y;
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

void GpsData::calculateUtmPoints()
{
    const TransverseMercatorExact& TMS = GeoUtils::getTransversMercatorExact();
    m_utmPoints.clear();
    m_utmPoints.reserve(m_segments.size());
    BOOST_FOREACH(const GpsSegment& rSegment, m_segments)
    {
        std::vector<UtmPoint> utmVec;
        utmVec.reserve( rSegment.getPoints().size());
        BOOST_FOREACH(const GpsPoint& rPoint, rSegment.getPoints())
        {
            Math::real gamma, k;
            UtmPoint utmP;
            TMS.Forward(Math::real(m_lon0),
                        rPoint.getLatitude(),
                        rPoint.getLongitude(),
                        utmP.x, utmP.y, gamma, k);

            utmP.speed = rPoint.getSpeed();

            utmVec.push_back(utmP);
        }
        m_utmPoints.push_back(utmVec);
    }
}

//------------------------------------------------------------------------------
