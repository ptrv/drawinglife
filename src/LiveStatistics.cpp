/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#include "LiveStatistics.h"
#include "GpsSegment.h"
#include "GpsPoint.h"
#include <string>


LiveStatistics::LiveStatistics( unsigned int screenWidth, unsigned int screenHeight) : 
	m_lastGpsPointId(-1),
	m_lastGpsSegmentId(-1),
	GpsView( screenWidth, screenHeight)
{
	// Initialize the hour histogram with zero.
	for (unsigned int hourIndex = 0; hourIndex < HOURS_PER_DAY; ++hourIndex)
	{
		m_hoursHistogram[hourIndex] = 0;
	}
}


LiveStatistics::~LiveStatistics( void)
{
}


void LiveStatistics::draw()
{
	// Draw background.
	ofFill();
	ofSetColor( m_backgroundColor);
	ofRect( m_positionCoordinates.x, m_positionCoordinates.y, m_width, m_height);

	// Draw border.
	ofNoFill();
	ofSetColor( m_borderColor);
	ofRect( m_positionCoordinates.x, m_positionCoordinates.y, m_width, m_height);


	// Draw a line for each hour.
	double yOffset = (double)m_height / (double)(HOURS_PER_DAY + 1);
	double yPos = m_positionCoordinates.y + yOffset;
	for (unsigned int hourIndex = 0; hourIndex < HOURS_PER_DAY; ++hourIndex)
	{
		glBegin(GL_LINE_STRIP);
		glVertex2d( m_positionCoordinates.x, yPos);
		glVertex2d( m_positionCoordinates.x + m_hoursHistogram[hourIndex], yPos);
		glEnd();
		yPos += yOffset;
	}
}


void LiveStatistics::drawAll()
{
}


void LiveStatistics::update()
{
	// Retrieve current segment.
	const GpsSegment& cSegment = m_walk->getCurrentSegment();

	// Retrieve current point.
	const GpsPoint& cPoint = m_walk->getCurrentPoint();

	// Extract current time stamp.
	const std::string timestamp = cPoint.getTimestamp();
	struct tm date = MakeTimeStruct(timestamp);

	// Update histogram.
	updateHistogram( date);
}


void LiveStatistics::updateHistogram( tm date)
{
	// Create an hour histogram based on the time stamps.
	++m_hoursHistogram[date.tm_hour];
}

