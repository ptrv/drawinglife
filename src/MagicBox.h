/*==========================================================
 Copyright (c) avp::ptr, 2010
 ==========================================================*/

#ifndef _MAGICBOX_H_
#define _MAGICBOX_H_

#include "GpsData.h"

class MagicBox
{
public:

    MagicBox(double size, double padding);
    ~MagicBox();

    bool isInBox(const ofxPointd utmPoint);

    const ofxPointd getDrawablePoint(const UtmPoint& utmPoint);

    void setupBox(ofxPointd currUtm, double lon0);

    void updateBoxIfNeeded(const ofxPointd utmPoint);

    void updateBoxSize(double size);

    void addToBoxSize(double newSize);

    const ofxRectangled& getTheBox() const { return m_theBox; }
    const ofxRectangled getNormalizedBox();
    const ofxRectangled& getPaddedBox() const { return m_theBox; }
    const ofxRectangled getNormalizedPaddedBox();

private:

    bool isInPaddedBox(const ofxPointd utmPoint);

    // ist auch m_lon0
    ofxPointd m_centerUtm;

    ofxRectangled m_theBox;
    ofxRectangled m_paddedBox;

    double m_currentSize;

    double m_padding;

};

#endif // _MAGICBOX_H_
