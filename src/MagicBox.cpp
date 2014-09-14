/*==========================================================
 Copyright (c) avp::ptr, 2010
 ==========================================================*/

#include "MagicBox.h"

//------------------------------------------------------------------------------

double MagicBox::m_zoomLevels[] = {20000.0, 30000.0, 40000.0, 50000.0};
int MagicBox::m_boxNum = 0;

//------------------------------------------------------------------------------

MagicBox::MagicBox(const AppSettings& settings,
                   const double size,
                   const double padding)
:
m_settings(settings),
m_currentSize(size),
m_padding(padding),
m_defaultSize(10000.0)
{
    m_boxId = m_boxNum;
    ++m_boxNum;
}

//------------------------------------------------------------------------------

MagicBox::~MagicBox()
{
    ofLogVerbose(Logger::MAGIC_BOX, "destroying");
    --m_boxNum;
}

//------------------------------------------------------------------------------

bool MagicBox::isInBox(const ofxPoint<double>& utmPoint) const
{
    return m_theBox.inside(utmPoint);
}

//------------------------------------------------------------------------------

bool MagicBox::isInPaddedBox(const ofxPoint<double>& utmPoint) const
{
    return m_paddedBox.inside(utmPoint);
}

//------------------------------------------------------------------------------

void MagicBox::addX(const double val)
{
    m_theBox.addX(val);
    m_paddedBox.addX(val);
}

//------------------------------------------------------------------------------

void MagicBox::addY(const double val)
{
    m_theBox.addY(val);
    m_paddedBox.addY(val);
}

//------------------------------------------------------------------------------
// Setter
//------------------------------------------------------------------------------

void MagicBox::setupBox(const ofxPoint<double>& currUtm)
{
    m_centerUtm = currUtm;

    m_theBox.setFromCenter(m_centerUtm , m_currentSize, m_currentSize);

    m_paddedBox.setFromCenter(m_centerUtm,
                              m_currentSize - (2 * m_padding),
                              m_currentSize - (2 * m_padding));
}

//------------------------------------------------------------------------------

void MagicBox::setupBoxStatic(const ofxPoint<double>& currUtm,
                              const double width,
                              const double height)
{
    m_centerUtm = currUtm;
    m_currentSize = width;
    m_padding = 0;

    m_theBox.setFromCenter(m_centerUtm , width, height);

    m_paddedBox.setFromCenter(m_centerUtm, width, height);
}

//------------------------------------------------------------------------------

void MagicBox::setSize(const double newSize)
{
    const double oldSize = m_currentSize;
    const double oldPadding = m_padding;

    m_theBox.setFromCenter(m_centerUtm, newSize, newSize);

    m_currentSize = newSize;

    m_padding = m_currentSize/(oldSize/oldPadding);
    m_paddedBox.setFromCenter(m_centerUtm,
                              newSize - (2 * m_padding),
                              newSize - (2 * m_padding));
}

//------------------------------------------------------------------------------
// Update
//------------------------------------------------------------------------------

void MagicBox::updateBoxIfNeeded(const ofxPoint<double>& utmPoint)
{
    if (!isInPaddedBox(utmPoint))
    {
        if (utmPoint.x >= m_paddedBox.getRight())
        {
            const double diffX = utmPoint.x - m_paddedBox.getRight();
            addX(diffX);
        }
        else if (utmPoint.x < m_paddedBox.getX())
        {
            const double diffX = utmPoint.x - m_paddedBox.getX();
            addX(diffX);
        }

        if (utmPoint.y >= m_paddedBox.getBottom())
        {
            const double diffY = utmPoint.y - m_paddedBox.getBottom();
            addY(diffY);
        }
        else if (utmPoint.y < m_paddedBox.getY())
        {
            const double diffY = utmPoint.y - m_paddedBox.getY();
            addY(diffY);
        }

        m_centerUtm = m_theBox.getCenter();
    }
}

//------------------------------------------------------------------------------

