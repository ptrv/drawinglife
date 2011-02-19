#include "LocationImage.h"
#include "GeographicLib/TransverseMercatorExact.hpp"

using namespace GeographicLib;

LocationImage::LocationImage(MagicBox* magicBox, LocationImageData lid, double minDim, double padding, double offsetX, double offsetY)
:
m_magicBox(magicBox),
m_lid(lid),
m_viewMinDimension(minDim),
m_viewPadding(padding),
m_viewXOffset(offsetX),
m_viewYOffset(offsetY)
{
    m_image.loadImage(lid.path);
    m_image.resize(lid.width, lid.height);
    m_image.setAnchorPercent(0.5, 0.5);

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
    ofSetColor(0xffffffff);
    const ofxPointd& tmp = m_magicBox->getDrawablePoint(m_utm);
    m_image.draw(getScaledUtmX(tmp.x), getScaledUtmY(tmp.y));
//    ofSetColor(0xff0000);
//    ofCircle(getScaledUtmX(tmp.x), getScaledUtmY(tmp.y), 10);

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
