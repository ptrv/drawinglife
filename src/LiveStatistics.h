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
	LiveStatistics( unsigned int screenWidth, unsigned int screenHeight);
	virtual ~LiveStatistics( void);
	void draw();
	void drawAll();
	void update();

protected:
	void updateHistogram( tm date);
	unsigned int m_hoursHistogram[HOURS_PER_DAY];
	int m_lastGpsPointId;
	int m_lastGpsSegmentId;
};

#endif // _LIVESTATISTICS_