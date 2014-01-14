#ifndef DRAWINGLIFEDRAWABLE_H
#define DRAWINGLIFEDRAWABLE_H

#include "DrawingLifeIncludes.h"

class DrawingLifeDrawable
{
public:

    DrawingLifeDrawable();

    void setViewBounds(const ofxPoint<double>& offset,
                       double minDimension, double padding);

    virtual void draw() = 0;

    double getScaledUtmX(double normalizedUtmX) const;
    double getScaledUtmY(double normalizedUtmY) const;
    UtmPoint getScaledUtm(const UtmPoint& normalizedUtmPoint) const;

private:

    double m_viewMinDimension;
    double m_viewPadding;
    ofxPoint<double> m_viewOffset;

};

#endif // DRAWINGLIFEDRAWABLE_H
