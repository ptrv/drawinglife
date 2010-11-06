/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#ifndef _WALK_H_
#define _WALK_H_

#include <string>
#include "GpsView.h"
#include "GpsData.h"
#include "MagicBox.h"

class Walk : public GpsView
{
public:
	Walk(ofColor dotColor, bool magicBoxEnabled=true);
	virtual ~Walk();

	//------------------------------------------------------------------------------

	/**
	 * \brief
	 * \param screenWidth
	 * \param screenHeight
	 * \param viewXOffset
	 * \param viewYOffset
	 * \param viewMinDimension
	 * \param viewPadding
	 */
	void setViewBounds(int screenWidth,
					   int screenHeight,
					   double viewXOffset,
					   double viewYOffset,
					   double viewMinDimension,
					   double viewPadding);

    /**
	 * \brief Scale UTM Y to application window size.
	 * \param utmY UTM Y value to scale.
	 * \return scaled UTM Y value.
	 */
   	double getScaledUtmY(double utmY);
    /**
	 * \brief Scale UTM X to application window size.
	 * \param utmX UTM X value to scale.
	 * \return scaled UTM X value.
	 */
	double getScaledUtmX(double utmX);
    /**
	 * \brief Draw function for GpsData.
	 */
    void draw();
    /**
	 * \brief Draw function for GpsData.
	 */
    void drawAll();
    /**
	 * \brief Counts through all GpsPoints.
	 */
    void update();
    /**
	 * \brief Counts to next segment.
	 */
    void updateToNextSegment();
    /**
	 * \brief Counts to previous segment.
	 */
    void updateToPreviousSegment();
    /**
	 * \brief Reset counter.
	 */
    void reset();
	/**
	 * \brief Get current GpsPoint information.
	 * \return formatted string with info.
	 */
    const std::string& getCurrentGpsInfoDebug();
    /**
	 * \brief Get current GpsPoint information.
	 * \return formatted string with info.
	 */
    const std::string& getCurrentGpsInfo();
    /**
	 * \brief Get current Gps location.
	 * \return location string for current GpsPoint.
	 */
	const std::string getGpsLocationCurrent();
    /**
	 * \brief Get current GpsSegment number.
	 * \return index of current GpsSegment.
	 */
	int getCurrentSegmentNum();
    /**
	 * \brief Get current GpsPoint number.
	 * \return index of current GpsSegment.
	 */
	int getCurrentPointNum();
    /**
	 * \brief Get timestamp for current GpsPoint.
	 * \return timestamp string of current GpsPoint.
	 */
	std::string getCurrentTimestamp();
    /**
	 * \brief Get current logitude for GpsPoint.
	 * \return longitude of current GpsPoint.
	 */
	double getCurrentLongitude();
    /**
	 * \brief Get current latitude for GpsPoint.
	 * \return latitude of current GpsPoint.
	 */
	double getCurrentLatitude();
    /**
	 * \brief Get current elevation for GpsPoint.
	 * \return elevation of current GpsPoint.
	 */
	double getCurrentElevation();
    /**
	 * \brief Get current UTM X for GpsPoint.
	 * \return UTM X of current GpsPoint.
	 */
	double getCurrentUtmX();
    /**
	 * \brief Get current UTM Y for GpsPoint.
	 * \return UTM Y of current GpsPoint.
	 */
	double getCurrentUtmY();

    static void setDotSize(float dotSize) { m_dotSize = dotSize; }

    static void setTrackAlpha(int dotAlpha) { m_dotAlpha = dotAlpha; }

//    void setDotColors();

    void setCurrentPointImage(ofImage img);

	void setMagicBox(MagicBox* magicBox);

	void setMagicBoxStatic(MagicBox* magicBox);

	void toggleTraced();

private:

	static double maxDrawX;
    static double minDrawX;
    static double maxDrawY;
    static double minDrawY;

	static float m_dotSize;
    static int m_dotAlpha;

	int m_currentGpsPoint;
	int m_currentGpsSegment;
	int m_currentPoint;
	bool m_firstPoint;

	int m_screenWidth;
	int m_screenHeight;
	double m_viewXOffset;
	double m_viewYOffset;
	double m_viewMinDimension;
	double m_viewPadding;

	std::string m_currentGpsPointInfoDebug;
    std::string m_currentGpsPointInfo;

    MagicBox* m_magicBox;

    ofColor m_dotColor;

    int m_maxPointsToDraw;

    ofImage m_image;

    bool m_currentPointIsImage;
	int m_imgOffsetX;
	int m_imgOffsetY;

	bool m_magicBoxEnabled;

	bool m_interactiveMode;
	bool m_drawTraced;

	ofColor m_currentSegColor;
};

#endif // _WALK_H_
