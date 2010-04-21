/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#ifndef _GPSPOINT_H_
#define _GPSPOINT_H_
#include "GeographicLib/TransverseMercatorExact.hpp"

using namespace GeographicLib;
/**
 * \brief Holds one gps point with timestamp.
 *
 */
class GpsPoint
{
	int m_gpsPointId;
	double m_latitude;
	double m_longitude;
	double m_elevation;
	double m_utmX;
	double m_utmY;
	double m_normalizedUtmX;
	double m_normalizedUtmY;
	std::string m_timestamp;
	std::string m_location;

public:
	GpsPoint()
	:
	m_gpsPointId(0),
	m_latitude(0.0),
	m_longitude(0.0),
	m_elevation(0.0),
	m_utmX(0.0),
	m_utmY(0.0),
	m_normalizedUtmX(0.0),
	m_normalizedUtmY(0.0),
	m_timestamp(""),
	m_location("")
	{};

	~GpsPoint(){};

	void setGpsPoint(double latitude,
					 double longitude,
					 double elevation,
					 const std::string& timestamp,
					 const std::string& location)
	{
		++m_gpsPointId;
		m_latitude = latitude;
		m_longitude = longitude;
		m_elevation = elevation;
		m_timestamp = timestamp;
		m_location = location;
        const TransverseMercatorExact& TMS = TransverseMercatorExact::UTM;
        Math::real gamma, k;
        TMS.Forward(Math::real(0), latitude, longitude, m_utmX, m_utmY, gamma, k);
	}
	//---------------------------------------------------------------------------
	double getLatitude() const { return m_latitude; }
	double getLongitude() const { return m_longitude; }
	double getElevation() const { return m_elevation; }
	//---------------------------------------------------------------------------
	const std::string& getLocation() const { return m_location; }
	//---------------------------------------------------------------------------
	double getUtmX() const { return m_utmX; }
	double getUtmY() const { return m_utmY; }
	double getNormalizedUtmX() const { return m_normalizedUtmX; }
	double getNormalizedUtmY() const { return m_normalizedUtmY; }
	//---------------------------------------------------------------------------
	const std::string& getTimestamp() const { return m_timestamp; }
	// -----------------------------------------------------------------------------
	void setNormalizedUtmX(double normUtmX) { m_normalizedUtmX = normUtmX; }
	void setNormalizedUtmY(double normUtmY) { m_normalizedUtmY = normUtmY; }
	// -----------------------------------------------------------------------------
	void clear()
	{
		m_gpsPointId = 0;
		m_latitude = 0;
		m_longitude = 0;
		m_elevation = 0;
		m_utmX = 0;
		m_utmY = 0;
		m_timestamp = "";
		m_location = "";
	}
};


#endif // _GPSPOINT_H_