void MagicBox::addToBoxSize(const double sizeToAdd)
{
    const double oldSize = m_currentSize;
    const double oldPadding = m_padding;

    m_currentSize += sizeToAdd;

    if (m_currentSize > 0)
    {
        m_theBox.addX(-sizeToAdd / 2);
        m_theBox.addY(-sizeToAdd / 2);

        m_theBox.setWidth(m_currentSize);
        m_theBox.setHeight(m_currentSize);

        // calcuzlates new padding with old size/padding ratio.
        m_padding = m_currentSize / (oldSize / oldPadding);

        m_paddedBox.setFromCenter(m_theBox.getCenter(),
                                  m_currentSize - (2 * m_padding),
                                  m_currentSize - (2 * m_padding));
    }
    else
    {
        m_currentSize = oldSize;
    }
}

//------------------------------------------------------------------------------
// Getters
//------------------------------------------------------------------------------

const ofxRectangle<double> MagicBox::getNormalizedBox() const
{
    return ofxRectangle<double> (0, 0, 1, 1);
}

//------------------------------------------------------------------------------

const ofxRectangle<double> MagicBox::getNormalizedPaddedBox() const
{
    return ofxRectangle<double> (m_padding / m_theBox.getWidth(),
                                 m_padding / m_theBox.getHeight(),
                                 1 - (m_padding / m_theBox.getWidth()),
                                 1 - (m_padding / m_theBox.getHeight()));
}

//------------------------------------------------------------------------------

const GpsPoint MagicBox::getCenterGps() const
{
    return GpsData::getGpsPoint(m_centerUtm);
}

//------------------------------------------------------------------------------

const ofxPoint<double> MagicBox::getDrawablePoint(const UtmPoint& utmPoint) const
{
    return ofxPoint<double> ((utmPoint.x - m_theBox.getX()) / m_theBox.getWidth(),
                             (utmPoint.y - m_theBox.getY()) / m_theBox.getHeight());
}

//------------------------------------------------------------------------------
// Zoom / move
//------------------------------------------------------------------------------
void MagicBox::toggleZoomLevel(unsigned int zoomLevel)
{
    if (zoomLevel > 3)
        zoomLevel = 3;

    if (m_currentSize != m_zoomLevels[zoomLevel])
    {
        setSize(m_zoomLevels[zoomLevel]);
    }
    else
    {
        setSize(m_defaultSize);
    }
}

//------------------------------------------------------------------------------

static const double SIZE_ZOOM_FACTOR = 4.0;

void MagicBox::zoom(Zoom z)
{
    const double val = getSize() / SIZE_ZOOM_FACTOR;

    addToBoxSize(z == ZOOM_IN ? -val : val);
}

//------------------------------------------------------------------------------

void MagicBox::move(Direction d)
{
    const double val = getSize() / SIZE_ZOOM_FACTOR;
    move(d, val);
}

//------------------------------------------------------------------------------

void MagicBox::move(Direction d, const double val)
{
    switch (d) {
    case UP:
        m_centerUtm.y += val;
        break;
    case DOWN:
        m_centerUtm.y -= val;
        break;
    case RIGHT:
        m_centerUtm.x += val;
        break;
    case LEFT:
        m_centerUtm.x -= val;
        break;
    default:
        break;
    }
    setBoxes();
}

//------------------------------------------------------------------------------
// Helper
//------------------------------------------------------------------------------
void MagicBox::setBoxes()
{
    m_theBox.setFromCenter(m_centerUtm, m_currentSize, m_currentSize);
    if (m_settings.isInteractiveMode())
    {
        m_paddedBox.setFromCenter(m_centerUtm, m_currentSize, m_currentSize);
    }
    else
    {
        m_paddedBox.setFromCenter(m_centerUtm,
                                  m_currentSize - (2 * m_padding),
                                  m_currentSize - (2 * m_padding));
    }
}

//------------------------------------------------------------------------------
