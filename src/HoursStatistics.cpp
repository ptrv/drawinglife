/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#include "HoursStatistics.h"


HoursStatistics::HoursStatistics( const unsigned int screenWidth, const unsigned int screenHeight) :
	LiveStatistics( screenWidth, screenHeight)
{
	// Initialize the hour histogram with zeros.
	for (unsigned int hourIndex = 0; hourIndex < HOURS_PER_DAY; ++hourIndex)
	{
		m_histogram[hourIndex] = 0;
	}
}


HoursStatistics::~HoursStatistics( void)
{
	// Nothing to delete.
}


void HoursStatistics::drawHistogram( void)
{
	// Draw a line for each hour.
	//---------------------------

	// General horizontal offset to evenly distribute lines and texts.
	double xOffset = m_width / ( HOURS_PER_DAY + 1);
	// Horizontal position of the line and the text.
	double xPos = m_positionCoordinates.x + xOffset;
	// Lower end of the line. Relative to the upper boundary of the view.
	double yPosBottom = m_positionCoordinates.y - 1;
	// Position of the text at the bottom of the line. Relative to the lower boundary of the view.
	double yPosBottomTextBottom = m_positionCoordinates.y + m_height - 10;


	for( unsigned int hourIndex = 0; hourIndex < HOURS_PER_DAY; ++hourIndex)
	{
		// The actual value for the current hour.
		unsigned int val = m_histogram[ hourIndex];

		glBegin(GL_LINE_STRIP);
		
		// Point at the bottom of the histogram line.
		glVertex2d( xPos, yPosBottom);
		
		// Point at the top of the histogram line.
		double yPosTop = yPosBottom - val;
		glVertex2d( xPos, yPosTop);

		glEnd();

		// Centering the horizontal position of the text.		
		unsigned int xShiftValue = ( val < 10) ? 4 : ( val < 99) ? 7 : 10;
		unsigned int xShiftHour = ( hourIndex < 10) ? 4 : (hourIndex < 99) ? 7 : 10;

		// Text on top of the histogram line.
		// The actual histogram value.
		if( val > 0)
			m_fontHistogram.drawString( ofToString( (int)val), xPos - xShiftValue, yPosTop - 8);

		// Text on the bottom of the histogram line.
		// The hour value from 0 - 23.
		m_fontHistogram.drawString( ofToString( (int)hourIndex), xPos - xShiftHour, yPosBottomTextBottom);
		
		// Shift horizontal position.
		xPos += xOffset;
	}
}


void HoursStatistics::updateHistogram( const tm date)
{
	// Create an hour histogram based on the time stamps.
	++m_histogram[ date.tm_hour];
}