/*==========================================================
 Copyright (c) avp::ptr, 2010
 ==========================================================*/

#include "MagicBox.h"
#include "GeographicLib/TransverseMercatorExact.hpp"

using namespace GeographicLib;

MagicBox::MagicBox()
:
m_currentSize(3000.0),
m_padding(500.0)
{
    //ctor
}

MagicBox::~MagicBox()
{
    //dtor
}

void MagicBox::draw()
{
    ofNoFill();
    ofSetColor(255,0,0);
    ofRect((float)m_theBox.x, (float)m_theBox.y, (float)m_theBox.width, (float)m_theBox.height);
}
bool MagicBox::isInBox(const ofxPointd utmPoint)
{
    return utmPoint.x >= m_theBox.x && utmPoint.y >= m_theBox.y && utmPoint.x < m_theBox.x + m_theBox.width && utmPoint.y < m_theBox.y + m_theBox.height;
}

const ofxPointd MagicBox::getDrawablePoint(const UtmPoint& utmPoint)
{
    ofxPointd normalizedPoint((utmPoint.x - m_theBox.x) / m_theBox.width,
                              (utmPoint.y - m_theBox.y) / m_theBox.height);
    return normalizedPoint;
}

void MagicBox::setupBox(ofxPointd currUtm, double lon0)
{
    m_centerUtm = currUtm;
    m_theBox.setFromCenter(m_centerUtm , m_currentSize, m_currentSize);
    ofLog(OF_LOG_SILENT, "box x: %lf, box y: %lf, box w: %lf, box h: %lf\n", m_theBox.x, m_theBox.y, m_theBox.width, m_theBox.height);
}

void MagicBox::updateBoxIfNeeded(const ofxPointd utmPoint)
{
    if(!this->isInBox(utmPoint))
    {
        if (utmPoint.x > m_theBox.x + m_theBox.width)
        {
            m_theBox.x += utmPoint.x - (m_theBox.x + m_theBox.width);
        }
        else if (utmPoint.x < m_theBox.x)
        {
            m_theBox.x += utmPoint.x - m_theBox.x;
        }


        if (utmPoint.y > m_theBox.y + m_theBox.height)
        {
            m_theBox.y += utmPoint.y - (m_theBox.y + m_theBox.height);
        }
        else if (utmPoint.y < m_theBox.y)
        {
            m_theBox.y += utmPoint.y - m_theBox.y;
        }

    }
}

void MagicBox::addToBoxSize(double newSize)
{
    m_currentSize += newSize;
    m_theBox.x -= newSize/2;
    m_theBox.y -= newSize/2;
    m_theBox.width = m_theBox.height = m_currentSize;
}
const ofxRectangled MagicBox::getNormalizedBox()
{
    ofxRectangled tmpRect;

    tmpRect.x = 0;
    tmpRect.y = 0;
    tmpRect.width = 1;
    tmpRect.height = 1;

    return tmpRect;

}

