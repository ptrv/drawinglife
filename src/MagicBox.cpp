/*==========================================================
 Copyright (c) avp::ptr, 2010
 ==========================================================*/

#include "MagicBox.h"

double MagicBox::m_zoomLevels[] = {20000.0, 30000.0, 40000.0, 50000.0};
int MagicBox::m_boxNum = 0;

MagicBox::MagicBox(const AppSettings& settings, double size, double padding)
:
m_settings(settings),
m_currentSize(size),
m_padding(padding),
m_defaultSize(10000.0)
{
    m_boxId = m_boxNum;
    ++m_boxNum;
}

MagicBox::~MagicBox()
{
    ofLogVerbose("MagicBox", "destroying");
    --m_boxNum;
}

bool MagicBox::isInBox(const ofxPoint<double>& utmPoint) const
{
    return utmPoint.x >= m_theBox.getX()
            && utmPoint.y >= m_theBox.getY()
            && utmPoint.x < m_theBox.getX() + m_theBox.getWidth()
            && utmPoint.y < m_theBox.getY() + m_theBox.getHeight();
}

bool MagicBox::isInPaddedBox(const ofxPoint<double>& utmPoint) const
{
    return utmPoint.x >= m_paddedBox.getX()
            && utmPoint.y >= m_paddedBox.getY()
            && utmPoint.x < m_paddedBox.getX() + m_paddedBox.getWidth()
            && utmPoint.y < m_paddedBox.getY() + m_paddedBox.getHeight();
}

const ofxPoint<double> MagicBox::getDrawablePoint(const UtmPoint& utmPoint) const
{
    return ofxPoint<double> ((utmPoint.x - m_theBox.getX()) / m_theBox.getWidth(),
                             (utmPoint.y - m_theBox.getY()) / m_theBox.getHeight());
}

void MagicBox::setCenter(double x, double y)
{
//	m_centerUtm.
}
void MagicBox::setupBox(const ofxPoint<double>& currUtm, double lon0)
{
    m_centerUtm = currUtm;
//    ofLog(OF_LOG_VERBOSE, "Center box %d, x: %lf, y: %lf", m_boxId+1, m_centerUtm.x, m_centerUtm.y);

    m_theBox.setFromCenter(m_centerUtm , m_currentSize, m_currentSize);
//    ofLog(OF_LOG_VERBOSE, "Box %d, x: %lf, box y: %lf, box w: %lf, box h: %lf", m_boxId+1, m_theBox.x, m_theBox.y, m_theBox.width, m_theBox.height);

    m_paddedBox.setFromCenter(m_centerUtm, m_currentSize-(2*m_padding), m_currentSize-(2*m_padding));
}

void MagicBox::setupBoxStatic(const ofxPoint<double>& currUtm, double lon0,
                              double width, double height)
{
    m_centerUtm = currUtm;
    m_currentSize = width;
    m_padding = 0;
    ofLog(OF_LOG_VERBOSE, "Center box %d, x: %lf, y: %lf", m_boxId+1, m_centerUtm.x, m_centerUtm.y);

    m_theBox.setFromCenter(m_centerUtm , width, height);
    ofLog(OF_LOG_VERBOSE, "Box %d, x: %lf, box y: %lf, box w: %lf, box h: %lf",
          m_boxId+1, m_theBox.getX(), m_theBox.getY(), m_theBox.getWidth(), m_theBox.getHeight());

    m_paddedBox.setFromCenter(m_centerUtm, width, height);
}

void MagicBox::updateBoxIfNeeded(const ofxPoint<double>& utmPoint)
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
        if (utmPoint.x >= m_paddedBox.getX() + m_paddedBox.getWidth())
        {
            double x1 = m_theBox.getX();
            x1 += utmPoint.x - (m_paddedBox.getX() + m_paddedBox.getWidth());
            m_theBox.setX(x1);
            double x2 = m_paddedBox.getX();
            x2 += utmPoint.x - (m_paddedBox.getX() + m_paddedBox.getWidth());
            m_paddedBox.setX(x2);
        }
        else if (utmPoint.x < m_paddedBox.getX())
        {
            double x1 = m_theBox.getX();
            x1 += utmPoint.x - m_paddedBox.getX();
            m_theBox.setX(x1);

            double x2 = m_paddedBox.getX();
            x2 += utmPoint.x - m_paddedBox.getX();
            m_paddedBox.setX(x2);
        }


        if (utmPoint.y >= m_paddedBox.getY() + m_paddedBox.getHeight())
        {
            double y1 = m_theBox.getY();
            y1 += utmPoint.y - (m_paddedBox.getY() + m_paddedBox.getHeight());
            m_theBox.setY(y1);

            double y2 = m_paddedBox.getY();
            y2 += utmPoint.y - (m_paddedBox.getY() + m_paddedBox.getHeight());
            m_paddedBox.setY(y2);
        }
        else if (utmPoint.y < m_paddedBox.getY())
        {
            double y1 = m_theBox.getY();
            y1 += utmPoint.y - m_paddedBox.getY();
            m_theBox.setY(y1);

            double y2 = m_paddedBox.getY();
            y2 += utmPoint.y - m_paddedBox.getY();
            m_paddedBox.setY(y2);
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
        double x = m_theBox.getX();
        x -= sizeToAdd/2;
        m_theBox.setX(x);

        double y = m_theBox.getY();
        y -= sizeToAdd/2;
        m_theBox.setY(y);

        m_theBox.setWidth(m_currentSize);
        m_theBox.setHeight(m_currentSize);

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

const ofxRectangle<double> MagicBox::getNormalizedBox() const
{
    return ofxRectangle<double> (0, 0, 1, 1);
}
const ofxRectangle<double> MagicBox::getNormalizedPaddedBox() const
{
    return ofxRectangle<double> (m_padding / m_theBox.getWidth(),
                                 m_padding / m_theBox.getHeight(),
                                 1 - (m_padding / m_theBox.getWidth()),
                                 1 - (m_padding / m_theBox.getHeight()));
}

void MagicBox::setBoxes()
{
    m_theBox.setFromCenter(m_centerUtm, m_currentSize, m_currentSize);
    if(m_settings.isInteractiveMode())
        m_paddedBox.setFromCenter(m_centerUtm, m_currentSize, m_currentSize);
    else
        m_paddedBox.setFromCenter(m_centerUtm, m_currentSize-(2*m_padding), m_currentSize-(2*m_padding));
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

const GpsPoint MagicBox::getCenterGps() const
{
    return GpsData::getGpsPoint(m_centerUtm);
}
