/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#include "DrawingLifeIncludes.h"

#include "Walk.h"

float Walk::m_dotSize = 2.0;
int Walk::m_dotAlpha = 127;

//------------------------------------------------------------------------------

Walk::Walk(const AppSettings& settings, ofColor dotColor, bool magicBoxEnabled)
:
DrawingLifeDrawable(),
m_settings(settings),
m_currentGpsPoint(0),
m_currentGpsSegment(0),
m_currentPoint(0),
m_firstPoint(true),
m_currentPointIsImage(false),
m_interactiveMode(false),
m_drawTraced(true),
m_imageAlpha(255)
{
    m_maxPointsToDraw = m_settings.getWalkLength();
    m_dotColor = dotColor;

    m_interactiveMode = m_settings.isInteractiveMode();
    m_drawTraced = m_settings.drawTraced();

    m_currentSegColor.r = m_settings.getColorInteractiveSegR();
    m_currentSegColor.g = m_settings.getColorInteractiveSegG();
    m_currentSegColor.b = m_settings.getColorInteractiveSegB();
    m_currentSegColor.a = m_settings.getColorInteractiveSegA();
}

//------------------------------------------------------------------------------

Walk::~Walk()
{
	m_image.clear();

    ofLogVerbose(AppLogTag::WALK, "destroying");
}

// -----------------------------------------------------------------------------
// Counting through GpsSegments and GpsPoints
// -----------------------------------------------------------------------------

void Walk::update()
{
    const GpsDataPtr gpsData = m_gpsData.lock();
    if (!gpsData)
    {
        return;
    }

    const UtmDataVector& utmPoints= gpsData->getUTMPoints();
    const UtmSegment& utmSegment = utmPoints[m_currentGpsSegment];

    if (m_currentGpsSegment < static_cast<int>(utmPoints.size()))
    {
        if (m_currentGpsPoint < static_cast<int>(utmSegment.size()) - 1)
        {
            if (!m_firstPoint)
            {
                ++m_currentGpsPoint;
            }
            else
            {
                m_firstPoint = false;
            }
        }
        else
        {
            ++m_currentGpsSegment;
            m_currentGpsPoint = 0;
        }
    }
    else
    {
        if (m_currentGpsPoint < static_cast<int>(utmSegment.size()) - 1)
        {
            ++m_currentGpsPoint;
        }
        else
        {
            std::cout << "I'm the last gps point" << std::endl;
            m_currentGpsPoint = 0;
            m_currentGpsSegment = 0;
            m_currentPoint = 0;
        }
    }
    ++m_currentPoint;
}

// -----------------------------------------------------------------------------
// Update functions for interactive mode
// -----------------------------------------------------------------------------

void Walk::updateToNextSegment()
{
    updateToSegment(FORWARD);
}

// -----------------------------------------------------------------------------

void Walk::updateToPreviousSegment()
{
    updateToSegment(BACKWARD);
}

// -----------------------------------------------------------------------------

void Walk::updateToSegment(const tWalkDirection direction)
{
    const GpsDataPtr gpsData = m_gpsData.lock();
    if (!gpsData)
    {
        return;
    }

    const UtmDataVector& utmPoints = gpsData->getUTMPoints();

    if (gpsData->getTotalGpsPoints() > 0
            && static_cast<int>(utmPoints.size()) > 0)
    {
        if (m_firstPoint
                || m_currentGpsSegment < static_cast<int>(utmPoints.size()) - 1)
        {
            if (m_firstPoint)
            {
                m_firstPoint = false;
            }
            else if (direction == FORWARD)
            {
                ++m_currentGpsSegment;
            }
            else if (direction == BACKWARD)
            {
                if (m_currentGpsSegment == 0)
                {
                    m_currentGpsSegment = static_cast<int>(utmPoints.size());
                }
                --m_currentGpsSegment;
            }
            const UtmSegment& utmSegment = utmPoints[m_currentGpsSegment];
            m_currentGpsPoint = static_cast<int>(utmSegment.size()) - 1;
            m_currentPoint += m_currentGpsPoint;
        }
        else
        {
            m_currentGpsPoint = 0;
            m_currentGpsSegment = 0;
            m_currentPoint = 0;
            m_firstPoint = true;
        }
    }
}

