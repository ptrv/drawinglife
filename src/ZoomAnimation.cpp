#include "ZoomAnimation.h"

#include "GpsData.h"
#include "Timeline.h"
#include "MagicBox.h"
#include "GeoUtils.h"


ZoomAnimation::ZoomAnimation(const AppSettings& settings,
                             const TimelineWeak timeline)
    : m_timeline(timeline)
    , m_zoomAnimFrames(settings.getZoomAnimFrames())
    , m_zoomAnimType(settings.getZoomAnimationCriteria())
    , m_animateXY(!settings.isBoundingBoxFixed())
{
    const double damp = settings.getZoomAnimationDamp();
    const double attr = settings.getZoomAnimationAttraction();
    const double dampCenter = settings.getZoomAnimationDampCenter();
    const double attrCenter = settings.getZoomAnimationAttractionCenter();

    m_integratorZ.reset(new Integrator<double>(0.0f, damp, attr));
    m_integratorXY.reset(new Integrator<ofxPoint<double> >(
                             0.0f, dampCenter, attrCenter));

    m_currentZoomFrame = m_zoomAnimFrames.begin();
}

void ZoomAnimation::update(const MagicBoxVector& magicBoxes)
{
    if (m_currentZoomFrame == m_zoomAnimFrames.end())
    {
        return;
    }

    if (const TimelinePtr timeline = m_timeline.lock())
    {
        if (zoomHasChanged(*timeline))
        {
            const bool isFirst = timeline->isFirst();

            setTargetZ(isFirst);

            if (m_animateXY)
            {
                setTargetXY(isFirst);
            }
        }

        m_integratorZ->update();
        if (m_animateXY)
        {
            m_integratorXY->update();
        }

        if (m_integratorZ->isTargeting() || m_integratorXY->isTargeting())
        {
            BOOST_FOREACH(MagicBoxPtr box, magicBoxes)
            {
                box->setSize(m_integratorZ->getValue());
                if (m_animateXY)
                {
                    box->setupBox(m_integratorXY->getValue());
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void ZoomAnimation::setTargetZ(const bool isFirst)
{
    const double zoomLevel = m_currentZoomFrame->frameZoom;
    if (isFirst)
    {
        m_integratorZ->set(zoomLevel);
    }
    m_integratorZ->setTarget(zoomLevel);
}

//------------------------------------------------------------------------------

void ZoomAnimation::setTargetXY(const bool isFirst)
{
    const double centerX = m_currentZoomFrame->frameCenterX;
    const double centerY = m_currentZoomFrame->frameCenterY;
    UtmPoint utmP = GeoUtils::LonLat2Utm(centerX, centerY);
    if (isFirst)
    {
        m_integratorXY->set(utmP);
    }
    m_integratorXY->setTarget(utmP);
}

//------------------------------------------------------------------------------

bool ZoomAnimation::zoomHasChanged(const Timeline& timeline)
{
    if (timeline.isFirst())
    {
        m_currentZoomFrame = m_zoomAnimFrames.begin();
        return true;
    }

    tZoomAnimFrameIterator nextFrame = m_currentZoomFrame + 1;

    if (nextFrame != m_zoomAnimFrames.end() &&
        zoomHasChanged(timeline, nextFrame))
    {
        m_currentZoomFrame = nextFrame;
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------

bool ZoomAnimation::zoomHasChanged(const Timeline& timeline,
                                   tZoomAnimFrameIterator nextFrame)
{
    switch (m_zoomAnimType)
    {
    case 1:
    {
        const int current = timeline.getCurrentCount();
        const int all = timeline.getAllCount();
        return (current / (float) all) >= nextFrame->frameTime;
    }
    case 2:
    {
        const int currentGpsId = timeline.getCurrentTimelineObj().gpsid;
        return currentGpsId == nextFrame->gpsId;
    }
    case 3:
    {
        const std::string& currentTimestamp =
            timeline.getCurrentTimelineObj().timeString;
        return nextFrame->timestamp.compare(currentTimestamp) <= 0;
    }
    default:
        return false;
    }
}

//------------------------------------------------------------------------------
