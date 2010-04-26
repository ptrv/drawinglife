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

    /**
    * \brief Set values for a GpsPoint.
    * \param latitude
    * \param longitude
    * \param elevation
    * \param timestamp
    * \param location
    */
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
	/**
	* \brief Get latitude.
	* \return latitude double value.
	*/
	double getLatitude() const { return m_latitude; }
	/**
	* \brief Get longitude.
	* \return longitude double value.
	*/
	double getLongitude() const { return m_longitude; }
	/**
	* \brief Get elevation.
	* \return elevation double value.
	*/
	double getElevation() const { return m_elevation; }
	//---------------------------------------------------------------------------
    /**
	* \brief Get location for GpsPoint.
	* \return Location string.
	*/
	const std::string& getLocation() const { return m_location; }
	//---------------------------------------------------------------------------
	/**
	* \brief Get UTM X value.
	* \return UTM X double value.
	*/
	double getUtmX() const { return m_utmX; }
	/**
	* \brief Get UTM Y value.
	* \return UTM Y double value.
	*/
	double getUtmY() const { return m_utmY; }
	/**
	* \brief Get normalized UTM X value.
	* \return normalized UTM X double value.
	*/
	double getNormalizedUtmX() const { return m_normalizedUtmX; }
	/**
	* \brief Get normalized UTM Y value.
	* \return normalized UTM Y double value.
	*/
	double getNormalizedUtmY() const { return m_normalizedUtmY; }
	//---------------------------------------------------------------------------
    /**
	* \brief Get time stamp for GpsPoint.
	* \return Timestamp string.
	*/
	const std::string& getTimestamp() const { return m_timestamp; }
	// -----------------------------------------------------------------------------
	/**
	* \brief Set normalized UTM X value.
	* \param normUtmX the new normalized UTM X value.
	*/
	void setNormalizedUtmX(double normUtmX) { m_normalizedUtmX = normUtmX; }
	/**
	* \brief Set normalized UTM Y value.
	* \param normUtmY the new normalized UTM Y value.
	*/
	void setNormalizedUtmY(double normUtmY) { m_normalizedUtmY = normUtmY; }
	// -----------------------------------------------------------------------------
	/**
	* \brief Clear GpsPoint object.
	*/
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
