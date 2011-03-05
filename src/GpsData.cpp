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

GpsData::GpsData(AppSettings* settings)
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
    // -----------------------------------------------------------------------------
    // Calculating central meridian for projection.
    m_lon0 = m_minLon + (m_maxLon-m_minLon)/2;
    // -----------------------------------------------------------------------------
    TMS.Forward(Math::real(m_lon0), m_minLat, m_minLon, m_minUtmX, m_minUtmY, minGamma, minK);
    TMS.Forward(Math::real(m_lon0), m_maxLat, m_maxLon, m_maxUtmX, m_maxUtmY, maxGamma, maxK);
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
double GpsData::getLongitude(int segmentIndex, int pointIndex)
{
	double longitude = -1000.0;
	if (segmentIndex < (int)m_segments.size())
	{
		if (pointIndex < (int)m_segments[segmentIndex].getPoints().size())
		{
			longitude = m_segments[segmentIndex].getPoints()[pointIndex].getLongitude();
		}
	}
	return longitude;
}

double GpsData::getLatitude(int segmentIndex, int pointIndex)
{
	double latitude = -1000.0;
	if (segmentIndex < (int)m_segments.size())
	{
		if (pointIndex < (int)m_segments[segmentIndex].getPoints().size())
		{
			latitude = m_segments[segmentIndex].getPoints()[pointIndex].getLatitude();
		}
	}
	return latitude;
}

double GpsData::getElevation(int segmentIndex, int pointIndex)
{
	double elevation = -1000.0;
	if (segmentIndex < (int)m_segments.size())
	{
		if (pointIndex < (int)m_segments[segmentIndex].getPoints().size())
		{
			elevation = m_segments[segmentIndex].getPoints()[pointIndex].getElevation();
		}
	}
	return elevation;
}
// -----------------------------------------------------------------------------

double GpsData::getUtmX(int segmentIndex, int pointIndex)
{
	double utmX = 0.0;
	if (segmentIndex < (int)m_utmPoints.size())
	{
		if (pointIndex < (int)m_utmPoints[segmentIndex].size())
		{
			utmX = m_utmPoints[segmentIndex][pointIndex].x;
		}
	}
	return utmX;
}

double GpsData::getUtmY(int segmentIndex, int pointIndex)
{
	double utmY = 0.0;
	if (segmentIndex < (int)m_utmPoints.size())
	{
		if (pointIndex < (int)m_utmPoints[segmentIndex].size())
		{
			utmY = m_utmPoints[segmentIndex][pointIndex].y;
		}
	}
	return utmY;
}

double GpsData::getNormalizedUtmX(int segmentIndex, int pointIndex)
{
	double utmX = 0.0;
	if (segmentIndex < (int)m_normalizedUtmPoints.size())
	{
		if (pointIndex < (int)m_normalizedUtmPoints[segmentIndex].size())
		{
			utmX = m_normalizedUtmPoints[segmentIndex][pointIndex].x;
		}
	}
	return utmX;
}

double GpsData::getNormalizedUtmY(int segmentIndex, int pointIndex)
{
	double utmY = 0.0;
	if (segmentIndex < (int)m_normalizedUtmPoints.size())
	{
		if (pointIndex < (int)m_normalizedUtmPoints[segmentIndex].size())
		{
			utmY = m_normalizedUtmPoints[segmentIndex][pointIndex].y;
		}
	}
	return utmY;
}

