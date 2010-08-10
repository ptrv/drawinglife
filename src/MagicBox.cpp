/*==========================================================
 Copyright (c) avp::ptr, 2010
 ==========================================================*/

#include "MagicBox.h"

MagicBox::MagicBox()
:
m_center(0.0),
m_x(0.0),
m_y(0.0),
m_width(0.0),
m_height(0.0)
{
    //ctor
}
//MagicBox::MagicBox(double x, double y, double w, double h)
//:
//m_center(0.0),
//m_x(x),
//m_y(y),
//m_width(w),
//m_height(h)
//{
//    //ctor
//}

MagicBox::~MagicBox()
{
    //dtor
}

bool MagicBox::isInBox(const UtmPoint& utmPoint)
{
    bool res = false;

    return res;
}

const UtmPoint MagicBox::getDrawablePoint(const UtmPoint& utmPoint)
{
    return UtmPoint();
}

void MagicBox::alignViewBox()
{

}
