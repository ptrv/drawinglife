/*==========================================================
 Copyright (c) avp::ptr, 2010
 ==========================================================*/

#include "MagicBox.h"

double MagicBox::m_zoomLevels[] = {20000.0, 30000.0, 40000.0, 50000.0};
int MagicBox::m_boxNum = 0;

MagicBox::MagicBox(double size, double padding)
:
m_currentSize(size),
m_padding(padding),
m_defaultSize(10000.0)
{
    m_boxId = m_boxNum;
    ++m_boxNum;
}

MagicBox::~MagicBox()
{
    --m_boxNum;
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
    ofLog(OF_LOG_VERBOSE, "Center box %d, x: %lf, y: %lf", m_boxId+1, m_centerUtm.x, m_centerUtm.y);

    m_theBox.setFromCenter(m_centerUtm , m_currentSize, m_currentSize);
    ofLog(OF_LOG_VERBOSE, "Box %d, x: %lf, box y: %lf, box w: %lf, box h: %lf", m_boxId+1, m_theBox.x, m_theBox.y, m_theBox.width, m_theBox.height);

    m_paddedBox.setFromCenter(m_centerUtm, m_currentSize-(2*m_padding), m_currentSize-(2*m_padding));
}

void MagicBox::setupBoxStatic(ofxPointd currUtm, double lon0, double width, double height)
{
    m_centerUtm = currUtm;
    m_currentSize = width;
    m_padding = 0;
    ofLog(OF_LOG_VERBOSE, "Center box %d, x: %lf, y: %lf", m_boxId+1, m_centerUtm.x, m_centerUtm.y);

    m_theBox.setFromCenter(m_centerUtm , width, height);
    ofLog(OF_LOG_VERBOSE, "Box %d, x: %lf, box y: %lf, box w: %lf, box h: %lf", m_boxId+1, m_theBox.x, m_theBox.y, m_theBox.width, m_theBox.height);

    m_paddedBox.setFromCenter(m_centerUtm, width, height);
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

void MagicBox::addToBoxSize(double sizeToAdd)
{
    double oldSize = m_currentSize;
    double oldPadding = m_padding;

    m_currentSize += sizeToAdd;

    if(m_currentSize > 0)
    {
        m_theBox.x -= sizeToAdd/2;
        m_theBox.y -= sizeToAdd/2;
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

void MagicBox::setSize(double newSize)
{
    double oldSize = m_currentSize;
    double oldPadding = m_padding;

    m_theBox.setFromCenter(m_centerUtm, newSize, newSize);

    m_currentSize = newSize;

    m_padding = m_currentSize/(oldSize/oldPadding);
    m_paddedBox.setFromCenter(m_centerUtm, newSize-(2*m_padding),newSize-(2*m_padding));
}

void MagicBox::toggleZoomLevel(unsigned int zoomLevel)
{
    if(zoomLevel > 4)
        zoomLevel = 4;

    switch(zoomLevel)
    {
        case 1:
            if(m_currentSize != m_zoomLevels[0])
                this->setSize(m_zoomLevels[0]);
            else
                this->setSize(m_defaultSize);
        break;
        case 2:
            if(m_currentSize != m_zoomLevels[1])
                this->setSize(m_zoomLevels[1]);
            else
                this->setSize(m_defaultSize);
        break;
        case 3:
            if(m_currentSize != m_zoomLevels[2])
                this->setSize(m_zoomLevels[2]);
            else
                this->setSize(m_defaultSize);
        break;
        case 4:
            if(m_currentSize != m_zoomLevels[3])
                this->setSize(m_zoomLevels[3]);
            else
                this->setSize(m_defaultSize);
        break;
        default:
        break;
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

void MagicBox::setBoxes()
{
    m_theBox.setFromCenter(m_centerUtm, m_currentSize, m_currentSize);
    m_paddedBox.setFromCenter(m_centerUtm, m_currentSize, m_currentSize);
}
void MagicBox::goUp(double val)
{
    m_centerUtm.y += val;
    setBoxes();
}
void MagicBox::goDown(double val)
{
    m_centerUtm.y -= val;
    setBoxes();
}
void MagicBox::goLeft(double val)
{
    m_centerUtm.x -= val;
    setBoxes();
}
void MagicBox::goRight(double val)
{
    m_centerUtm.x += val;
    setBoxes();
}
