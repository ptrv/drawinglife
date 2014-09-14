#ifndef DRAWINGLIFEDRAWABLE_H
#define DRAWINGLIFEDRAWABLE_H

#include "DrawingLifeIncludes.h"

class DrawingLifeDrawable
{
public:

    DrawingLifeDrawable();
    virtual ~DrawingLifeDrawable() {}

    void setViewBounds(const ViewDimensions& viewDimensions);

    virtual void draw() = 0;

    double getScaledUtmX(double normalizedUtmX) const;
    double getScaledUtmY(double normalizedUtmY) const;
    UtmPoint getScaledUtm(const UtmPoint& normalizedUtmPoint) const;

private:

    ViewDimensions m_viewDimensions;

};

#endif // DRAWINGLIFEDRAWABLE_H
