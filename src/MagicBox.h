/*==========================================================
 Copyright (c) avp::ptr, 2010
 ==========================================================*/

#ifndef _MAGICBOX_H_
#define _MAGICBOX_H_

#include "GpsData.h"

class MagicBox
{
public:

    MagicBox();
//    MagicBox(double x, double y, double w, double h);
    ~MagicBox();

    bool isInBox(const ofxPointd utmPoint);

    const ofxPointd getDrawablePoint(const UtmPoint& utmPoint);

    void setupBox(ofxPointd currUtm, double lon0);

    void updateBoxIfNeeded(const ofxPointd utmPoint);

    void draw();

    void addToBoxSize(double newSize);

    const ofxRectangled& getTheBox() const { return m_theBox; }
    const ofxRectangled getNormalizedBox();

private:

    // ist auch m_lon0
    ofxPointd m_centerGps;
    ofxPointd m_centerUtm;
    ofxPointd m_centerUtmNormalized;

    ofxRectangled m_theBox;

    double m_currentSize;

    double m_padding;

};

#endif // _MAGICBOX_H_