// -----------------------------------------------------------------------------
// reset counters
// -----------------------------------------------------------------------------

void Walk::reset()
{
    m_currentGpsPoint = 0;
    m_currentGpsSegment = 0;
    m_currentPoint = 0;
    m_firstPoint = true;
}

// -----------------------------------------------------------------------------
// Draw functions
// -----------------------------------------------------------------------------

void Walk::draw()
{
    const GpsDataPtr gpsData = m_gpsData.lock();
    const MagicBoxPtr magicBox = m_magicBox.lock();
    if (!gpsData || !magicBox)
    {
        return;
    }

    const UtmDataVector& utmPoints = gpsData->getUTMPoints();
    const UtmSegment& currentSegment = utmPoints[m_currentGpsSegment];

    if (utmPoints.size() > 0 && currentSegment.size() > 0)
	{
        // ---------------------------------------------------------------------
		// Draw Gps data
        // ---------------------------------------------------------------------
        const UtmPoint& currentUtm = currentSegment[m_currentGpsPoint];

        if (!m_interactiveMode && !m_settings.isMultiMode()
                && !m_settings.isBoundingBoxFixed())
        {
            magicBox->updateBoxIfNeeded(currentUtm);
        }

        int startSeg, startPoint;
        calculateStartSegmentAndStartPoint(startSeg, startPoint, *gpsData.get());

        if (m_interactiveMode && !m_drawTraced)
        {
            startSeg = m_currentGpsSegment;
        }

        for (int i = startSeg; i <= m_currentGpsSegment; ++i)
        {
            const UtmSegment& segment = utmPoints[i];
            glBegin(GL_LINE_STRIP);
            int pointEnd;
            if (i == m_currentGpsSegment)
            {
                pointEnd = m_currentGpsPoint;
                if (m_interactiveMode && m_drawTraced)
                {
                    ofSetColor(m_currentSegColor);
                }
            }
            else
            {
                pointEnd = static_cast<int>(segment.size()) - 1;
            }

            for (int j = startPoint; j <= pointEnd; ++j)
            {
                const UtmPoint& utm = segment[j];
                bool isInBox = true;

                if (m_settings.isBoundingBoxAuto() && !m_settings.isMultiMode())
                {
                    isInBox = magicBox->isInBox(utm);
                    if (!isInBox)
                    {
                        glEnd();
                        glBegin(GL_LINE_STRIP);
                    }
                }

                if (m_settings.useSpeed())
                {
                    drawSpeedColor(utm.speed, isInBox);
                }

                if (isInBox)
                {
                    const ofxPoint<double>& pt = magicBox->getDrawablePoint(utm);
                    glVertex2d(getScaledUtmX(pt.x),
                               getScaledUtmY(pt.y));
                }
            }
            glEnd();

            startPoint = 0;
        }

        drawCurrentPoint(*magicBox.get(), currentUtm);
    }

    // draw borders of bounding boxes.
    if (m_settings.showBoundingBox())
    {
        drawBoxes();
    }
}

// -----------------------------------------------------------------------------

void Walk::drawAll()
{
    const GpsDataPtr gpsData = m_gpsData.lock();
    const MagicBoxPtr magicBox = m_magicBox.lock();
    if (!gpsData || !magicBox)
    {
        return;
    }

    ofNoFill();
    BOOST_FOREACH(const UtmSegment& utmSegment, gpsData->getUTMPoints())
    {
        glBegin(GL_LINE_STRIP);
        BOOST_FOREACH(const UtmPoint& utmPoint, utmSegment)
        {
            bool isInBox = true;
            if (m_settings.isBoundingBoxAuto() && !m_settings.isMultiMode())
            {
                isInBox = magicBox->isInBox(utmPoint);
            }
            if (isInBox)
            {
                const ofxPoint<double>& tmp =
                        magicBox->getDrawablePoint(utmPoint);
                glVertex2d(getScaledUtmX(tmp.x),
                           getScaledUtmY(tmp.y));
            }
        }
        glEnd();
    }
}

