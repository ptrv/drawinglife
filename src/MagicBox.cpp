/*==========================================================
 Copyright (c) avp::ptr, 2010
 ==========================================================*/

#include "MagicBox.h"

MagicBox::MagicBox(double size, double padding)
:
m_currentSize(size),
m_padding(padding)
{
    //ctor
}

MagicBox::~MagicBox()
{
    //dtor
}

bool MagicBox::isInBox(const ofxPointd utmPoint)
{
    return utmPoint.x >= m_theBox.x && utmPoint.y >= m_theBox.y && utmPoint.x < m_theBox.x + m_theBox.width && utmPoint.y < m_theBox.y + m_theBox.height;
}

bool MagicBox::isInPaddedBox(const ofxPointd utmPoint)
{
    return utmPoint.x >= m_paddedBox.x && utmPoint.y >= m_paddedBox.y && utmPoint.x < m_paddedBox.x + m_paddedBox.width && utmPoint.y < m_paddedBox.y + m_paddedBox.height;
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
    ofLog(OF_LOG_VERBOSE, "Center x: %lf, y: %lf\n", m_centerUtm.x, m_centerUtm.y);

    m_theBox.setFromCenter(m_centerUtm , m_currentSize, m_currentSize);
    ofLog(OF_LOG_SILENT, "box x: %lf, box y: %lf, box w: %lf, box h: %lf\n", m_theBox.x, m_theBox.y, m_theBox.width, m_theBox.height);

    m_paddedBox.setFromCenter(m_centerUtm, m_currentSize-(2*m_padding), m_currentSize-(2*m_padding));
}

void MagicBox::updateBoxIfNeeded(const ofxPointd utmPoint)
{

//    if(!this->isInBox(utmPoint))
//    {
//        if (utmPoint.x >= m_theBox.x + m_theBox.width)
//        {
//            m_theBox.x += utmPoint.x - (m_theBox.x + m_theBox.width);
//        }
//        else if (utmPoint.x < m_theBox.x)
//        {
//            m_theBox.x += utmPoint.x - m_theBox.x;
//        }
//
//
//        if (utmPoint.y >= m_theBox.y + m_theBox.height )
//        {
//            m_theBox.y += utmPoint.y - (m_theBox.y + m_theBox.height);
//        }
//        else if (utmPoint.y < m_theBox.y)
//        {
//            m_theBox.y += utmPoint.y - m_theBox.y;
//        }
//    }

    // -----------------------------------------------------------------------------

    if(!this->isInPaddedBox(utmPoint))
    {
        if (utmPoint.x >= m_paddedBox.x + m_paddedBox.width)
        {
            m_theBox.x += utmPoint.x - (m_paddedBox.x + m_paddedBox.width);
            m_paddedBox.x += utmPoint.x - (m_paddedBox.x + m_paddedBox.width);
        }
        else if (utmPoint.x < m_paddedBox.x)
        {
            m_theBox.x += utmPoint.x - m_paddedBox.x;
            m_paddedBox.x += utmPoint.x - m_paddedBox.x;
        }


        if (utmPoint.y >= m_paddedBox.y + m_paddedBox.height)
        {
            m_theBox.y += utmPoint.y - (m_paddedBox.y + m_paddedBox.height);
            m_paddedBox.y += utmPoint.y - (m_paddedBox.y + m_paddedBox.height);
        }
        else if (utmPoint.y < m_paddedBox.y)
        {
            m_theBox.y += utmPoint.y - m_paddedBox.y;
            m_paddedBox.y += utmPoint.y - m_paddedBox.y;
        }
        m_centerUtm = m_theBox.getCenter();
    }
}

void MagicBox::addToBoxSize(double newSize)
{
    double oldSize = m_currentSize;
    double oldPadding = m_padding;

    m_currentSize += newSize;

    if(m_currentSize > 0)
    {
        m_theBox.x -= newSize/2;
        m_theBox.y -= newSize/2;
        m_theBox.width = m_theBox.height = m_currentSize;

        // calcuzlates new padding with old size/padding ratio.
        m_padding = m_currentSize/(oldSize/oldPadding);

        m_paddedBox.setFromCenter(m_theBox.getCenter(), m_currentSize-(2*m_padding), m_currentSize-(2*m_padding));
    }
    else
    {
        m_currentSize = oldSize;
    }
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
const ofxRectangled MagicBox::getNormalizedPaddedBox()
{
    ofxRectangled tmpRect;

    tmpRect.x = 0 + (m_padding/m_theBox.width);
    tmpRect.y = 0 + (m_padding/m_theBox.height);
    tmpRect.width = 1 - (m_padding/m_theBox.width);
    tmpRect.height = 1 - (m_padding/m_theBox.height);

    return tmpRect;

}

