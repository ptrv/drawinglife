/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#ifndef _DATA_H_
#define _DATA_H_

// found at http://www.gpsy.com/gpsinfo/geotoutm/
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
	refEllipsoid(23)
	{};
	
	~GpsPoint(){};

	void setGpsPoint(double latitude, 
					 double longitude, 
					 double elevation, 
					 const std::string& timestamp)
	{
		++m_gpsPointId;
		m_latitude = latitude;
		m_longitude = longitude;
		m_elevation = elevation;
		m_timestamp = timestamp;
		LLtoUTM(refEllipsoid, m_latitude , m_longitude, m_utmY, m_utmX, utmZone);
	}
	double getLatitude() const { return m_latitude; }
	double getLongitude() const { return m_longitude; }
	double getElevation() const { return m_elevation; }
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
	}
};

/**
 * \brief Has vector with GpsPoints that have the same segment number. 
 * 
 */

class GpsSegment 
{
	int m_gpsSegmentId;
	std::vector<GpsPoint> m_points;
	int m_segment;

public:
	
	GpsSegment()
	:
	m_gpsSegmentId(0),
	m_segment(-1)
	{
		m_points.reserve(1000);
	};
	
	~GpsSegment(){};
	
	void setGpsSegment(const std::vector<GpsPoint>& points, int segment)
	{
		++m_gpsSegmentId;
		m_points.clear();
		m_points = points;
		m_segment = segment;
	}
	const std::vector<GpsPoint>& getPoints() const { return m_points; }
	int getSegmentNum() const { return m_segment; }
	void clear()
	{
		m_points.clear();
		m_segment = -1;
		m_gpsSegmentId = 0;
	}
};

/**
 * \brief Holds a vector with segments, user and min/max values for longitude/latitude.
 *
 */

class GpsData 
{
	int m_gpsDataId;
	std::vector<GpsSegment> m_segments;
	std::string m_user;
	double m_minLon;
	double m_maxLon;
	double m_minLat;
	double m_maxLat;
	double m_minUtmX;
	double m_maxUtmX;
	double m_minUtmY;
	double m_maxUtmY;
	
	char utmZone[4];
	int refEllipsoid;//23 for WGS-84. See list with file "LatLong- UTM conversion.cpp" for id numbers

public:
	GpsData()
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
	};
	
	~GpsData(){};
	
	// -----------------------------------------------------------------------------
	// Set Gps data.
	// -----------------------------------------------------------------------------
	void setGpsData(const std::vector<GpsSegment>& segments, 
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
	}
	// -----------------------------------------------------------------------------
	// Get members.
	// -----------------------------------------------------------------------------
	const std::vector<GpsSegment>& getSegments() const { return m_segments; }
	double getMinLon() const { return m_minLon; }
	double getMaxLon() const { return m_maxLon; }
	double getMinLat() const { return m_minLat; }
	double getMaxLat() const { return m_maxLat; }

	// get UTM convertd min/max values
	double getMinUtmX() const { return m_minUtmX; }
	double getMaxUtmX() const { return m_maxUtmX; }
	double getMinUtmY() const { return m_minUtmY; }
	double getMaxUtmY() const { return m_maxUtmY; }
	
	// -----------------------------------------------------------------------------
	void clear()
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
	double getLongitude(int segmentIndex, int pointIndex)
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

	double getLatitude(int segmentIndex, int pointIndex)
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
	
	double getUtmX(int segmentIndex, int pointIndex)
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
	
	double getUtmY(int segmentIndex, int pointIndex)
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
	int getTotalGpsPoints()
	{
		int num = 0;
		for (unsigned int i = 0; i < m_segments.size(); ++i) {
			for (unsigned int j = 0; j < m_segments[i].getPoints().size(); ++j) {
				++num;
			}
		}
		return num;
	}
};

#endif // _DATA_H_