// -----------------------------------------------------------------------------

void Walk::drawBoxes()
{
    if (const MagicBoxPtr magicBox = m_magicBox.lock())
    {
        ofNoFill();
        ofSetColor(255,0,0);

        const ofxRectangle<double>& normalizedBox =
                magicBox->getNormalizedBox();
        const double x = getScaledUtmX(normalizedBox.getX());
        const double y = getScaledUtmY(normalizedBox.getY());
        const double w = getScaledUtmX(normalizedBox.getWidth()) - x;
        const double h = getScaledUtmY(normalizedBox.getHeight()) - y;

        ofRect(x, y , w, h);

        ofNoFill();
        ofSetColor(0,255,0);

        const ofxRectangle<double>& normalizedPaddedBox =
                magicBox->getNormalizedPaddedBox();
        const double xp = getScaledUtmX(normalizedPaddedBox.getX());
        const double yp = getScaledUtmY(normalizedPaddedBox.getY());
        const double wp = getScaledUtmX(normalizedPaddedBox.getWidth()) - xp;
        const double hp = getScaledUtmY(normalizedPaddedBox.getHeight()) - yp;

        ofRect(xp, yp , wp, hp);
    }
}

// -----------------------------------------------------------------------------
// Draw helpers
// -----------------------------------------------------------------------------

void Walk::calculateStartSegmentAndStartPoint(int& startSeg, int& startPoint,
                                              const GpsData& gpsData)
{
    if (m_maxPointsToDraw > 0 && m_currentPoint - m_maxPointsToDraw >= 0)
    {
        const int startIndex = m_currentPoint - m_maxPointsToDraw;
        const GpsDataIndex& gpsDataIndex = gpsData.getIndices()[startIndex];
        startSeg = gpsDataIndex.gpsSegment;
        startPoint = gpsDataIndex.gpsPoint;
    }
    else
    {
        startSeg = 0;
        startPoint = 0;
    }
}

// -----------------------------------------------------------------------------

void Walk::drawSpeedColor(const double speed, bool& isInBox)
{
    const ofColor& color = speed > m_settings.getSpeedThreshold()
                                        ? m_settings.getSpeedColorAbove()
                                        : m_settings.getSpeedColorUnder();

    ofSetColor(color);
    if (color.a == 0.0)
    {
        isInBox = false;
        glEnd();
        glBegin(GL_LINE_STRIP);
    }
}

// -----------------------------------------------------------------------------

void Walk::drawCurrentPoint(const MagicBox& box, const UtmPoint& currentUtm)
{
    const ofxPoint<double>& currentPoint = box.getDrawablePoint(currentUtm);

    if (m_currentPointIsImage)
    {
        ofSetColor(255, 255, 255, m_imageAlpha);
        m_image.draw(getScaledUtmX(currentPoint.x),
                     getScaledUtmY(currentPoint.y));

    }
    else if (!m_interactiveMode)
    {
        ofFill();
        bool skipDrawing = false;
        if (m_settings.useSpeed())
        {
            skipDrawing = currentUtm.speed > m_settings.getSpeedThreshold()
                    ? m_settings.getSpeedColorAbove().a == 0.0
                    : m_settings.getSpeedColorUnder().a == 0.0;
        }
        if (!skipDrawing)
        {
            ofSetColor(m_dotColor);
            ofCircle(getScaledUtmX(currentPoint.x),
                     getScaledUtmY(currentPoint.y),
                     m_dotSize);
        }
    }
}

// -----------------------------------------------------------------------------
// Getters
// -----------------------------------------------------------------------------

const std::string Walk::getCurrentGpsLocation() const
{
    if (const GpsDataPtr gpsData = m_gpsData.lock())
    {
        return gpsData->getGpsLocation(m_currentGpsSegment, m_currentGpsPoint);
    }
    return std::string();
}

