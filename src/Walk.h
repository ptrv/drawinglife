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
    Walk(const AppSettings& settings, ofColor dotColor, bool magicBoxEnabled=true);
	~Walk();

	//------------------------------------------------------------------------------

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
	 * \brief Get current Gps location.
	 * \return location string for current GpsPoint.
	 */
    const std::string getGpsLocationCurrent() const;
    /**
	 * \brief Get current GpsSegment number.
	 * \return index of current GpsSegment.
	 */
    int getCurrentSegmentNum() const;
    /**
	 * \brief Get current GpsPoint number.
	 * \return index of current GpsSegment.
	 */
    int getCurrentPointNum() const;
    /**
	 * \brief Get timestamp for current GpsPoint.
	 * \return timestamp string of current GpsPoint.
	 */
    std::string getCurrentTimestamp() const;
    /**
	 * \brief Get current logitude for GpsPoint.
	 * \return longitude of current GpsPoint.
	 */
    double getCurrentLongitude() const;
    /**
	 * \brief Get current latitude for GpsPoint.
	 * \return latitude of current GpsPoint.
	 */
    double getCurrentLatitude() const;
    /**
	 * \brief Get current elevation for GpsPoint.
	 * \return elevation of current GpsPoint.
	 */
    double getCurrentElevation() const;
    /**
	 * \brief Get current UTM X for GpsPoint.
	 * \return UTM X of current GpsPoint.
	 */
    double getCurrentUtmX() const;
    /**
	 * \brief Get current UTM Y for GpsPoint.
	 * \return UTM Y of current GpsPoint.
	 */
    double getCurrentUtmY() const;

    static void setDotSize(float dotSize) { m_dotSize = dotSize; }

    static void setTrackAlpha(int dotAlpha) { m_dotAlpha = dotAlpha; }

//    void setDotColors();

    void setCurrentPointImage(const ofImage& img, int alpha);

	/**
	 * \brief Sets the GpsData.
	 * \param gpsData Pointer to GpsData object.
	**/
    void setGpsData(const GpsDataWeak gpsData);

    void setMagicBox(MagicBoxWeak magicBox);

    void setMagicBoxStatic(MagicBoxWeak magicBox, double lat, double lon);

	void toggleTraced();

private:

    typedef boost::function<double(const GpsDataPtr, int, int)> tFuncGetCurrentDouble;
    double getCurrentThing(const tFuncGetCurrentDouble& fnGetCurrentDouble) const;

    void updateToSegment(bool prev);

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
