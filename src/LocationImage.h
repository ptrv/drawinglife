#ifndef _LOCATIONIMAGE_H_
#define _LOCATIONIMAGE_H_

#include "DrawingLifeIncludes.h"
#include "MagicBox.h"

class LocationImage
{
public:
    LocationImage(MagicBox* magicBox, LocationImageData lid);
    virtual ~LocationImage();

//    void setup(double minX, double maxX, double minY, double maxY);
    void setViewBounds(double minDim, double padding, double offsetX, double offsetY);
    void draw();

protected:
private:

   	double getScaledUtmY(double utmY);
	double getScaledUtmX(double utmX);

    MagicBox* m_magicBox;
    LocationImageData m_lid;

    UtmPoint m_utm;

    double m_viewMinDimension;
    double m_viewPadding;
    double m_viewXOffset;
    double m_viewYOffset;


    ofImage m_image;
};

#endif // _LOCATIONIMAGE_H_
