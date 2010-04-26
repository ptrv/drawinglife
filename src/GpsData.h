/*=======================================================
 Copyright (c) avp::ptr, 2010
 =======================================================*/

#ifndef _GPSDATA_H_
#define _GPSDATA_H_

#include <vector>
#include <string>
#include "GpsSegment.h"

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
	void setGpsData(const std::vector<GpsSegment>& segments,
					double minLon,
					double maxLon,
					double minLat,
					double maxLat,
					const std::string& user);
	// -----------------------------------------------------------------------------
	// Get members.
	// -----------------------------------------------------------------------------
	const std::vector<GpsSegment>& getSegments() const { return m_segments; }
	double getMinLon() const { return m_minLon; }
	double getMaxLon() const { return m_maxLon; }
	double getMinLat() const { return m_minLat; }
	double getMaxLat() const { return m_maxLat; }

	// get UTM convertd min/max values
	double getMinUtmX() const { return m_minUtmX; }
	double getMaxUtmX() const { return m_maxUtmX; }
	double getMinUtmY() const { return m_minUtmY; }
	double getMaxUtmY() const { return m_maxUtmY; }

	// -----------------------------------------------------------------------------
	void clear();
	// -----------------------------------------------------------------------------
	// Get Gps point.
	// -----------------------------------------------------------------------------
	double getLongitude(int segmentIndex, int pointIndex);
	double getLatitude(int segmentIndex, int pointIndex);
	// -----------------------------------------------------------------------------
	double getUtmX(int segmentIndex, int pointIndex);
	double getUtmY(int segmentIndex, int pointIndex);
	// -----------------------------------------------------------------------------
	double getNormalizedUtmX(int segmentIndex, int pointIndex);
	double getNormalizedUtmY(int segmentIndex, int pointIndex);
	// -----------------------------------------------------------------------------
//	const GpsPoint& getCurrentPoint();
	// -----------------------------------------------------------------------------
	const std::string getGpsLocation(int segmentIndex, int pointIndex);
	const std::string getGpsLocationCurrent();
	// -----------------------------------------------------------------------------
	int getCurrentSegmentNum();
	int getCurrentPointNum();
	string getCurrentTimestamp();
	double getCurrentLongitude();
	double getCurrentLatitude();
//	double getCurrentElevation();
	// -----------------------------------------------------------------------------
	int getTotalGpsPoints();
	//------------------------------------------------------------------------------
	void setViewBounds(int screenWidth,
                        int screenHeight,
                        double viewXOffset,
                        double viewYOffset,
                        double viewMinDimension,
                        double viewPadding);

   	double getScaledUtmY(double utmY);
	double getScaledUtmX(double utmX);
	//---------------------------------------------------------------------------
    void draw(bool animated = true);
    void update();
    void reset();

private:
	void normalizeGpsPoints();
	void setMinMaxRatioUTM();
	void setMinMaxValuesUTM();

    int m_gpsDataId;
	std::vector<GpsSegment> m_segments;
	std::string m_user;
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

};


#endif // _GPSDATA_H_
