/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#ifndef _GPSSEGMENT_H_
#define _GPSSEGMENT_H_

#include <ostream>
#include "GpsPoint.h"

/**
 * \brief Has vector with GpsPoints that have the same segment number.
 *
 */

class GpsSegment
{
	int m_gpsSegmentId;	///< TODO: Not in use as one could think. 
	std::vector<GpsPoint> m_points;
	int m_segment;	///< Identifier of the segment. 
	friend std::ostream& operator<< (std::ostream& os, const GpsSegment& obj);

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
	* \param points a vector with points.
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
	int getPointsCount() const	{ return m_points.size();	}
	int getId() const			{ return m_segment;			}
};


// -----------------------------------------------------------------------------
// Streams. 
// -----------------------------------------------------------------------------

inline std::ostream& operator<< (std::ostream& os, const GpsSegment& obj)
{
	os << "GpsSegment #" << obj.m_segment;
	//os << ": (";
	//std::vector<GpsPoint> points = obj.m_points;
	//for(unsigned i = 0; i < points.size(); ++i)
	//	os << points[i] << ", ";
	//os << ")";
	return os;
}


#endif // _GPSSEGMENT_H_
