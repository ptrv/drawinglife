/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#include "DrawingLifeIncludes.h"
#include "GpsData.h"
#include "GeographicLib/TransverseMercatorExact.hpp"

#if defined (WIN32)
#undef max
#undef min
#endif
#include <limits>

using namespace GeographicLib;

double GpsData::maxDrawX = -numeric_limits<double>::max();
double GpsData::minDrawX = numeric_limits<double>::max();
double GpsData::maxDrawY = -numeric_limits<double>::max();
double GpsData::minDrawY = numeric_limits<double>::max();
double GpsData::m_lon0Global = 0.0;

double regionsLon0[5] = {-119.0, -74.0, 12.0, 116.0, 146.0};
double regionsMinLon[5] = {-180.0, -100.0, -35.0, 65.0, 130.0};
double regionsMaxLon[5] = {-100.0, -35.0, 65.0, 130.0, 180.0};

GpsData::GpsData(const AppSettings& settings)
:
m_settings(settings),
m_gpsDataId(0),
m_user(""),
m_minLon(0.0),
m_maxLon(0.0),
m_minLat(0.0),
m_maxLat(0.0),
m_minUtmX(0.0),
m_maxUtmX(0.0),
m_minUtmY(0.0),
m_maxUtmY(0.0),
m_lon0(0.0)
{
	m_segments.reserve(1000); // TODO good amount.
}

GpsData::~GpsData()
{
    ofLogVerbose("GpsData", "destroying");
}


// -----------------------------------------------------------------------------
// Set Gps data.
// -----------------------------------------------------------------------------
void GpsData::setGpsData(const std::vector<GpsSegment>& segments,
						 double minLon,
						 double maxLon,
						 double minLat,
						 double maxLat,
						 const std::string& user)
{
	++m_gpsDataId;
	m_segments.clear();
	m_segments = segments;
	m_minLon = minLon;
	m_maxLon = maxLon;
	m_minLat = minLat;
	m_maxLat = maxLat;
    const TransverseMercatorExact& TMS = TransverseMercatorExact::UTM;
    Math::real minGamma, minK, maxGamma, maxK;
    // -------------------------------------------------------------------------
    // Calculating central meridian for projection.
    m_lon0 = m_minLon + (m_maxLon-m_minLon)/2;
    // -------------------------------------------------------------------------
    TMS.Forward(Math::real(m_lon0), m_minLat, m_minLon, m_minUtmX, m_minUtmY,
                minGamma, minK);
    TMS.Forward(Math::real(m_lon0), m_maxLat, m_maxLon, m_maxUtmX, m_maxUtmY,
                maxGamma, maxK);
	m_user = user;
	calculateUtmPoints();
    normalizeUtmPoints();
}
// -----------------------------------------------------------------------------
void GpsData::clear()
{
	m_gpsDataId = 0;
	m_segments.clear();
	m_minLon = 0.0;
	m_maxLon = 0.0;
	m_minLat = 0.0;
	m_maxLat = 0.0;
	m_minUtmX = 0.0;
	m_maxUtmX = 0.0;
	m_minUtmY = 0.0;
	m_maxUtmY = 0.0;
	m_user = "";
}

// -----------------------------------------------------------------------------
// Get Gps point.
// -----------------------------------------------------------------------------
double GpsData::getLongitude(const size_t segmentIndex,
                             const size_t pointIndex) const
{
    try
    {
        const GpsPointVector& points = m_segments.at(segmentIndex).getPoints();
        return points.at(pointIndex).getLongitude();
    }
    catch (const std::out_of_range&)
    {
        return -1000.0;
    }
}

double GpsData::getLatitude(const size_t segmentIndex,
                            const size_t pointIndex) const
{
    try
    {
        const GpsPointVector& points = m_segments.at(segmentIndex).getPoints();
        return points.at(pointIndex).getLatitude();
    }
    catch (const std::out_of_range&)
    {
        return -1000.0;
    }
}

double GpsData::getElevation(const size_t segmentIndex,
                             const size_t pointIndex) const
{
    try
    {
        const GpsPointVector& points = m_segments.at(segmentIndex).getPoints();
        return points.at(pointIndex).getElevation();
    }
    catch (const std::out_of_range&)
    {
        return -1000.0;
    }
}
// -----------------------------------------------------------------------------

double GpsData::getUtmX(const size_t segmentIndex,
                        const size_t pointIndex) const
{
    try
    {
        const UtmSegment& utmSegment = m_utmPoints.at(segmentIndex);
        return utmSegment.at(pointIndex).x;
    }
    catch (const std::out_of_range&)
    {
        return 0.0;
    }
}

