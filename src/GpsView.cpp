/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#include "GpsView.h"
#include <exception>


GpsView::GpsView( const unsigned int screenWidth, const unsigned int screenHeight) : 
	m_gpsData( 0),
	m_walk( 0),
	m_position( NORTHEAST),
	m_width( MIN_WIDTH), // Default setting can be overwritten by setWidthAsWalk().
	m_height( MIN_HEIGHT),
	m_margin( 15),
	m_screenWidth( screenWidth),
	m_screenHeight( screenHeight),
	m_syncWidthWithWalk( true),
	m_backgroundColor( makeHexColorARGB( 127, 127, 127, 127)),
	m_borderColor( makeHexColorARGB( 255, 255, 255, 255)),
	m_lineColor( makeHexColorARGB( 255, 255, 255, 255)),
	m_textColor( makeHexColorARGB( 255, 255, 255, 255)),
	m_maxWidth( m_screenWidth - 2 * m_margin),
	m_maxHeight( m_screenHeight - 2 * m_margin)
{
	setPosition( m_position);
	m_fontHistogram.loadFont( "consola.ttf", 8);
}


GpsView::~GpsView( void)
{
	if(m_walk)
		m_walk = 0;
	if(m_gpsData)
		m_gpsData = 0;
}


void GpsView::setGpsData( const GpsData* gpsData)
{
	m_gpsData = 0;
	m_gpsData = gpsData;
}


void GpsView::setWalk( Walk* walk)
{
	m_walk = walk;
}


void GpsView::setPosition( const Point2D positionCoordinates)
{
	m_positionCoordinates = positionCoordinates;
}


void GpsView::setPosition( const Position position)
{
	m_position = position;

	switch( position)
	{
	case NORTHWEST:
		m_positionCoordinates = makePoint2D( m_margin, m_margin);
		break;
	case NORTHEAST:
		m_positionCoordinates = makePoint2D( m_screenWidth - m_width - m_margin, m_margin);
		break;
	case SOUTHWEST:
		m_positionCoordinates = makePoint2D( m_margin, m_screenHeight - m_height - m_margin);
		break;
	case SOUTHEAST:
		m_positionCoordinates = makePoint2D( m_screenWidth - m_width - m_margin, m_screenHeight - m_height - m_margin);
		break;
	case NORTH:
		m_positionCoordinates = makePoint2D( m_screenWidth * 0.5f - m_width * 0.5f, m_margin);
		break;
	case WEST:
		m_positionCoordinates = makePoint2D( m_margin, m_screenHeight * 0.5f - m_height * 0.5f);
		break;
	case EAST:
		m_positionCoordinates = makePoint2D( m_screenWidth - m_width - m_margin, m_screenHeight * 0.5f - m_height * 0.5f);
		break;
	case SOUTH:
		m_positionCoordinates = makePoint2D( m_screenWidth * 0.5f - m_width * 0.5f, m_screenHeight - m_height - m_margin);
		break;
	case CENTER:
		m_positionCoordinates = makePoint2D( m_screenWidth * 0.5f - m_width * 0.5f, m_screenHeight * 0.5f - m_height * 0.5f);
		break;
	default:
		throw std::runtime_error("Undefined position for statistic view.");
		break;
	}
}


void GpsView::setMargin( const float margin)
{
	m_margin = margin;
}


void GpsView::setDimensions( const float width, const float height)
{
	m_width = (width < MIN_WIDTH) ? MIN_WIDTH : (width > m_maxWidth) ? m_maxWidth : width;
	m_height = (height < MIN_HEIGHT) ? MIN_HEIGHT : (height > m_maxHeight) ? m_maxHeight : height;
}


void GpsView::setDimensions( const Point2D dimensions)
{
	setDimensions( dimensions.x, dimensions.y);
}


void GpsView::setWidthAsWalk( void)
{
	if( m_walk)
	{
		// Set width as read from walk.
		Point2D dim = m_walk->getDimensions();
		m_width = dim.x;
		// Update position of the frame.
		setPosition( m_position);
	}
}


void GpsView::setSyncWidthWithWalk( bool syncWithWidthWalk)
{
	m_syncWidthWithWalk = syncWithWidthWalk;
}


void GpsView::setBackgroundColor( const ColorARGB backgroundColor)
{
	m_backgroundColor = makeHexColorARGB( backgroundColor);
}


void GpsView::setBackgroundColor( const int backgroundColor)
{
	m_backgroundColor = backgroundColor;
}


void GpsView::setBorderColor( const ColorARGB borderColor)
{
	m_borderColor = makeHexColorARGB( borderColor);
}


void GpsView::setBorderColor( const int borderColor)
{
	m_borderColor = borderColor;
}


void GpsView::setLineColor( const ColorARGB lineColor)
{
	m_lineColor = makeHexColorARGB( lineColor);
}


void GpsView::setLineColor( const int lineColor)
{
	m_lineColor = lineColor;
}


void GpsView::setTextColor( const ColorARGB textColor)
{
	m_textColor = makeHexColorARGB( textColor);
}


void GpsView::setTextColor( const int textColor)
{
	m_textColor = textColor;
}


void GpsView::setScreenDimensions( const unsigned int screenWidth, const unsigned int screenHeight)
{
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	if( m_syncWidthWithWalk)
	{
		setWidthAsWalk();
	}
	// Update position of the frame.
	setPosition( m_position);
}


void GpsView::setFontForHistogram( const ofTrueTypeFont fontHistogram)
{
	m_fontHistogram = fontHistogram;
}