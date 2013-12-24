/*==========================================================
 Copyright (c) avp::ptr, 2013
 ==========================================================*/

#ifndef OFXRECTANGLE_H
#define OFXRECTANGLE_H

#include "ofxPoint.h"

//----------------------------------------------------------
// ofxRectangled
//----------------------------------------------------------

template < class T >
class ofxRectangle
{
  public:
    ofxRectangle() :pos(), width(), height() {}
    ofxRectangle(T _x, T _y, T _w, T _h) : pos(_x, _y), width(_w), height(_h) {}
    virtual ~ofxRectangle(){}

    void set(T px, T py, T w, T h)
    {
        pos.set(px, py);
        width	= w;
        height	= h;
    }

    void set(ofxPoint<T> p, T w, T h)
    {
        pos = p;
        width	= w;
        height	= h;
    }

    void setFromCenter(T px, T py, T w, T h)
    {
        pos.set(px - (w / 2), py - (h / 2));
        width	= w;
        height	= h;
    }

    void setFromCenter(ofxPoint<T> p, T w, T h)
    {
        pos.set(p.x - (w / 2), p.y - (h / 2));
        width	= w;
        height	= h;
    }

    ofxPoint<T> getCenter()
    {
        return ofxPoint<T>(pos.x + width / 2, pos.y + height / 2, 0);
    }

    bool inside (ofxPoint<T> p){
        return inside(p.x, p.y);
    }

    bool inside(T px, T py){
        if( px > pos.x && py > pos.y && px < pos.x + width
                && py < pos.y + height )
        {
            return true;
        }
        return false;
    }

    inline T getX() const { return pos.x; }
    inline T getY() const { return pos.y; }
    inline T getWidth() const { return width; }
    inline T getHeight() const { return height; }

    inline void setX(const T newX) { pos.x = newX; }
    inline void setY(const T newY) { pos.y = newY; }
    inline void setWidth(const T newWidth) { width = newWidth; }
    inline void setHeight(const T newHeight) { height = newHeight; }

private:
    ofxPoint<T> pos;
    T width, height;

};


#endif // OFXRECTANGLE_H
