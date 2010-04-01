/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#ifndef _GPSPOINT_H_
#define _GPSPOINT_H_

// UTM conversion found at http://www.gpsy.com/gpsinfo/geotoutm/
#include "UTMconversion/LatLong-UTMconversion.h"

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
	std::string m_timestamp;
	std::string m_location;
	
	// variables for UTM conversation
	char utmZone[4];
	int refEllipsoid;//WGS-84. See list with file "LatLong- UTM conversion.cpp" for id numbers
	
	
public:
	GpsPoint()
	:
	m_gpsPointId(0),
	m_latitude(0.0),
	m_longitude(0.0),
	m_elevation(0.0),
	m_utmX(0.0),
	m_utmY(0.0),
	m_timestamp(""),
	m_location(""),
	refEllipsoid(23)
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
		LLtoUTM(refEllipsoid, m_latitude , m_longitude, m_utmY, m_utmX, utmZone);
	}
	double getLatitude() const { return m_latitude; }
	double getLongitude() const { return m_longitude; }
	double getElevation() const { return m_elevation; }
	const std::string& getLocation() const { return m_location; }
	double getUtmX() const { return m_utmX; }
	double getUtmY() const { return m_utmY; }
	const std::string& getTimestamp() const { return m_timestamp; }
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