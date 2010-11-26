/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#ifndef _DRAWABLE_H_
#define _DRAWABLE_H_


/**
 * \brief An interface declaring basic functions for drawable views.
**/
class DrawingLifeDrawable
{
public:
	virtual void draw( void) = 0;
	virtual void drawAll( void) = 0;
	virtual void update( void) = 0;
};


#endif // _DRAWABLE_H_
