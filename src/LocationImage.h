#ifndef _LOCATIONIMAGE_H_
#define _LOCATIONIMAGE_H_

#include "DrawingLifeIncludes.h"
#include "MagicBox.h"
#include "DrawingLifeDrawable.h"

class LocationImage : public DrawingLifeDrawable
{
public:
    LocationImage(ofImageWeak image, const MagicBoxWeak magicBox,
                  LocationImageData lid);
    virtual ~LocationImage();

    void draw();

private:

    const MagicBoxWeak m_magicBox;
    LocationImageData m_lid;

    UtmPoint m_utm;

    ofImageWeak m_image;
};

#endif // _LOCATIONIMAGE_H_