//---------------------------------------------------------------------------
const std::string GpsData::getGpsLocation(int segmentIndex, int pointIndex)
{
	std::string loc = "";
	if (segmentIndex < (int)m_segments.size())
	{
		if (pointIndex < (int)m_segments[segmentIndex].getPoints().size())
		{
			loc = m_segments[segmentIndex].getPoints()[pointIndex].getLocation();
		}
	}
	return loc;
}
// -----------------------------------------------------------------------------
int GpsData::getTotalGpsPoints() const
{
	int num = 0;
	for (unsigned int i = 0; i < m_segments.size(); ++i) {
		for (unsigned int j = 0; j < m_segments[i].getPoints().size(); ++j) {
			++num;
		}
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
    for(unsigned int i = 0; i < m_segments.size(); ++i)
    {
        std::vector<UtmPoint> utmVec;
        utmVec.reserve( m_segments[i].getPoints().size());
        for(unsigned int j = 0; j < m_segments[i].getPoints().size(); ++j)
        {
            Math::real gamma, k;
            UtmPoint utmP;
            TMS.Forward(Math::real(m_lon0Global),
                        m_segments[i].getPoints()[j].getLatitude(),
                        m_segments[i].getPoints()[j].getLongitude(),
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
	m_normalizedUtmPoints = m_utmPoints;
	for (unsigned int i = 0; i < m_utmPoints.size(); ++i) {
		for (unsigned int j = 0; j < m_utmPoints[i].size(); ++j)
		{
			double x = m_utmPoints[i][j].x;
			double y = m_utmPoints[i][j].y;
			x = (x - m_minUtmX) / (m_maxUtmX - m_minUtmX);
			y = (y - m_minUtmY) / (m_maxUtmY - m_minUtmY);
			m_normalizedUtmPoints[i][j].x = x;
			m_normalizedUtmPoints[i][j].y = y;
		}
	}
}
void GpsData::normalizeUtmPointsGlobal()
{
	setGlobalMinMaxRatioUTM();
	m_normalizedUtmPointsGlobal = m_utmPoints;
	for (unsigned int i = 0; i < m_utmPoints.size(); ++i) {
		for (unsigned int j = 0; j < m_utmPoints[i].size(); ++j)
		{
			double x = m_utmPoints[i][j].x;
			double y = m_utmPoints[i][j].y;
			x = (x - minDrawX) / (maxDrawX - minDrawX);
			y = (y - minDrawY) / (maxDrawY- minDrawY);
			m_normalizedUtmPointsGlobal[i][j].x = x;
			m_normalizedUtmPointsGlobal[i][j].y = y;
		}
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

	for (unsigned int i = 0; i < m_utmPoints.size(); ++i) {
		for (unsigned int j = 0; j < m_utmPoints[i].size(); ++j)
		{
			double x = m_utmPoints[i][j].x;
			double y = m_utmPoints[i][j].y;
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
    for(unsigned int i = 0; i < m_segments.size(); ++i)
    {
        std::vector<UtmPoint> utmVec;
        utmVec.reserve( m_segments[i].getPoints().size());
        for(unsigned int j = 0; j < m_segments[i].getPoints().size(); ++j)
        {
            Math::real gamma, k;
            UtmPoint utmP;
            TMS.Forward(Math::real(m_lon0),
                        m_segments[i].getPoints()[j].getLatitude(),
                        m_segments[i].getPoints()[j].getLongitude(),
                        utmP.x, utmP.y, gamma, k);

            utmP.speed = m_segments[i].getPoints()[j].getSpeed();

            utmVec.push_back(utmP);
        }
        m_utmPoints.push_back(utmVec);
    }
}

// TODO parameter m_lon0
void GpsData::calculateUtmPointsGlobalLon(bool regionsOn)
{
    m_indices.clear();
    int counter = 0;
    const TransverseMercatorExact& TMS = TransverseMercatorExact::UTM;
    m_utmPoints.clear();
    m_utmPoints.reserve(m_segments.size());

    const GpsRegion* regions = m_settings->getRegions();

    for(unsigned int i = 0; i < m_segments.size(); ++i)
    {
        std::vector<UtmPoint> utmVec;
        utmVec.reserve( m_segments[i].getPoints().size());
        for(unsigned int j = 0; j < m_segments[i].getPoints().size(); ++j)
        {
            Math::real gamma, k;
            UtmPoint utmP;

            if(regionsOn)
            {
                double lon0 = 0.0;
                if(m_segments[i].getPoints()[j].getLongitude() >= regions[0].minLon && m_segments[i].getPoints()[j].getLongitude() < regions[0].maxLon)
                {
                    lon0 = regions[0].lon0;
                }
                else if(m_segments[i].getPoints()[j].getLongitude() >= regions[1].minLon && m_segments[i].getPoints()[j].getLongitude() < regions[1].maxLon)
                {
                    lon0 = regions[1].lon0;
                }
                else if(m_segments[i].getPoints()[j].getLongitude() >= regions[2].minLon && m_segments[i].getPoints()[j].getLongitude() < regions[2].maxLon)
                {
                    lon0 = regions[2].lon0;
                }
                else if(m_segments[i].getPoints()[j].getLongitude() >= regions[3].minLon && m_segments[i].getPoints()[j].getLongitude() < regions[3].maxLon)
                {
                    lon0 = regions[3].lon0;
                }
                else if(m_segments[i].getPoints()[j].getLongitude() >= regions[4].minLon && m_segments[i].getPoints()[j].getLongitude() <= regions[4].maxLon)
                {
                    lon0 = regions[4].lon0;
                }
                TMS.Forward(Math::real(lon0),
                            m_segments[i].getPoints()[j].getLatitude(),
                            m_segments[i].getPoints()[j].getLongitude(),
                            utmP.x, utmP.y, gamma, k);

            }
            else
            {

                TMS.Forward(Math::real(m_lon0Global),
                            m_segments[i].getPoints()[j].getLatitude(),
                            m_segments[i].getPoints()[j].getLongitude(),
                            utmP.x, utmP.y, gamma, k);
            }

            utmP.speed = m_segments[i].getPoints()[j].getSpeed();

            utmVec.push_back(utmP);

            GpsDataIndex tmpIndex;
            tmpIndex.point = counter;
            tmpIndex.gpsPoint = j;
            tmpIndex.gpsSegment = i;
            m_indices.push_back(tmpIndex);
            ++counter;
        }
        m_utmPoints.push_back(utmVec);
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
