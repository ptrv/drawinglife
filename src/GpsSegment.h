/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#ifndef _GPSSEGMENT_H_
#define _GPSSEGMENT_H_

#include "GpsPoint.h"
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

#endif // _GPSSEGMENT_H_
