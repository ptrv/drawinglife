/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#ifndef _WALK_H_
#define _WALK_H_

#include <string>
#include "GpsData.h"
#include "MagicBox.h"
#include "DrawingLifeDrawable.h"


class Walk : public DrawingLifeDrawable
{
public:

    enum tWalkDirection
    {
        FORWARD, BACKWARD
    };

    Walk(const AppSettings& settings, ofColor dotColor, bool magicBoxEnabled=true);
	~Walk();

    void update();
    void updateToNextSegment();
    void updateToPreviousSegment();

    void reset();

    void draw();
    void drawAll();

    const std::string getCurrentGpsLocation() const;
    int getCurrentSegmentNum() const;
    int getCurrentPointNum() const;
    std::string getCurrentTimestamp() const;
    double getCurrentLongitude() const;
    double getCurrentLatitude() const;
    double getCurrentElevation() const;
    double getCurrentUtmX() const;
    double getCurrentUtmY() const;

    void setGpsData(const GpsDataWeak gpsData);
    void setMagicBox(MagicBoxWeak magicBox);
    void setMagicBoxStatic(MagicBoxWeak magicBox, double lat, double lon);
    void setCurrentPointImage(const ofImage& img, int alpha);

    void toggleTraced();

    static void setDotSize(float dotSize) { m_dotSize = dotSize; }
    static void setTrackAlpha(int dotAlpha) { m_dotAlpha = dotAlpha; }

private:

    void updateToSegment(const tWalkDirection direction);

    void drawBoxes();

    void calculateStartSegmentAndStartPoint(int& startSeg, int& startPoint,
                              const GpsData& gpsData);

    void drawSpeedColor(double speed, bool& isInBox);

    void drawCurrentPoint(const MagicBox& box, const UtmPoint& currentUtm);

    typedef boost::function<double(const GpsDataPtr&,int,int)> tFnGetCurrentDouble;
    double getCurrentThing(const tFnGetCurrentDouble& fnGetCurrentDouble) const;

    const AppSettings& m_settings;

    GpsDataWeak m_gpsData;

	static float m_dotSize;
    static int m_dotAlpha;

	int m_currentGpsPoint;
	int m_currentGpsSegment;
	int m_currentPoint;
	bool m_firstPoint;

    MagicBoxWeak m_magicBox;

    ofColor m_dotColor;

    int m_maxPointsToDraw;

    ofImage m_image;

    bool m_currentPointIsImage;

	bool m_interactiveMode;
	bool m_drawTraced;

	ofColor m_currentSegColor;

	int m_imageAlpha;
};

#endif // _WALK_H_
