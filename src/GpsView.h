/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#ifndef _GPSVIEW_H_
#define _GPSVIEW_H_
#define MIN_WIDTH 400
#define MIN_HEIGHT 20


#include "Drawable.h"
#include "GpsData.h"
#include "Walk.h"


enum Position { NORTHWEST, NORTHEAST, SOUTHWEST, SOUTHEAST, NORTH, EAST, WEST, SOUTH, CENTER };


/**
 * \brief A class to implement basic functions of a drawable GpsView.
**/
class GpsView : public Drawable
{
public:

	/**
	 * \brief Default constructor. 
	**/
	GpsView( const unsigned int screenWidth, const unsigned int screenHeight);

	/**
	 * \brief Destructor. 
	**/
	virtual ~GpsView( void);

	/**
	 * \brief Returns a GpsData object. 
	 * \return A GpsData pointer. 
	**/
	const GpsData& getGpsData() const { return *m_gpsData; }

	/**
	 * \brief Sets the GpsData.
	 * \param gpsData A GpsData pointer.
	**/
	void setGpsData( const GpsData* gpsData);
	void setWalk( Walk* walk);
	void setPosition( const Point2D positionCoordinates);
	void setPosition( const Position position);
	void setMargin( const float margin);
	void setDimensions( const float width, const float height);
	void setDimensions( const Point2D dimensions);
	/**
	 * \brief Option to synchronize the width with the width of the walk.
	 * The function must be called after setWalk() has been called.
	 * \param syncWithWidthWalk Boolean switch.
	**/
	void setSyncWidthWithWalk( bool syncWithWidthWalk);
	void setBackgroundColor( const ColorARGB backgroundColor);
	void setBackgroundColor( const int backgroundColor);
	void setBorderColor( const ColorARGB borderColor);
	void setBorderColor( const int borderColor);
	void setLineColor( const ColorARGB lineColor);
	void setLineColor( const int lineColor);
	void setTextColor( const ColorARGB textColor);
	void setTextColor( const int textColor);
	void setScreenDimensions( const unsigned int screenWidth, const unsigned int screenHeight);
	void setFontForHistogram( const ofTrueTypeFont fontHistogram);

protected:
	void setWidthAsWalk( void);

	const GpsData* m_gpsData;		///< Container for gps data.
	Walk* m_walk;

	// Visual members.

	Point2D m_positionCoordinates;
	Position m_position;
	float m_width;
	float m_height;
	float m_margin;
	unsigned int m_screenWidth;
	unsigned int m_screenHeight;
	bool m_syncWidthWithWalk;
	int m_backgroundColor;
	int m_borderColor;
	int m_lineColor;
	int m_textColor;
	ofTrueTypeFont m_fontHistogram;
	float m_maxWidth;
	float m_maxHeight;
};


#endif // _GPSVIEW_H_