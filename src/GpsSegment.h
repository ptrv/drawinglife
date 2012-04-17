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

	/**
	* \brief Set GpsSegment values.
	* \param points a vecor with points.
	* \param segment number for segment.
	*/
	void setGpsSegment(const std::vector<GpsPoint>& points, int segment)
	{
		++m_gpsSegmentId;
		m_points.clear();
		m_points = points;
		m_segment = segment;
	}
    /**
	* \brief Get GpsPoints for this segment.
	* \return vector with GpsPoints.
	*/
	const std::vector<GpsPoint>& getPoints() const { return m_points; }
	/**
	* \brief Get segment number.
	* \return Number for segment.
	*/
	int getSegmentNum() const { return m_segment; }
	/**
	* \brief Clear values of GpsSegment.
	*/
	void clear()
	{
		m_points.clear();
		m_segment = -1;
		m_gpsSegmentId = 0;
	}
};

#endif // _GPSSEGMENT_H_
