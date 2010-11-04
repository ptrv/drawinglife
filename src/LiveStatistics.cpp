#include "LiveStatistics.h"



LiveStatistics::LiveStatistics( void) : 
	m_width(400), 
	m_height(200),
	m_position(MakePoint2D( 10, 10)),
	m_backgroundColor(MakeHexARGB( 127, 127, 127, 127)),
	m_borderColor(MakeHexARGB( 255, 255, 255, 255))
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