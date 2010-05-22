/*=======================================================
 Copyright (c) avp::ptr, 2010
 =======================================================*/

#ifndef _GPSDATA_H_
#define _GPSDATA_H_

#include <vector>
#include <string>
#include "GpsSegment.h"

struct UtmPoint
{
    double x;
    double y;
};

using namespace std;
/**
 * \brief Holds a vector with segments, user and min/max values for longitude/latitude.
 *
 */
class GpsData
{

public:
	GpsData();
	~GpsData();

	// -----------------------------------------------------------------------------
	// Set Gps data.
	// -----------------------------------------------------------------------------
	/**
	* \brief Set values of GpsData object.
	* \param segments vecor with GpsSegments.
	* \param minLon minimum longitude value.
	* \param maxLon maximum longitude value.
	* \param minLat minimum latitude value.
	* \param maxLat maximum  latitude value.
	* \param user string with user name.
	*/
	void setGpsData(const vector<GpsSegment>& segments,
					double minLon,
					double maxLon,
					double minLat,
					double maxLat,
					const string& user);
	// -----------------------------------------------------------------------------
	// Get members.
	// -----------------------------------------------------------------------------
	/**
	* \brief Get GpsSegment objects.
	* \return vector with GpsSegments.
	*/
	const vector<GpsSegment>& getSegments() const { return m_segments; }
	/**
	* \brief Get minimum longitude value.
	* \return minimum longitude value.
	*/
	double getMinLon() const { return m_minLon; }
	/**
	* \brief Get maximum longitude value.
	* \return maximum longitude value.
	*/
	double getMaxLon() const { return m_maxLon; }
	/**
	* \brief Get minimum latitude value.
	* \return minimum latitude value.
	*/
	double getMinLat() const { return m_minLat; }
	/**
	* \brief Get maximum latitude value.
	* \return maximum latitude value.
	*/
	double getMaxLat() const { return m_maxLat; }
	// get UTM convertd min/max values
	/**
	* \brief Get minimum UTM X value.
	* \return minimum UTM X value.
	*/
	double getMinUtmX() const { return m_minUtmX; }
	/**
	* \brief Get maximum UTM X value.
	* \return maximum UTM X value.
	*/
	double getMaxUtmX() const { return m_maxUtmX; }
	/**
	* \brief Get minimum UTM Y value.
	* \return minimum UTM Y value.
	*/
	double getMinUtmY() const { return m_minUtmY; }
	/**
	* \brief Get maximum UTM Y value.
	* \return maximum UTM Y value.
	*/
	double getMaxUtmY() const { return m_maxUtmY; }

	// -----------------------------------------------------------------------------
	/**
	* \brief Clear GpsData values.
	*/
	void clear();
	// -----------------------------------------------------------------------------
	// Get Gps point.
	// -----------------------------------------------------------------------------
	/**
	* \brief Get longitude for given GpsSegment and GpsPoint.
	* \param segmentIndex number of a GpsSegment.
	* \param pointIndex number of a GpsPoint.
	* \return longitude for given GpsSegment and GpsPoint.
	*/
	double getLongitude(int segmentIndex, int pointIndex);
	/**
	* \brief Get latitude for given GpsSegment and GpsPoint.
	* \param segmentIndex number of a GpsSegment.
	* \param pointIndex number of a GpsPoint.
	* \return latitude for given GpsSegment and GpsPoint.
	*/
	double getLatitude(int segmentIndex, int pointIndex);
	/**
	* \brief Get elevation for given GpsSegment and GpsPoint.
	* \param segmentIndex number of a GpsSegment.
	* \param pointIndex number of a GpsPoint.
	* \return elevation for given GpsSegment and GpsPoint.
	*/
	double getElevation(int segmentIndex, int pointIndex);
	// -----------------------------------------------------------------------------
	/**
	* \brief Get UTM X for given GpsSegment and GpsPoint.
	* \param segmentIndex number of a GpsSegment.
	* \param pointIndex number of a GpsPoint.
	* \return UTM X for given GpsSegment and GpsPoint.
	*/
	double getUtmX(int segmentIndex, int pointIndex);
	/**
	* \brief Get UTM Y for given GpsSegment and GpsPoint.
	* \param segmentIndex number of a GpsSegment.
	* \param pointIndex number of a GpsPoint.
	* \return UTM Y for given GpsSegment and GpsPoint.
	*/
	double getUtmY(int segmentIndex, int pointIndex);
	// -----------------------------------------------------------------------------
	/**
	* \brief Get normalized UTM X for given GpsSegment and GpsPoint.
	* \param segmentIndex number of a GpsSegment.
	* \param pointIndex number of a GpsPoint.
	* \return normalized UTM X for given GpsSegment and GpsPoint.
	*/
	double getNormalizedUtmX(int segmentIndex, int pointIndex);
	/**
	* \brief Get normalized UTM Y for given GpsSegment and GpsPoint.
	* \param segmentIndex number of a GpsSegment.
	* \param pointIndex number of a GpsPoint.
	* \return normalized UTM Y for given GpsSegment and GpsPoint.
	*/
	double getNormalizedUtmY(int segmentIndex, int pointIndex);
	// -----------------------------------------------------------------------------
	/**
	* \brief Get total number of GpsPoints.
	* \return number of all GpsPoints
	*/
	int getTotalGpsPoints();
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

    /**
    * \brief Get central meridian for projection.
    * \return central meridian double value.
    */
    double getProjectionCentralMeridian() const { return m_lon0; }
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
	const string getGpsLocationCurrent();
	// -----------------------------------------------------------------------------
    /**
    * \brief Get current GpsSegment number.
    * \return number of current GpsSegment.
    */
	int getCurrentSegmentNum();
    /**
    * \brief Get current GpsPoint number.
    * \return number of current GpsSegment.
    */
	int getCurrentPointNum();
    /**
    * \brief Get timestamp for current GpsPoint.
    * \return timestamp string of current GpsPoint.
    */
	string getCurrentTimestamp();
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

private:
	// -----------------------------------------------------------------------------
//	const GpsPoint& getCurrentPoint();
	// -----------------------------------------------------------------------------
	/**
	* \brief Get location for given GpsSegment and GpsPoint.
	* \param segmentIndex number of a GpsSegment.
	* \param pointIndex number of a GpsPoint.
	* \return location for given GpsSegment and GpsPoint.
	*/
	const string getGpsLocation(int segmentIndex, int pointIndex);

    /**
    * \brief Normalize all UtmPoints to a value between 0 and 1.
    */
	void normalizeUtmPoints();
    /**
    * \brief Set range of min/max values to be the same (square).
    */
	void setMinMaxRatioUTM();
    /**
    * \brief Set ÚTM min/max values for all GpsPoints.
    */
	void setMinMaxValuesUTM();

    /**
    * \brief Calculate UTM values for all GpsPoints.
    */
    void calculateUtmPoints();

    int m_gpsDataId;
	vector<GpsSegment> m_segments;
	string m_user;
	double m_minLon;
	double m_maxLon;
	double m_minLat;
	double m_maxLat;
	double m_minUtmX;
	double m_maxUtmX;
	double m_minUtmY;
	double m_maxUtmY;

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

	vector< vector<UtmPoint> > m_utmPoints;
	vector< vector<UtmPoint> > m_normalizedUtmPoints;
    double m_lon0;

    string m_currentGpsPointInfoDebug;
    string m_currentGpsPointInfo;
};

#endif // _GPSDATA_H_