double GpsData::getUtmY(const size_t segmentIndex,
                        const size_t pointIndex) const
{
    try
    {
        const UtmSegment& utmSegment = m_utmPoints.at(segmentIndex);
        return utmSegment.at(pointIndex).y;
    }
    catch (const std::out_of_range&)
    {
        return 0.0;
    }
}

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
    try
    {
        const UtmSegment& normUtmSeg = m_normalizedUtmPoints.at(segmentIndex);
        return normUtmSeg.at(pointIndex).x;
    }
    catch (const std::out_of_range&)
    {
        return 0.0;
    }
}

double GpsData::getNormalizedUtmY(const size_t segmentIndex,
                                  const size_t pointIndex) const
{
    try
    {
        const UtmSegment& normUtmSeg = m_normalizedUtmPoints.at(segmentIndex);
        return normUtmSeg.at(pointIndex).y;
    }
    catch (const std::out_of_range&)
    {
        return 0.0;
    }
}

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

//---------------------------------------------------------------------------
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
// -----------------------------------------------------------------------------
int GpsData::getTotalGpsPoints() const
{
	int num = 0;
    BOOST_FOREACH(const GpsSegment& rSegment, m_segments)
    {
        num += rSegment.getPoints().size();
    }
	return num;
}

// -----------------------------------------------------------------------------
// Set min/max aspect ratio with UTM values.
// -----------------------------------------------------------------------------
void GpsData::setMinMaxRatioUTM()
{
	setMinMaxValuesUTM();

	double minLon = this->getMinUtmX();
	double maxLon = this->getMaxUtmX();
	double minLat = this->getMinUtmY();
	double maxLat = this->getMaxUtmY();

	// Calculate horizontal and vertical range.
	double deltaLon = maxLon - minLon;
	double deltaLat = maxLat - minLat;

	// Aspect ratio is: width < height.
	if (deltaLon <	deltaLat)
	{
		m_minUtmX = minLon - (deltaLat - deltaLon)/2.0;
		m_maxUtmX = maxLon + (deltaLat - deltaLon)/2.0;
		m_minUtmY = minLat;
		m_maxUtmY = maxLat;
	}
	// Aspect ratio is: width > height.
	else if (deltaLon > deltaLat)
	{
		m_minUtmX = minLon;
		m_maxUtmX = maxLon;
		m_minUtmY = minLat - (deltaLon - deltaLat)/2.0;
		m_maxUtmY = maxLat + (deltaLon - deltaLat)/2.0;
	}
	// Aspect ratio is: height == width.
	else
	{
		m_minUtmX = minLon;
		m_maxUtmX = maxLon;
		m_minUtmY = minLat;
		m_maxUtmY = maxLat;
	}
}
void GpsData::setGlobalMinMaxRatioUTM()
{
	double minLon = minDrawX;
	double maxLon = maxDrawX;
	double minLat = minDrawY;
	double maxLat = maxDrawY;

	// Calculate horizontal and vertical range.
	double deltaLon = maxLon - minLon;
	double deltaLat = maxLat - minLat;

	// Aspect ratio is: width < height.
	if (deltaLon <	deltaLat)
	{
		minDrawX = minLon - (deltaLat - deltaLon)/2.0;
		maxDrawX = maxLon + (deltaLat - deltaLon)/2.0;
		minDrawY = minLat;
		maxDrawY = maxLat;
	}
	// Aspect ratio is: width > height.
	else if (deltaLon > deltaLat)
	{
		minDrawX = minLon;
		maxDrawX = maxLon;
		minDrawY = minLat - (deltaLon - deltaLat)/2.0;
		maxDrawY = maxLat + (deltaLon - deltaLat)/2.0;
	}
	// Aspect ratio is: height == width.
	else
	{
		minDrawX = minLon;
		maxDrawX = maxLon;
		minDrawY = minLat;
		maxDrawY = maxLat;
	}
}

// -----------------------------------------------------------------------------
void GpsData::calculateUtmPoints(double lon0)
{
    const TransverseMercatorExact& TMS = TransverseMercatorExact::UTM;
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
// -----------------------------------------------------------------------------
// Normalize Utm points.
// -----------------------------------------------------------------------------
void GpsData::normalizeUtmPoints()
{
	setMinMaxRatioUTM();
    normalizeUtmPoints(m_normalizedUtmPoints);
}
void GpsData::normalizeUtmPointsGlobal()
{
	setGlobalMinMaxRatioUTM();
    normalizeUtmPoints(m_normalizedUtmPointsGlobal);
}

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
            x = (x - m_minUtmX) / (m_maxUtmX - m_minUtmX);
            y = (y - m_minUtmY) / (m_maxUtmY - m_minUtmY);

            normalizedSegments.push_back(UtmPoint(x, y));
        }
        utmDataVec.push_back(normalizedSegments);
    }
}

