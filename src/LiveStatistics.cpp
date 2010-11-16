/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#include "LiveStatistics.h"
#include "GpsSegment.h"
#include "GpsPoint.h"
#include <string>


LiveStatistics::LiveStatistics( void) : 
	m_width( 400), 
	m_height( 200),
	m_position(MakePoint2D( 10, 10)),
	m_backgroundColor(MakeHexARGB( 127, 127, 127, 127)),
	m_borderColor(MakeHexARGB( 255, 255, 255, 255)),
	m_lastGpsPointId(-1),
	m_lastGpsSegmentId(-1)
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
	ofRect( m_position.x, m_position.y, m_width, m_height);
	// Draw border.
	ofNoFill();
	ofSetColor( m_borderColor);
	ofRect( m_position.x, m_position.y, m_width, m_height);


	// Draw a line for each hour.
	double yOffset = (double)m_height / (double)(HOURS_PER_DAY + 1);
	double yPos = m_position.y + yOffset;
	for (unsigned int hourIndex = 0; hourIndex < HOURS_PER_DAY; ++hourIndex)
	{
		glBegin(GL_LINE_STRIP);
		glVertex2d( m_position.x, yPos);
		glVertex2d( m_position.x + m_hoursHistogram[hourIndex], yPos);
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
	if (m_lastGpsSegmentId != cSegment.getId())
	{
		m_lastGpsSegmentId = cSegment.getId();
		//std::cout << cSegment << " -----------" << std::endl;
	}

	// Retrieve current point.
	const GpsPoint& cPoint = m_walk->getCurrentPoint();
	m_lastGpsPointId = cPoint.getId();
	//std::cout << cPoint << std::endl;

	// Extract current time stamp.
	const std::string timestamp = cPoint.getTimestamp();
	struct tm date = MakeTimeStruct(timestamp);
	int hour = date.tm_hour;
	int minute = date.tm_min;
	// Print time stamp to console.
	//std::cout << timestamp << " ---> hour: " << hour << ", minute: " << minute << std::endl;

	// Create an hour histogram based on the time stamps.
	m_hoursHistogram[hour]++;
}


void LiveStatistics::setPosition( const Point2D position)
{
	m_position = position;
}


void LiveStatistics::setDimensions( const float width, const float height)
{
	m_width = width;
	m_height = height;
}


void LiveStatistics::setBackgroundColor( const ColorRGBA backgroundColor)
{
	m_backgroundColor = MakeHexARGB( backgroundColor);
}


void LiveStatistics::setBorderColor( const ColorRGBA borderColor)
{
	m_borderColor = MakeHexARGB( borderColor);
}
