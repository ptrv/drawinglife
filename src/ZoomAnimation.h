#ifndef ZOOMANIMATION_H
#define ZOOMANIMATION_H

#include "DrawingLifeIncludes.h"
#include "Integrator.h"

class ZoomAnimation
{
public:
    ZoomAnimation(const AppSettings& settings, const TimelineWeak timeline);

    void update(DrawingLifeApp& app);

private:

    bool zoomHasChanged(const Timeline& timeline);
    bool zoomHasChangedTime(const Timeline& timeline);
    bool zoomHasChangedId(const Timeline& timeline);
    bool zoomHasChangedTimestamp(const Timeline& timeline);

    const AppSettings& m_settings;
    const TimelineWeak m_timeline;

    boost::scoped_ptr<Integrator<double> > m_zoomIntegrator;
    boost::scoped_ptr<Integrator<ofxPoint<double> > > m_theIntegrator;

    static int m_sZoomFrameCount;

};

#endif // ZOOMANIMATION_H
