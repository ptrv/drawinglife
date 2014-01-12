#ifndef _LOCATIONIMAGE_H_
#define _LOCATIONIMAGE_H_

#include "DrawingLifeIncludes.h"
#include "MagicBox.h"
#include "DrawingLifeDrawable.h"

class LocationImage : public DrawingLifeDrawable
{
public:
    LocationImage(const MagicBox& magicBox, LocationImageData lid);
    virtual ~LocationImage();

    void draw();

private:

    const MagicBox& m_magicBox;
    LocationImageData m_lid;

    UtmPoint m_utm;

    ofImage m_image;
};

#endif // _LOCATIONIMAGE_H_
