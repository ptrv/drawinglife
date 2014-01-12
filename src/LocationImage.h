#ifndef _LOCATIONIMAGE_H_
#define _LOCATIONIMAGE_H_

#include "DrawingLifeIncludes.h"
#include "MagicBox.h"

class LocationImage
{
public:
    LocationImage(const MagicBox& magicBox, LocationImageData lid);
    virtual ~LocationImage();

//    void setup(double minX, double maxX, double minY, double maxY);
    void setViewBounds(double minDim, double padding,
                       const ofxPoint<double>& offset);
    void draw();

protected:
private:

   	double getScaledUtmY(double utmY);
	double getScaledUtmX(double utmX);

    const MagicBox& m_magicBox;
    LocationImageData m_lid;

    UtmPoint m_utm;

    double m_viewMinDimension;
    double m_viewPadding;
    ofxPoint<double> m_viewOffset;


    ofImage m_image;
};

#endif // _LOCATIONIMAGE_H_