void GpsData::setMinMaxValuesUTM()
{
	double minX = numeric_limits<double>::max();
	double minY = numeric_limits<double>::max();
	// using -std::numeric_limits<double>::max() for smallest double
	// std::numeric_limits<double>::min() yields smallest positiv number.
	double maxX = -numeric_limits<double>::max();
	double maxY = -numeric_limits<double>::max();

    BOOST_FOREACH(const UtmSegment& segments, m_utmPoints)
    {
        BOOST_FOREACH(const UtmPoint& utmPoint, segments)
        {
            double x = utmPoint.x;
            double y = utmPoint.y;
            if (x < minX) minX = x;
            if (x > maxX) maxX = x;
            if (y < minY) minY = y;
            if (y > maxY) maxY = y;
        }
    }

    m_minUtmX = minX;
    m_minUtmY = minY;
    m_maxUtmX = maxX;
    m_maxUtmY = maxY;
}

void GpsData::calculateUtmPoints()
{
    const TransverseMercatorExact& TMS = TransverseMercatorExact::UTM;
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

UtmPoint GpsData::getUtmPointWithRegion(double lat, double lon,
                                        const AppSettings& settings)
{
    const GpsRegion* regions = settings.getRegions();
    UtmPoint utmP;
    Math::real gamma, k;
    const TransverseMercatorExact& TMS = TransverseMercatorExact::UTM;

    if(settings.isRegionsOn())
    {
        double lon0 = 0.0;
        if(lon >= regions[0].minLon && lon < regions[0].maxLon)
        {
            lon0 = regions[0].lon0;
        }
        else if(lon >= regions[1].minLon && lon < regions[1].maxLon)
        {
            lon0 = regions[1].lon0;
        }
        else if(lon >= regions[2].minLon && lon < regions[2].maxLon)
        {
            lon0 = regions[2].lon0;
        }
        else if(lon >= regions[3].minLon && lon < regions[3].maxLon)
        {
            lon0 = regions[3].lon0;
        }
        else if(lon >= regions[4].minLon && lon <= regions[4].maxLon)
        {
            lon0 = regions[4].lon0;
        }
        TMS.Forward(Math::real(lon0),
                    lat,
                    lon,
                    utmP.x, utmP.y, gamma, k);
        utmP.lon0 = lon0;

    }
    else
    {

        TMS.Forward(Math::real(m_lon0Global),
                    lat,
                    lon,
                    utmP.x, utmP.y, gamma, k);
        utmP.lon0 = m_lon0Global;
    }

    return utmP;
}

GpsPoint GpsData::getGpsPoint(const ofxPoint<double>& utmP)
{
    const TransverseMercatorExact& TMS = TransverseMercatorExact::UTM;
    Math::real gamma, k;

    GpsPoint p;
    double lat = 0.0;
    double lon = 0.0;
    // Works just for lon > -35 and lon < 65,
    // see AppSettings.xml settings->meridian->region3
    TMS.Reverse(Math::real(12.0),
                utmP.x,
                utmP.y,
                lat, lon, gamma, k);
    p.setGpsPointFromLatLon(lat, lon);

//    std::cout << "lat: " << lat << ", lon: " << lon << std::endl;
    return p;

}

// TODO parameter m_lon0
void GpsData::calculateUtmPointsGlobalLon(bool regionsOn)
{
    m_indices.clear();
    int counter = 0;
    //const TransverseMercatorExact& TMS = TransverseMercatorExact::UTM;
    m_utmPoints.clear();
    m_utmPoints.reserve(m_segments.size());

//    const GpsRegion* regions = m_settings.getRegions();
    int i = 0;
    BOOST_FOREACH(const GpsSegment& rSegment, m_segments)
    {
        int j = 0;
        std::vector<UtmPoint> utmVec;
        utmVec.reserve( rSegment.getPoints().size());
        BOOST_FOREACH(const GpsPoint& rPoint, rSegment.getPoints())
        {
            UtmPoint utmP = getUtmPointWithRegion(rPoint.getLatitude(),
                                                  rPoint.getLongitude(),
                                                  m_settings);

            utmP.speed = rPoint.getSpeed();

            utmVec.push_back(utmP);

            GpsDataIndex tmpIndex;
            tmpIndex.point = counter;
            tmpIndex.gpsPoint = j++;
            tmpIndex.gpsSegment = i;
            m_indices.push_back(tmpIndex);
            ++counter;
        }
        m_utmPoints.push_back(utmVec);
        ++i;
    }
}

void GpsData::setGlobalValues(double minX,
                              double maxX,
                              double minY,
                              double maxY,
                              double lon0)
{
    minDrawX = minX;
    maxDrawX = maxX;
    minDrawY = minY;
    maxDrawY = maxY;

    m_lon0Global = lon0;
    //normalizeUtmPointsGlobal();
}
