/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#ifndef _DATA_H_
#define _DATA_H_

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
	
public:
	GpsPoint()
	:
	m_gpsPointId(0),
	m_latitude(0.0),
	m_longitude(0.0),
	m_elevation(0.0),
	m_timestamp("")
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
	}
	double getLatitude() const { return m_latitude; }
	double getLongitude() const { return m_longitude; }
	double getElevation() const { return m_elevation; }
	const std::string& getTimestamp() const { return m_timestamp; }
	void clear()
	{
		m_gpsPointId = 0;
		m_latitude = 0;
		m_longitude = 0;
		m_elevation = 0;
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

public:
	GpsData()
	:
	m_gpsDataId(0),
	m_user(""),
	m_minLon(0.0),
	m_maxLon(0.0),
	m_minLat(0.0),
	m_maxLat(0.0)
	{
		m_segments.reserve(1000); // TODO richtige groesse hacken 
	};
	
	~GpsData(){};
	
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
		m_user = user;
	}
	void clear()
	{
		m_gpsDataId = 0;
		m_segments.clear();
		m_minLon = 0.0;
		m_maxLon = 0.0;
		m_minLat = 0.0;
		m_maxLat = 0.0;
		m_user = "";
	}
	const std::vector<GpsSegment>& getSegments() const { return m_segments; }
	double getMinLon() const { return m_minLon; }
	double getMaxLon() const { return m_maxLon; }
	double getMinLat() const { return m_minLat; }
	double getMaxLat() const { return m_maxLat; }	
	
};

#endif // _DATA_H_
