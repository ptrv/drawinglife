/*==========================================================
 Copyright (c) avp::ptr, 2010
 ==========================================================*/

#ifndef _MAGICBOX_H_
#define _MAGICBOX_H_

#include "GpsData.h"

class MagicBox
{
public:

    MagicBox(AppSettings* settings, double size, double padding);
    ~MagicBox();

    bool isInBox(const ofxPointd utmPoint);

    const ofxPointd getDrawablePoint(const UtmPoint& utmPoint);

    void setupBox(ofxPointd currUtm, double lon0);
    void setupBoxStatic(ofxPointd currUtm, double lon0, double width, double height);

    void updateBoxIfNeeded(const ofxPointd utmPoint);

    void updateBoxSize(double size);

    void addToBoxSize(double sizeToAdd);

    void setSize(double newSize);
    void setCenter(double x, double y);

    const ofxRectangled& getTheBox() const { return m_theBox; }
    const ofxRectangled getNormalizedBox();
    const ofxRectangled& getPaddedBox() const { return m_theBox; }
    const ofxRectangled getNormalizedPaddedBox();
    const ofxPointd& getCenter() const {return m_centerUtm; }
    const GpsPoint getCenterGps() const;
    const double getSize() const { return m_currentSize; }

    void toggleZoomLevel(unsigned int zoomLevel);

    void goUp(double val);
    void goDown(double val);
    void goLeft(double val);
    void goRight(double val);

private:

    bool isInPaddedBox(const ofxPointd utmPoint);

    void setBoxes();
    // ist auch m_lon0
    ofxPointd m_centerUtm;

    ofxRectangled m_theBox;
    ofxRectangled m_paddedBox;

    AppSettings* m_settings;
    double m_currentSize;

    double m_padding;

    static double m_zoomLevels[4];

    double m_defaultSize;

    static int m_boxNum;

    int m_boxId;
};

#endif // _MAGICBOX_H_
