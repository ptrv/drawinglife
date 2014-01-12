#include "DrawingLifeDrawable.h"

DrawingLifeDrawable::DrawingLifeDrawable()
:
m_screenWidth(0.0),
m_screenHeight(0.0),
m_viewMinDimension(0.0),
m_viewPadding(0.0),
m_viewOffset(0.0, 0.0)
{
}

void DrawingLifeDrawable::setViewBounds(double screenWidth, double screenHeight,
                                        const ofxPoint<double>& offset,
                                        double minDimension, double padding)
{
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;
    m_viewOffset = offset;
    m_viewMinDimension = minDimension;
    m_viewPadding = padding;
}

double DrawingLifeDrawable::getScaledUtmX(const double normalizedUtmX) const
{
    return ( normalizedUtmX * (m_viewMinDimension - 2.0 * m_viewPadding) + m_viewOffset.x);
}

double DrawingLifeDrawable::getScaledUtmY(const double normalizedUtmY) const
{
    // Flip y coordinates ??
    return ofGetHeight() - ( normalizedUtmY * (m_viewMinDimension - 2.0 * m_viewPadding) + m_viewOffset.y);
}

UtmPoint DrawingLifeDrawable::getScaledUtm(const UtmPoint& normalizedUtmPoint) const
{
    return UtmPoint(getScaledUtmX(normalizedUtmPoint.x),
                    getScaledUtmY(normalizedUtmPoint.y));
}
