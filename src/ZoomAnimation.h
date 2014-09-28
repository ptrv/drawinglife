#ifndef ZOOMANIMATION_H
#define ZOOMANIMATION_H

#include "DrawingLifeIncludes.h"
#include "Integrator.h"

class ZoomAnimation
{
public:
    ZoomAnimation(const AppSettings& settings, const TimelineWeak timeline);

    void update(const MagicBoxVector& magicBoxes);

private:

    bool zoomHasChanged(const Timeline& timeline);

    typedef ZoomAnimFrameVec::const_iterator tZoomAnimFrameIterator;
    bool zoomHasChanged(const Timeline& timeline,
                        tZoomAnimFrameIterator nextFrame);

    const AppSettings& m_settings;
    const TimelineWeak m_timeline;

    boost::scoped_ptr<Integrator<double> > m_zoomIntegrator;
    boost::scoped_ptr<Integrator<ofxPoint<double> > > m_theIntegrator;

    tZoomAnimFrameIterator m_currentZoomFrame;

};

#endif // ZOOMANIMATION_H
