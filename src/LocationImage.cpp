#include "LocationImage.h"
#include "GeographicLib/TransverseMercatorExact.hpp"

using namespace GeographicLib;

LocationImage::LocationImage(MagicBox* magicBox, LocationImageData lid)
:
m_magicBox(magicBox),
m_lid(lid),
m_viewMinDimension(0.0),
m_viewPadding(0.0),
m_viewXOffset(0.0),
m_viewYOffset(0.0)
{
    m_image.loadImage(lid.path);
    m_image.resize(lid.width, lid.height);
    if(lid.anchorType == 1)
    {
        m_image.setAnchorPercent(lid.anchorX, lid.anchorY);
    }
    else if(lid.anchorType == 2)
    {
        m_image.setAnchorPoint(lid.anchorX, lid.anchorY);
    }

    m_utm.x = 0.0;
    m_utm.y = 0.0;

    const TransverseMercatorExact& TMS = TransverseMercatorExact::UTM;
    Math::real minGamma, minK, maxGamma, maxK;

    TMS.Forward(Math::real(12.0), lid.gps.lat, lid.gps.lon, m_utm.x, m_utm.y, minGamma, minK);

}

LocationImage::~LocationImage()
{
    //dtor
}


void LocationImage::draw()
{
//    ofSetColor(0xffffff);
    ofSetColor(255,255,255,m_lid.alpha);
    const ofxPoint<double>& tmp = m_magicBox->getDrawablePoint(m_utm);
    m_image.draw(getScaledUtmX(tmp.x), getScaledUtmY(tmp.y));

    if(m_lid.anchorShow)
    {
        ofSetHexColor(0xff0000);
        ofCircle(getScaledUtmX(tmp.x), getScaledUtmY(tmp.y), 5);
    }

}

double LocationImage::getScaledUtmX(double normalizedUtmX)
{
    return ( normalizedUtmX * (m_viewMinDimension - 2.0 * m_viewPadding) + m_viewXOffset);
}

double LocationImage::getScaledUtmY(double normalizedUtmY)
{
    // Flip y coordinates ??
    return ofGetHeight() - ( normalizedUtmY * (m_viewMinDimension - 2.0 * m_viewPadding) + m_viewYOffset);
}

void LocationImage::setViewBounds(double minDim, double padding, double offsetX, double offsetY)
{
    m_viewMinDimension = minDim;
    m_viewPadding = padding;
    m_viewXOffset = offsetX;
    m_viewYOffset = offsetY;

}

