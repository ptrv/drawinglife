/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#include "GpsData.h"
#include "UTMconversion/LatLong-UTMconversion.h"

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
m_maxUtmY(0.0),
refEllipsoid(23)
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
	LLtoUTM(refEllipsoid, m_minLat, m_minLon, m_minUtmY, m_minUtmX, utmZone);
	LLtoUTM(refEllipsoid, m_maxLat, m_maxLon, m_maxUtmY, m_maxUtmX, utmZone);
	m_user = user;

	setMinMaxRatioUTM();
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
GpsPoint GpsData::getNormalizedUtm(int segmentIndex, int pointIndex)
{
	GpsPoint utm = p;

	if (segmentIndex < (int)m_segments.size())
	{
		if (pointIndex < (int)m_segments[segmentIndex].getPoints().size())
		{
			utm. = m_segments[segmentIndex].getPoints()[pointIndex].getUtmY();
		}
	}

	
	utm

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
// Set min/max ratio with UTM values
// -----------------------------------------------------------------------------
void GpsData::setMinMaxRatioUTM()
{
	double minLon = this->getMinUtmX();
	double maxLon = this->getMaxUtmX();
	double minLat = this->getMinUtmY();
	double maxLat = this->getMaxUtmY();

	double deltaLon = maxLon - minLon;
	double deltaLat = maxLat - minLat;
	if (deltaLon <	deltaLat)
	{
		m_minUtmX = minLon - (deltaLat - deltaLon)/2;
		m_maxUtmX = maxLon + (deltaLat - deltaLon)/2;
		m_minUtmY = minLat;
		m_maxUtmY = maxLat;
	}
	else if (deltaLat < deltaLon)
	{
		m_minUtmX = minLon;
		m_maxUtmX = maxLon;
		m_minUtmY = minLat - (deltaLon - deltaLat)/2;
		m_maxUtmY = maxLat + (deltaLon - deltaLat)/2;
	}
	else
	{
		m_minUtmX = minLon;
		m_maxUtmX = maxLon;
		m_minUtmY = minLat;
		m_maxUtmY = maxLat;
	}
}
