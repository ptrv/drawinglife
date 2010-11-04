#ifndef _LIVESTATISTICS_
#define _LIVESTATISTICS_


#include "DrawingLifeIncludes.h"


class LiveStatistics
{
public:
	LiveStatistics( void);
	~LiveStatistics( void);

	void draw();
	void setPosition( const Point2D position);
	void setDimensions( const float width, const float height);
	void setBackgroundColor( const ColorRGBA backgroundColor);
	void setBorderColor( const ColorRGBA borderColor);

private:
	Point2D m_position;
	float m_width;
	float m_height;
	int m_backgroundColor;
	int m_borderColor;
	
};

#endif // _LIVESTATISTICS_