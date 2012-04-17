/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#ifndef _GPSPOINT_H_
#define _GPSPOINT_H_


#include <ostream>


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
	std::string m_timestamp;
	std::string m_location;
	friend bool operator== (const GpsPoint& lhs, const GpsPoint& rhs);
	friend std::ostream& operator<< (std::ostream& os, const GpsPoint& obj);

public:
	GpsPoint()
	:
	m_gpsPointId(0),
	m_latitude(0.0),
	m_longitude(0.0),
	m_elevation(0.0),
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
	void setGpsPoint(int id,
					 double latitude,
					 double longitude,
					 double elevation,
					 const std::string& timestamp,
					 const std::string& location)
	{
		m_gpsPointId = id;
		m_latitude = latitude;
		m_longitude = longitude;
		m_elevation = elevation;
		m_timestamp = timestamp;
		m_location = location;
	}
	//---------------------------------------------------------------------------
	int getId() const { return m_gpsPointId; }
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
	* \brief Get time stamp for GpsPoint.
	* \return Timestamp string.
	*/
	const std::string& getTimestamp() const { return m_timestamp; }
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
		m_timestamp = "";
		m_location = "";
	}
};

// -----------------------------------------------------------------------------
// Compare functions.
// -----------------------------------------------------------------------------

inline bool operator== (const GpsPoint& lhs, const GpsPoint& rhs)
{
	return lhs.m_gpsPointId == rhs.m_gpsPointId;
}
inline bool operator!= (const GpsPoint& lhs, const GpsPoint& rhs)
{
	return !operator== (lhs,rhs);
}

// -----------------------------------------------------------------------------
// Streams. 
// -----------------------------------------------------------------------------

inline std::ostream& operator<< (std::ostream& os, const GpsPoint& obj)
{
	return os << "GpsPoint #" << obj.m_gpsPointId;
}

#endif // _GPSPOINT_H_
