/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#ifndef _DRAWABLE_H_
#define _DRAWABLE_H_


class Drawable
{
public:
	virtual void draw() = 0;
	virtual void drawAll() = 0;
	virtual void update() = 0;
};


#endif // _DRAWABLE_H_