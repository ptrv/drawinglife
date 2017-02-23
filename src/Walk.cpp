/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#include "DrawingLifeIncludes.h"

#include "Walk.h"

#include "GeoUtils.h"

float Walk::m_dotSize = 2.0;
int Walk::m_dotAlpha = 127;

//------------------------------------------------------------------------------

Walk::Walk(const AppSettings& settings, ofColor dotColor)
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

    fnGetCurrentLongitude = boost::bind(&GpsData::getLongitude, _1, _2, _3);
    fnGetCurrentLatitude = boost::bind(&GpsData::getLatitude, _1, _2, _3);
    fnGetCurrentElevation = boost::bind(&GpsData::getElevation, _1, _2, _3);
    fnGetCurrentUtmX = boost::bind(&GpsData::getUtmX, _1, _2, _3);
    fnGetCurrentUtmY = boost::bind(&GpsData::getUtmY, _1, _2, _3);

    m_fgColor = ofColor(m_settings.getColorForegroundR(),
                        m_settings.getColorForegroundG(),
                        m_settings.getColorForegroundB(),
                        m_settings.getAlphaTrack());
}

//------------------------------------------------------------------------------

Walk::~Walk()
{
    ofLogVerbose(Logger::WALK, "destroying");
    m_image.clear();
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

    const int numSegments = static_cast<int>(utmPoints.size());
    const int numPoints = static_cast<int>(utmSegment.size());

    if (m_currentGpsSegment < numSegments)
    {
        if (m_currentGpsPoint < numPoints - 1)
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
        if (m_currentGpsPoint < numPoints - 1)
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

    const int numSegments = static_cast<int>(utmPoints.size());

    if (gpsData->getTotalGpsPoints() > 0 && numSegments > 0)
    {
        if (m_firstPoint || m_currentGpsSegment < (numSegments - 1))
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
                    m_currentGpsSegment = numSegments;
                }
                --m_currentGpsSegment;
            }
            const UtmSegment& utmSegment = utmPoints[m_currentGpsSegment];
            m_currentGpsPoint = static_cast<int>(utmSegment.size()) - 1;
            m_currentPoint += m_currentGpsPoint;
        }
        else
        {
            reset();
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

    const UtmDataVector& utmData = gpsData->getUTMPoints();
    if (m_currentGpsSegment >= static_cast<int>(utmData.size()))
    {
        return;
    }
    const UtmSegment& currentSegment = utmData[m_currentGpsSegment];

    if (utmData.size() > 0 &&
        currentSegment.size() > 0 &&
        m_currentGpsPoint < static_cast<int>(currentSegment.size()))
    {
        // ---------------------------------------------------------------------
        // Draw Gps data
        // ---------------------------------------------------------------------
        const UtmPoint& currentUtm = currentSegment[m_currentGpsPoint];

        if (!m_interactiveMode &&
            !m_settings.isMultiMode() &&
            !m_settings.isBoundingBoxStatic())
        {
            magicBox->updateBoxIfNeeded(currentUtm);
        }

        int startSeg, startPoint;
        boost::tie(startSeg, startPoint) =
            calculateStartSegmentAndStartPoint(*gpsData);

        if (m_interactiveMode && !m_drawTraced)
        {
            startSeg = m_currentGpsSegment;
        }

        for (int i = startSeg; i <= m_currentGpsSegment; ++i)
        {
            const UtmSegment& segment = utmData[i];
#ifdef USE_OPENGL_FIXED_FUNCTIONS
            glBegin(GL_LINE_STRIP);
            ofSetColor(m_fgColor);
#else
            PointsAndColors pts;
            ofColor currentColor = m_fgColor;
#endif

            int pointEnd;
            if (i == m_currentGpsSegment)
            {
                pointEnd = m_currentGpsPoint;
                if (m_interactiveMode && m_drawTraced)
                {
#ifdef USE_OPENGL_FIXED_FUNCTIONS
                    ofSetColor(m_currentSegColor);
#else
                    currentColor = m_currentSegColor;
#endif
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

                if (m_settings.isBoundingBoxCropMode() &&
                    !m_settings.isMultiMode())
                {
                    isInBox = magicBox->isInBox(utm);
                }

                if (m_settings.useSpeed())
                {
#ifdef USE_OPENGL_FIXED_FUNCTIONS
                    drawSpeedColor(utm.speed, isInBox);
#else
                    drawSpeedColor(utm.speed, isInBox, currentColor);
#endif
                }

                if (isInBox)
                {
                    const ofxPoint<double>& pt = magicBox->getDrawablePoint(utm);
#ifdef USE_OPENGL_FIXED_FUNCTIONS
                    glVertex2d(getScaledUtmX(pt.x), getScaledUtmY(pt.y));
#else
                    pts.add(getScaledVec2f(pt.x, pt.y), currentColor);
#endif
                }
                else
                {
#ifdef USE_OPENGL_FIXED_FUNCTIONS
                    glEnd();
                    glBegin(GL_LINE_STRIP);
#else
                    if (!pts.points.empty())
                    {
                        drawPoints(pts);
                        pts = PointsAndColors();
                    }
#endif
                }
            }
#ifdef USE_OPENGL_FIXED_FUNCTIONS
            glEnd();
#else
            drawPoints(pts);
#endif
            startPoint = 0;
        }

        drawCurrentPoint(*magicBox, currentUtm);
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

    ofSetColor(m_fgColor);

    BOOST_FOREACH(const UtmSegment& utmSegment, gpsData->getUTMPoints())
    {
#ifdef USE_OPENGL_FIXED_FUNCTIONS
        glBegin(GL_LINE_STRIP);
#else
        tPoints pts;
#endif
        BOOST_FOREACH(const UtmPoint& utmPoint, utmSegment)
        {
            bool isInBox = true;
            if (m_settings.isBoundingBoxCropMode() && !m_settings.isMultiMode())
            {
                isInBox = magicBox->isInBox(utmPoint);
            }
            if (isInBox)
            {
                const ofxPoint<double>& tmp =
                        magicBox->getDrawablePoint(utmPoint);
#ifdef USE_OPENGL_FIXED_FUNCTIONS
                glVertex2d(getScaledUtmX(tmp.x), getScaledUtmY(tmp.y));
#else
                pts.push_back(getScaledVec2f(tmp.x, tmp.y));
#endif
            }
            else
            {
#ifdef USE_OPENGL_FIXED_FUNCTIONS
                glEnd();
                glBegin(GL_LINE_STRIP);
#else
                if (!pts.empty())
                {
                    m_vbo.setVertexData(&pts[0], (int)pts.size(), GL_DYNAMIC_DRAW);
                    m_vbo.draw(GL_LINE_STRIP, 0, (int)pts.size());
                    pts.clear();
                }
#endif
            }
        }
#ifdef USE_OPENGL_FIXED_FUNCTIONS
        glEnd();
#else
        m_vbo.setVertexData(&pts[0], (int)pts.size(), GL_DYNAMIC_DRAW);
        m_vbo.draw(GL_LINE_STRIP, 0, (int)pts.size());
#endif
    }
}

// -----------------------------------------------------------------------------

void Walk::drawBoxes()
{
    if (const MagicBoxPtr magicBox = m_magicBox.lock())
    {
        ofNoFill();
        ofSetColor(255,0,0);

        const ofxRectangle<double>& normalizedBox = magicBox->getNormalizedBox();
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

void Walk::drawPoints(const PointsAndColors& pts)
{
#ifndef USE_OPENGL_FIXED_FUNCTIONS
    const tPoints& points = pts.points;
    const tColorSlices& colors = pts.colors;
    m_vbo.setVertexData(&points[0], (int)points.size(), GL_DYNAMIC_DRAW);
    for (tColorSlices::const_iterator it = colors.begin(); it != colors.end(); ++it)
    {
        ofSetColor(it->color);
        m_vbo.draw(GL_LINE_STRIP, it->start, it->total);
    }
#endif
}

// -----------------------------------------------------------------------------
// Draw helpers
// -----------------------------------------------------------------------------

std::pair<int, int> Walk::calculateStartSegmentAndStartPoint(const GpsData& gpsData)
{
    if (m_maxPointsToDraw > 0 && m_currentPoint - m_maxPointsToDraw >= 0)
    {
        const int startIndex = m_currentPoint - m_maxPointsToDraw;
        const GpsDataIndex& gpsDataIndex = gpsData.getIndices()[startIndex];
        return std::make_pair(gpsDataIndex.gpsSegment, gpsDataIndex.gpsPoint);
    }

    return std::make_pair(0, 0);
}

// -----------------------------------------------------------------------------

#ifdef USE_OPENGL_FIXED_FUNCTIONS
void Walk::drawSpeedColor(const double speed, bool& isInBox)
#else
void Walk::drawSpeedColor(const double speed, bool& isInBox, ofColor& currentColor)
#endif
{
    const ofColor& color = speed > m_settings.getSpeedThreshold() ?
        m_settings.getSpeedColorAbove() :
        m_settings.getSpeedColorUnder();

#ifdef USE_OPENGL_FIXED_FUNCTIONS
    ofSetColor(color);
#else
    currentColor = color;
#endif
    if (color.a == 0.0)
    {
        isInBox = false;
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
            skipDrawing = currentUtm.speed > m_settings.getSpeedThreshold() ?
                m_settings.getSpeedColorAbove().a == 0.0 :
                m_settings.getSpeedColorUnder().a == 0.0;
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

double Walk::getCurrentDoubleValue(const tFnGetCurrentDouble& fnGetCurrentDouble) const
{
    if (const GpsDataPtr gpsData = m_gpsData.lock())
    {
        return fnGetCurrentDouble(*gpsData, m_currentGpsSegment, m_currentGpsPoint);
    }
    return 0.0;
}

// -----------------------------------------------------------------------------

double Walk::getCurrentLongitude() const
{
    return getCurrentDoubleValue(fnGetCurrentLongitude);
}

// -----------------------------------------------------------------------------

double Walk::getCurrentLatitude() const
{
    return getCurrentDoubleValue(fnGetCurrentLatitude);
}

// -----------------------------------------------------------------------------

double Walk::getCurrentElevation() const
{
    return getCurrentDoubleValue(fnGetCurrentElevation);
}

// -----------------------------------------------------------------------------

double Walk::getCurrentUtmX() const
{
    return getCurrentDoubleValue(fnGetCurrentUtmX);
}

// -----------------------------------------------------------------------------

double Walk::getCurrentUtmY() const
{
    return getCurrentDoubleValue(fnGetCurrentUtmY);
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
        magicBox->setupBox(gpsData->getUtm(0, 0));
    }
}

// -----------------------------------------------------------------------------

void Walk::setMagicBoxStatic(MagicBoxWeak magicBoxWeak,
                             const double lat,
                             const double lon)
{
    m_magicBox = magicBoxWeak;
    reset();
    UtmPoint utmP = GeoUtils::LonLat2Utm(lon, lat);
    if (MagicBoxPtr magicBox = m_magicBox.lock())
    {
        magicBox->setupBox(utmP);
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
