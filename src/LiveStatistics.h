/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#ifndef _LIVESTATISTICS_
#define _LIVESTATISTICS_
#define HOURS_PER_DAY 24


#include "GpsView.h"
#include "DrawingLifeIncludes.h"


class LiveStatistics : public GpsView
{
public:
	LiveStatistics( void);
	virtual ~LiveStatistics( void);

	void draw();
	void drawAll();
	void update();
	void setPosition( const Point2D position);
	void setDimensions( const float width, const float height);
	void setBackgroundColor( const ColorRGBA backgroundColor);
	void setBorderColor( const ColorRGBA borderColor);

protected:
	Point2D m_position;
	float m_width;
	float m_height;
	int m_backgroundColor;
	int m_borderColor;
	unsigned int m_hoursHistogram[HOURS_PER_DAY];
	int m_lastGpsPointId;
	int m_lastGpsSegmentId;
};

#endif // _LIVESTATISTICS_