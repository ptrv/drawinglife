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

	void setGpsPoint(double latitude, double longitude, double elevation, std::string timestamp)
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
	std::string getTimestamp() const { return m_timestamp; }
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
	m_points(NULL),
	m_segment(-1)
	{};
	
	~GpsSegment(){};
	
	void setGpsSegment(std::vector<GpsPoint> points, int segment)
	{
		++m_gpsSegmentId;
		m_points = points;
		m_segment = segment;
	}
	std::vector<GpsPoint> getPoints() const { return m_points; }
	int getSegmentNum() const { return m_segment; }
	void clear()
	{
		m_points.clear();
		m_segment = -1;
		m_gpsSegmentId = 0;
	}
};

/**
 * \brief Holds a vector with segments and user.
 *
 */

class GpsData 
{
	int m_gpsDataId;
	std::vector<GpsSegment> m_segments;
	int m_user;

public:
	GpsData()
	:
	m_gpsDataId(0),
	m_segments(NULL),
	m_user(-1)
	{};
	
	~GpsData(){};
	
	void setGpsData(std::vector<GpsSegment> segments, int user)
	{
		++m_gpsDataId;
		m_segments = segments;
		m_user = user;
	}
	void clear()
	{
		m_gpsDataId = 0;
		m_segments.clear();
		m_user = -1;
	}
	std::vector<GpsSegment> getSegments() const { return m_segments; }
	
	
};

#endif // _DATA_H_
