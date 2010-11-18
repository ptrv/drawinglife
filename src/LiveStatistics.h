/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#ifndef _LIVESTATISTICS_
#define _LIVESTATISTICS_


#include "GpsView.h"
#include "DrawingLifeIncludes.h"


class LiveStatistics : public GpsView
{
public:
	LiveStatistics( const unsigned int screenWidth, const unsigned int screenHeight);
	virtual ~LiveStatistics( void);

	void draw( void);
	void drawAll( void);
	void update( void);

protected:
	virtual void updateHistogram( const tm date) = 0;
	virtual void drawHistogram( void) = 0;
};

#endif // _LIVESTATISTICS_