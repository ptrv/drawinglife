/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#include "GpsData.h"
#include "GeographicLib/TransverseMercatorExact.hpp"
#include <limits>

GpsData::GpsData()
:
m_gpsDataId(0),
m_user(""),
m_minLon(0.0),
m_maxLon(0.0),
m_minLat(0.0),
m_maxLat(0.0),
m_minUtmX(0.0),
m_maxUtmX(0.0),
m_minUtmY(0.0),
m_maxUtmY(0.0)
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
    const GeographicLib::TransverseMercatorExact& TMS = GeographicLib::TransverseMercatorExact::UTM;
    GeographicLib::Math::real minGamma, minK, maxGamma, maxK;
    TMS.Forward(GeographicLib::Math::real(0), m_minLat, m_minLon, m_minUtmY, m_minUtmX, minGamma, minK);
    TMS.Forward(GeographicLib::Math::real(0), m_maxLat, m_maxLon, m_maxUtmY, m_maxUtmX, maxGamma, maxK);
	m_user = user;
	normalizeGpsPoints();
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
// -----------------------------------------------------------------------------

double GpsData::getUtmX(int segmentIndex, int pointIndex)
{
	double utmX = 0.0;
	if (segmentIndex < (int)m_segments.size())
	{
		if (pointIndex < (int)m_segments[segmentIndex].getPoints().size())
		{
			utmX = m_segments[segmentIndex].getPoints()[pointIndex].getUtmX();
		}
	}
	return utmX;
}

double GpsData::getUtmY(int segmentIndex, int pointIndex)
{
	double utmY = 0.0;
	if (segmentIndex < (int)m_segments.size())
	{
		if (pointIndex < (int)m_segments[segmentIndex].getPoints().size())
		{
			utmY = m_segments[segmentIndex].getPoints()[pointIndex].getUtmY();
		}
	}
	return utmY;
}
// -----------------------------------------------------------------------------
double GpsData::getNormalizedUtmX(int segmentIndex, int pointIndex)
{
	double normalizedUtmX = 0.0;
	if (segmentIndex < (int)m_segments.size())
	{
		if (pointIndex < (int)m_segments[segmentIndex].getPoints().size())
		{
			normalizedUtmX = m_segments[segmentIndex].getPoints()[pointIndex].getNormalizedUtmX();
		}
	}
	return normalizedUtmX;

}

double GpsData::getNormalizedUtmY(int segmentIndex, int pointIndex)
{
	double normalizedUtmY = 0.0;
	if (segmentIndex < (int)m_segments.size())
	{
		if (pointIndex < (int)m_segments[segmentIndex].getPoints().size())
		{
			normalizedUtmY = m_segments[segmentIndex].getPoints()[pointIndex].getNormalizedUtmY();
		}
	}
	return normalizedUtmY;

}

// -----------------------------------------------------------------------------

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
int GpsData::getTotalGpsPoints()
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
// -----------------------------------------------------------------------------
// Normalize Gps points.
// -----------------------------------------------------------------------------
void GpsData::normalizeGpsPoints()
{
	setMinMaxRatioUTM();
	for (unsigned int i = 0; i < m_segments.size(); ++i) {
		for (unsigned int j = 0; j < m_segments[i].getPoints().size(); ++j)
		{
			GpsPoint* p = const_cast<GpsPoint*>(&(m_segments[i].getPoints()[j]));
			double x = p->getUtmX();
			double y = p->getUtmY();
			x = (x - m_minUtmX) / (m_maxUtmX - m_minUtmX);
			y = (y - m_minUtmY) / (m_maxUtmY - m_minUtmY);
			p->setNormalizedUtmX(x);
			p->setNormalizedUtmY(y);
		}
	}
}

void GpsData::setMinMaxValuesUTM()
{
	double minX = std::numeric_limits<double>::max();
	double minY = std::numeric_limits<double>::max();
	double maxX = -std::numeric_limits<double>::max();
	double maxY = -std::numeric_limits<double>::max();

	for (unsigned int i = 0; i < m_segments.size(); ++i) {
		for (unsigned int j = 0; j < m_segments[i].getPoints().size(); ++j)
		{
			double x = m_segments[i].getPoints()[j].getUtmX();
			double y = m_segments[i].getPoints()[j].getUtmY();
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
