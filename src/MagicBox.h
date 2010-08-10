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

    bool isInBox(const UtmPoint& utmPoint);

    const UtmPoint getDrawablePoint(const UtmPoint& utmPoint);

private:

    void alignViewBox();

    double m_center;
    double m_x;
    double m_y;
    double m_width;
    double m_height;
};

#endif // _MAGICBOX_H_
