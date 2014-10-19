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

    Walk(const AppSettings& settings, ofColor dotColor);
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

    void drawPoints(const PointsAndColors& pts);

    void updateToSegment(const tWalkDirection direction);

    void drawBoxes();

    std::pair<int, int> calculateStartSegmentAndStartPoint(const GpsData& gpsData);

#ifdef USE_OPENGL_FIXED_FUNCTIONS
    void drawSpeedColor(double speed, bool& isInBox);
#else
    void drawSpeedColor(double speed, bool& isInBox, ofColor& currentColor);
#endif

    void drawCurrentPoint(const MagicBox& box, const UtmPoint& currentUtm);

    typedef boost::function<double(const GpsData&,int,int)> tFnGetCurrentDouble;
    double getCurrentDoubleValue(const tFnGetCurrentDouble& fnGetCurrentDouble) const;

    tFnGetCurrentDouble fnGetCurrentLongitude;
    tFnGetCurrentDouble fnGetCurrentLatitude;
    tFnGetCurrentDouble fnGetCurrentElevation;
    tFnGetCurrentDouble fnGetCurrentUtmX;
    tFnGetCurrentDouble fnGetCurrentUtmY;

    const AppSettings& m_settings;

    GpsDataWeak m_gpsData;

	static float m_dotSize;
    static int m_dotAlpha;

	int m_currentGpsPoint;
	int m_currentGpsSegment;
	int m_currentPoint;
	bool m_firstPoint;

    MagicBoxWeak m_magicBox;

    ofColor m_fgColor;
    ofColor m_dotColor;

    int m_maxPointsToDraw;

    ofImage m_image;

    bool m_currentPointIsImage;

	bool m_interactiveMode;
	bool m_drawTraced;

	ofColor m_currentSegColor;

	int m_imageAlpha;

#ifndef USE_OPENGL_FIXED_FUNCTIONS
    ofVbo m_vbo;
#endif
};

#endif // _WALK_H_
