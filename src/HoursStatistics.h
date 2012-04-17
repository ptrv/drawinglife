/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#ifndef _HOURSSTATISTICS_H_
#define _HOURSSTATISTICS_H_
#define HOURS_PER_DAY 24


#include "LiveStatistics.h"


/**
 * \brief A class to visualize an hours histogram.
**/
class HoursStatistics : public LiveStatistics
{
public:
	HoursStatistics( unsigned int screenWidth, unsigned int screenHeight);
	virtual ~HoursStatistics( void);

protected:
	void updateHistogram( const tm date);
	void drawHistogram( void);

	unsigned int m_histogram[ HOURS_PER_DAY];
	unsigned int m_valuesCount;
};

#endif // _HOURSSTATISTICS_H_