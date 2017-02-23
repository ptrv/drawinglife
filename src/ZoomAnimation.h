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

    boost::scoped_ptr<Integrator> m_integratorZ;

    class IntegratorXY
    {
    public:
        IntegratorXY(double val, double damp, double attr);
        ofxPoint<double> getValue() const;
        void update();
        bool isTargeting() const;
        void set(ofxPoint<double> p);
        void setTarget(ofxPoint<double> p);
    private:
        Integrator m_x;
        Integrator m_y;
    };
    boost::scoped_ptr<IntegratorXY> m_integratorXY;

    const ZoomAnimFrameVec& m_zoomAnimFrames;
    tZoomAnimFrameIterator m_currentZoomFrame;

    int m_zoomAnimType;
    bool m_animateXY;
};

#endif // ZOOMANIMATION_H
