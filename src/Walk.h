/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/


class Walk 
{
public:
	Walk();
	~Walk();

	//------------------------------------------------------------------------------
    /**
	 * \brief Set scaling values GpsPoints.
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
	//---------------------------------------------------------------------------
    /**
	 * \brief Draw function for GpsData.
	 * \param animated if true GpsPoints are drawn animated.
	 */
    void draw(bool animated = true);
    /**
	 * \brief Counts through all GpsPoints.
	 */
    void update();
    /**
	 * \brief Reset counter.
	 */
    void reset();
	// -----------------------------------------------------------------------------
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
	
    static void setGlobalValues(double minX,
                                double maxX,
                                double minY,
                                double maxY,
                                double lon0);
	
    static void setDotSize(float dotSize) { m_dotSize = dotSize; }
	
private:
	
	static double maxDrawX;
    static double minDrawX;
    static double maxDrawY;
    static double minDrawY;

	static float m_dotSize;

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

};