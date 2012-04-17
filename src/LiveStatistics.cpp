/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#include "LiveStatistics.h"
#include "GpsSegment.h"
#include "GpsPoint.h"
#include <string>


LiveStatistics::LiveStatistics( const unsigned int screenWidth, const unsigned int screenHeight) : 
	GpsView( screenWidth, screenHeight)
{
	// Nothing to initialize.
}


LiveStatistics::~LiveStatistics( void)
{
	// Nothing to delete.
}


void LiveStatistics::draw( void)
{
	ofEnableAlphaBlending();

	// Draw background.
	ofFill();
	ofSetColorWithHexColorARGB( m_backgroundColor);
	ofRect( m_positionCoordinates.x, m_positionCoordinates.y, m_width, m_height);

	// Draw histogram.
	drawHistogram();

	// Draw border.
	ofNoFill();
	ofSetColorWithHexColorARGB( m_borderColor);
	ofRect( m_positionCoordinates.x, m_positionCoordinates.y, m_width, m_height);

	ofDisableAlphaBlending();
}


void LiveStatistics::drawAll( void)
{
	// Not implemented yet.
}


void LiveStatistics::update( void)
{
	// Retrieve current point.
	const GpsPoint& currentPoint = m_walk->getCurrentPoint();

	// Extract current time stamp.
	const std::string timestamp = currentPoint.getTimestamp();
	struct tm date = MakeTimeStruct( timestamp);

	// Update histogram.
	updateHistogram( date);
}