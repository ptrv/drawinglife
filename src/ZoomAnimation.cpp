#include "DrawingLifeIncludes.h"
#include "DrawingLifeApp.h"
#include "GpsData.h"
#include "Timeline.h"

#include "ZoomAnimation.h"

#include "GeoUtils.h"

int ZoomAnimation::m_sZoomFrameCount = 0;

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

}

void ZoomAnimation::update(DrawingLifeApp& app)
{
    const TimelinePtr timeline = m_timeline.lock();
    if (!timeline)
    {
        return;
    }

    if (m_settings.isZoomAnimation())
    {
        if (zoomHasChanged(*timeline.get()))
        {
            const ZoomAnimFrame& zoomAnimFrame =
                    m_settings.getZoomAnimFrames()[m_sZoomFrameCount];
            const float zoomLevel = zoomAnimFrame.frameZoom;
            const double centerX = zoomAnimFrame.frameCenterX;
            const double centerY = zoomAnimFrame.frameCenterY;

            UtmPoint utmP = GeoUtils::LonLat2Utm(centerX, centerY);

            if (timeline->isFirst())
            {
                m_zoomIntegrator->set(zoomLevel);
                m_theIntegrator->set(utmP);
            }
            m_zoomIntegrator->setTarget(zoomLevel);
            m_theIntegrator->setTarget(utmP);

        }
    }
    m_zoomIntegrator->update();
    m_theIntegrator->update();

    if (m_zoomIntegrator->isTargeting() || m_theIntegrator->isTargeting())
    {
        const MagicBoxVector& magicBoxes = app.getMagicBoxVector();
        BOOST_FOREACH(MagicBoxPtr box, magicBoxes)
        {
            box->setSize(m_zoomIntegrator->getValue());
            box->setupBox(m_theIntegrator->getValue());
        }
    }
}

//------------------------------------------------------------------------------

bool ZoomAnimation::zoomHasChanged(const Timeline& timeline)
{
    if (timeline.isFirst())
    {
        m_sZoomFrameCount = 0;
        return true;
    }

    switch (m_settings.getZoomAnimationCriteria())
    {
        case 1:
            return zoomHasChangedTime(timeline);
        case 2:
            return zoomHasChangedId(timeline);
        case 3:
            return zoomHasChangedTimestamp(timeline);
        default:
            return false;
    }
}

//------------------------------------------------------------------------------

bool ZoomAnimation::zoomHasChangedId(const Timeline& timeline)
{
    const ZoomAnimFrameVec& zoomAnimFrames = m_settings.getZoomAnimFrames();

    if (m_sZoomFrameCount + 1 >= static_cast<int>(zoomAnimFrames.size()))
    {
        return false;
    }
    const int currentId = timeline.getCurrentTimelineObj().gpsid;
    const int zoomChangeId = zoomAnimFrames[m_sZoomFrameCount+1].gpsId;
    if (currentId == zoomChangeId)
    {
        ++m_sZoomFrameCount;
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------

bool ZoomAnimation::zoomHasChangedTimestamp(const Timeline& timeline)
{
    const ZoomAnimFrameVec& zoomAnimFrames = m_settings.getZoomAnimFrames();

    if (m_sZoomFrameCount+1 >= static_cast<int>(zoomAnimFrames.size()))
    {
        return false;
    }
    const string& currentTimestamp = timeline.getCurrentTimelineObj().timeString;
    const string& zoomChangeTimestamp =
            zoomAnimFrames[m_sZoomFrameCount+1].timestamp;
    if (zoomChangeTimestamp.compare(currentTimestamp) == 0)
    {
        ++m_sZoomFrameCount;
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------

bool ZoomAnimation::zoomHasChangedTime(const Timeline& timeline)
{
    const ZoomAnimFrameVec& zoomAnimFrames = m_settings.getZoomAnimFrames();

    const int current = timeline.getCurrentCount();
    const int all = timeline.getAllCount();

    int currIndex = 0;
    for (size_t i = 0; i < zoomAnimFrames.size(); ++i)
    {
        if ((current / (float)all) > zoomAnimFrames[i].frameTime)
        {
            currIndex = i;
        }
        else
        {
            break;
        }
    }
    if (m_sZoomFrameCount != currIndex)
    {
        ++m_sZoomFrameCount;
        if (m_sZoomFrameCount >= static_cast<int>(zoomAnimFrames.size()))
        {
            --m_sZoomFrameCount;
        }
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
