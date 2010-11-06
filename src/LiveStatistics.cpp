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
	m_dummyCount( 0)
{
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
	ofRect( m_position.x + 10, m_position.y + 10, 50 + m_dummyCount, 10);
	// Draw dummy line.
	glBegin(GL_LINE_STRIP);
	glVertex2d( m_position.x, m_position.y + m_height * 0.5f);
	glVertex2d( m_position.x + m_dummyCount, m_position.y + m_height * 0.5f);
	glEnd();

	// Extract current time stamp.
	// TODO: The current gps point needs to be addressed.
	GpsSegment segment = m_gpsData->getSegments()[0];
	GpsPoint point = segment.getPoints()[0];
	const std::string timestamp = point.getTimestamp();

	std::cout << "---" << timestamp << "---" << std::endl;
}


void LiveStatistics::drawAll()
{
}


void LiveStatistics::update()
{
	m_dummyCount++;
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