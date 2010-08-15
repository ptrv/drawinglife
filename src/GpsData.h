/*=======================================================
 Copyright (c) avp::ptr, 2010
 =======================================================*/

#ifndef _GPSDATA_H_
#define _GPSDATA_H_

#include "DrawingLifeIncludes.h"
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
	/**
	* \brief Set values of GpsData object.
	* \param segments vecor with GpsSegments.
	* \param minLon minimum longitude value.
	* \param maxLon maximum longitude value.
	* \param minLat minimum latitude value.
	* \param maxLat maximum  latitude value.
	* \param user string with user name.
	*/
	void setGpsData(const std::vector<GpsSegment>& segments,
					double minLon,
					double maxLon,
					double minLat,
					double maxLat,
					const std::string& user);
	// -----------------------------------------------------------------------------
	// Get members.
	// -----------------------------------------------------------------------------
	/**
	* \brief Get GpsSegment objects.
	* \return vector with GpsSegments.
	*/
	const std::vector<GpsSegment>& getSegments() const { return m_segments; }
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
	* \param segmentIndex index of a GpsSegment.
	* \param pointIndex index of a GpsPoint.
	* \return longitude for given GpsSegment and GpsPoint.
	*/
	double getLongitude(int segmentIndex, int pointIndex);
	/**
	* \brief Get latitude for given GpsSegment and GpsPoint.
	* \param segmentIndex index of a GpsSegment.
	* \param pointIndex index of a GpsPoint.
	* \return latitude for given GpsSegment and GpsPoint.
	*/
	double getLatitude(int segmentIndex, int pointIndex);
	/**
	* \brief Get elevation for given GpsSegment and GpsPoint.
	* \param segmentIndex index of a GpsSegment.
	* \param pointIndex index of a GpsPoint.
	* \return elevation for given GpsSegment and GpsPoint.
	*/
	double getElevation(int segmentIndex, int pointIndex);
	// -----------------------------------------------------------------------------
	/**
	* \brief Get UTM X for given GpsSegment and GpsPoint.
	* \param segmentIndex index of a GpsSegment.
	* \param pointIndex index of a GpsPoint.
	* \return UTM X for given GpsSegment and GpsPoint.
	*/
	double getUtmX(int segmentIndex, int pointIndex);
	/**
	* \brief Get UTM Y for given GpsSegment and GpsPoint.
	* \param segmentIndex index of a GpsSegment.
	* \param pointIndex index of a GpsPoint.
	* \return UTM Y for given GpsSegment and GpsPoint.
	*/
	double getUtmY(int segmentIndex, int pointIndex);
	// -----------------------------------------------------------------------------
	/**
	* \brief Get normalized UTM X for given GpsSegment and GpsPoint.
	* \param segmentIndex index of a GpsSegment.
	* \param pointIndex index of a GpsPoint.
	* \return normalized UTM X for given GpsSegment and GpsPoint.
	*/
	double getNormalizedUtmX(int segmentIndex, int pointIndex);
	/**
	* \brief Get normalized UTM Y for given GpsSegment and GpsPoint.
	* \param segmentIndex index of a GpsSegment.
	* \param pointIndex index of a GpsPoint.
	* \return normalized UTM Y for given GpsSegment and GpsPoint.
	*/
	double getNormalizedUtmY(int segmentIndex, int pointIndex);
	// -----------------------------------------------------------------------------
	/**
	* \brief Get total number of GpsPoints.
	* \return number of all GpsPoints
	*/
	int getTotalGpsPoints() const;
	//------------------------------------------------------------------------------

    /**
    * \brief Get central meridian for projection.
    * \return central meridian double value.
    */
    double getProjectionCentralMeridian() const { return m_lon0; }
    /**
    * \brief Normalize all UtmPoints to a value between 0 and 1 with global min/max values.
    */
	void normalizeUtmPointsGlobal();
    /**
    * \brief Calculate UTM values for all GpsPoints with global lon0.
    */
    void calculateUtmPointsGlobalLon();

	static void setGlobalValues(double minX,
                                double maxX,
                                double minY,
                                double maxY,
                                double lon0);

	const std::vector< std::vector<UtmPoint> >& getUTMPoints() const { return m_utmPoints; };
	const std::vector< std::vector<UtmPoint> >& getNormalizedUTMPoints() const { return m_normalizedUtmPoints; };
	const std::vector< std::vector<UtmPoint> >& getNormalizedUTMPointsGlobal() const { return m_normalizedUtmPointsGlobal; };


	// -----------------------------------------------------------------------------
	/**
	* \brief Get location for given GpsSegment and GpsPoint.
	* \param segmentIndex index of a GpsSegment.
	* \param pointIndex index of a GpsPoint.
	* \return location for given GpsSegment and GpsPoint.
	*/
	const std::string getGpsLocation(int segmentIndex, int pointIndex);

	double getLon0() const { return m_lon0; }

	static double getLon0Glogal() { return GpsData::m_lon0Global; }

	const std::string& getUser() const { return m_user; }

    /**
    * \brief Calculate UTM values for all GpsPoints.
    */
    void calculateUtmPoints(double lon0);

    const std::vector<GpsDataIndex >& getIndices() const { return m_indices; };

private:
    /**
    * \brief Normalize all UtmPoints to a value between 0 and 1.
    */
	void normalizeUtmPoints();
    /**
    * \brief Set range of min/max values to be the same (square).
    */
	void setMinMaxRatioUTM();
    /**
    * \brief Set range of global min/max values to be the same (square).
    */
	void setGlobalMinMaxRatioUTM();
    /**
    * \brief Set UTM min/max values for all GpsPoints.
    */
	void setMinMaxValuesUTM();
    /**
    * \brief Calculate UTM values for all GpsPoints.
    */
    void calculateUtmPoints();

    static double maxDrawX;
    static double minDrawX;
    static double maxDrawY;
    static double minDrawY;

    static double m_lon0Global;

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

	std::vector< std::vector<UtmPoint> > m_utmPoints;
	std::vector< std::vector<UtmPoint> > m_normalizedUtmPoints;
	std::vector< std::vector<UtmPoint> > m_normalizedUtmPointsGlobal;
    double m_lon0;

    std::vector<GpsDataIndex > m_indices;
};

#endif // _GPSDATA_H_
