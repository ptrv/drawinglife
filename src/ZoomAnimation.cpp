#include "ZoomAnimation.h"

#include "GpsData.h"
#include "Timeline.h"
#include "MagicBox.h"
#include "GeoUtils.h"


ZoomAnimation::ZoomAnimation(const AppSettings& settings,
                             const TimelineWeak timeline)
    : m_settings(settings), m_timeline(timeline)
{
    const double damp = m_settings.getZoomAnimationDamp();
    const double attr = m_settings.getZoomAnimationAttraction();
    const double dampCenter = m_settings.getZoomAnimationDampCenter();
    const double attrCenter = m_settings.getZoomAnimationAttractionCenter();

    m_zoomIntegrator.reset(new Integrator<double>(0.0f, damp, attr));
    m_theIntegrator.reset(new Integrator<ofxPoint<double> >(0.0f,
                                                            dampCenter,
                                                            attrCenter));

    m_currentZoomFrame = m_settings.getZoomAnimFrames().begin();
}

void ZoomAnimation::update(const MagicBoxVector& magicBoxes)
{
    if (!m_settings.isZoomAnimation())
    {
        return;
    }

    if (const TimelinePtr timeline = m_timeline.lock())
    {
        if (zoomHasChanged(*timeline))
        {
            const float zoomLevel = m_currentZoomFrame->frameZoom;
            const double centerX = m_currentZoomFrame->frameCenterX;
            const double centerY = m_currentZoomFrame->frameCenterY;

            UtmPoint utmP = GeoUtils::LonLat2Utm(centerX, centerY);

            if (timeline->isFirst())
            {
                m_zoomIntegrator->set(zoomLevel);
                m_theIntegrator->set(utmP);
            }
            m_zoomIntegrator->setTarget(zoomLevel);
            m_theIntegrator->setTarget(utmP);

        }

        m_zoomIntegrator->update();
        m_theIntegrator->update();

        if (m_zoomIntegrator->isTargeting() || m_theIntegrator->isTargeting())
        {
            BOOST_FOREACH(MagicBoxPtr box, magicBoxes)
            {
                box->setSize(m_zoomIntegrator->getValue());
                box->setupBox(m_theIntegrator->getValue());
            }
        }
    }
}

//------------------------------------------------------------------------------

bool ZoomAnimation::zoomHasChanged(const Timeline& timeline)
{
    const ZoomAnimFrameVec& zoomAnimFrames = m_settings.getZoomAnimFrames();

    if (timeline.isFirst())
    {
        m_currentZoomFrame = zoomAnimFrames.begin();
        return true;
    }

    tZoomAnimFrameIterator nextFrame = m_currentZoomFrame + 1;

    if (nextFrame != zoomAnimFrames.end() &&
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
    switch (m_settings.getZoomAnimationCriteria())
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
        return nextFrame->timestamp.compare(currentTimestamp) == 0;
    }
    default:
        return false;
    }
}

//------------------------------------------------------------------------------
