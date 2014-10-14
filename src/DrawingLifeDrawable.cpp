#include "DrawingLifeDrawable.h"

//------------------------------------------------------------------------------

DrawingLifeDrawable::DrawingLifeDrawable()
{
}

//------------------------------------------------------------------------------

void DrawingLifeDrawable::setViewBounds(const ViewDimensions& viewDimensions)
{
    m_viewDimensions = viewDimensions;
}

//------------------------------------------------------------------------------

double DrawingLifeDrawable::getScaledUtmX(const double normalizedUtmX) const
{
    return ( normalizedUtmX
             * (m_viewDimensions.minDimension - 2.0 * m_viewDimensions.padding)
             + m_viewDimensions.offset.x);
}

//------------------------------------------------------------------------------

double DrawingLifeDrawable::getScaledUtmY(const double normalizedUtmY) const
{
    // Flip y coordinates ??
    return ofGetHeight() - (normalizedUtmY
                            * (m_viewDimensions.minDimension - 2.0 * m_viewDimensions.padding)
                            + m_viewDimensions.offset.y);
}

//------------------------------------------------------------------------------

UtmPoint DrawingLifeDrawable::getScaledUtm(const UtmPoint& normalizedUtmPoint) const
{
    return UtmPoint(getScaledUtmX(normalizedUtmPoint.x),
                    getScaledUtmY(normalizedUtmPoint.y));
}

//------------------------------------------------------------------------------

ofVec2f DrawingLifeDrawable::getScaledVec2f(const double normalizedUtmX,
                                            const double normalizedUtmY) const
{
    return ofVec2f(getScaledUtmX(normalizedUtmX), getScaledUtmY(normalizedUtmY));
}

//------------------------------------------------------------------------------
