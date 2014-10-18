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

    void setTargetZ(bool isFirst);
    void setTargetXY(bool isFirst);

    bool zoomHasChanged(const Timeline& timeline);

    typedef ZoomAnimFrameVec::const_iterator tZoomAnimFrameIterator;
    bool zoomHasChanged(const Timeline& timeline,
                        tZoomAnimFrameIterator nextFrame);

    // const AppSettings& m_settings;
    const TimelineWeak m_timeline;

    boost::scoped_ptr<Integrator<double> > m_integratorZ;
    boost::scoped_ptr<Integrator<ofxPoint<double> > > m_integratorXY;

    const ZoomAnimFrameVec& m_zoomAnimFrames;
    tZoomAnimFrameIterator m_currentZoomFrame;

    int m_zoomAnimType;
    bool m_animateXY;
};

#endif // ZOOMANIMATION_H