// -----------------------------------------------------------------------------

int Walk::getCurrentSegmentNum() const
{
    if (const GpsDataPtr gpsData = m_gpsData.lock())
    {
        try
        {
            const GpsSegmentVector& segments = gpsData->getSegments();
            return segments.at(m_currentGpsSegment).getSegmentNum();
        }
        catch (const std::out_of_range&) {}
    }
    return 0;
}

// -----------------------------------------------------------------------------

int Walk::getCurrentPointNum() const
{
    return m_currentPoint;
}

// -----------------------------------------------------------------------------

std::string Walk::getCurrentTimestamp() const
{
    if (const GpsDataPtr gpsData = m_gpsData.lock())
    {
        try
        {
            const GpsSegmentVector& segments = gpsData->getSegments();
            const GpsPointVector& points =
                    segments.at(m_currentGpsSegment).getPoints();
            return points.at(m_currentGpsPoint).getTimestamp();
        }
        catch (const std::out_of_range&) {}
    }
    return std::string();
}

// -----------------------------------------------------------------------------

double Walk::getCurrentThing(const tFnGetCurrentDouble& fnGetCurrentDouble) const
{
    if (const GpsDataPtr gpsData = m_gpsData.lock())
    {
        return fnGetCurrentDouble(gpsData, m_currentGpsSegment, m_currentGpsPoint);
    }
    else
    {
        return 0.0;
    }
}

// -----------------------------------------------------------------------------

double Walk::getCurrentLongitude() const
{
    return getCurrentThing(boost::bind(&GpsData::getLongitude, _1, _2, _3));
}

// -----------------------------------------------------------------------------

double Walk::getCurrentLatitude() const
{
    return getCurrentThing(boost::bind(&GpsData::getLatitude, _1, _2, _3));
}

// -----------------------------------------------------------------------------

double Walk::getCurrentElevation() const
{
    return getCurrentThing(boost::bind(&GpsData::getElevation, _1, _2, _3));
}

// -----------------------------------------------------------------------------

double Walk::getCurrentUtmX() const
{
    return getCurrentThing(boost::bind(&GpsData::getUtmX, _1, _2, _3));
}

// -----------------------------------------------------------------------------

double Walk::getCurrentUtmY() const
{
    return getCurrentThing(boost::bind(&GpsData::getUtmY, _1, _2, _3));
}

// -----------------------------------------------------------------------------
// Setters
// -----------------------------------------------------------------------------

void Walk::setGpsData(const GpsDataWeak gpsDataWeak)
{
    m_gpsData = gpsDataWeak;
}

// -----------------------------------------------------------------------------

void Walk::setMagicBox(MagicBoxWeak magicBoxWeak)
{
    m_magicBox = magicBoxWeak;
    reset();
    const GpsDataPtr gpsData = m_gpsData.lock();
    MagicBoxPtr magicBox = m_magicBox.lock();
    if (gpsData && magicBox)
    {
        magicBox->setupBox(gpsData->getUtm(0, 0), GpsData::getLon0Glogal());
    }
}

// -----------------------------------------------------------------------------

void Walk::setMagicBoxStatic(MagicBoxWeak magicBoxWeak,
                             const double lat,
                             const double lon)
{
    m_magicBox = magicBoxWeak;
    reset();
    UtmPoint utmP = GpsData::getUtmPointWithRegion(lat, lon, m_settings);
    if (MagicBoxPtr magicBox = m_magicBox.lock())
    {
        magicBox->setupBox(utmP, GpsData::getLon0Glogal());
    }

}

// -----------------------------------------------------------------------------

void Walk::setCurrentPointImage(const ofImage& img, const int alpha)
{
    m_image = img;
    m_image.setAnchorPercent(0.5f, 0.5f);
    m_imageAlpha = alpha;
    m_currentPointIsImage = true;
}

// -----------------------------------------------------------------------------
// other
// -----------------------------------------------------------------------------
void Walk::toggleTraced()
{
    m_drawTraced = !m_drawTraced;
}

// -----------------------------------------------------------------------------
