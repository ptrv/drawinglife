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
class GpsData : public boost::enable_shared_from_this<GpsData>
{

public:
    GpsData(const AppSettings& settings);
	~GpsData();

    //--------------------------------------------------------------------------

    void setGpsData(const GpsSegmentVector& segments,
                    const ofxPoint<double>& minLonLat,
                    const ofxPoint<double>& maxLonLat,
					const std::string& user);

    void clear();

    //--------------------------------------------------------------------------

    const GpsSegmentVector& getSegments() const { return m_segments; }
    double getMinLon() const { return m_minLonLat.x; }
    double getMaxLon() const { return m_maxLonLat.x; }
    double getMinLat() const { return m_minLonLat.y; }
    double getMaxLat() const { return m_maxLonLat.y; }
    double getMinUtmX() const { return m_minUtm.x; }
    double getMaxUtmX() const { return m_maxUtm.x; }
    double getMinUtmY() const { return m_minUtm.y; }
    double getMaxUtmY() const { return m_maxUtm.y; }

    double getLongitude(size_t segmentIndex, size_t pointIndex) const;
    double getLatitude(size_t segmentIndex, size_t pointIndex) const;
    double getElevation(size_t segmentIndex, size_t pointIndex) const;
    double getUtmX(size_t segmentIndex, size_t pointIndex) const;
    double getUtmY(size_t segmentIndex, size_t pointIndex) const;
    UtmPoint getUtm(size_t segmentIndex, size_t pointIndex) const;

    double getNormalizedUtmX(size_t segmentIndex, size_t pointIndex) const;
    double getNormalizedUtmY(size_t segmentIndex, size_t pointIndex) const;
    UtmPoint getNormalizedUtm(size_t segmentIndex, size_t pointIndex) const;

    const std::string getGpsLocation(size_t segmentIndex, size_t pointIndex) const;

    int getTotalGpsPoints() const;

    static GpsPoint getGpsPoint(const ofxPoint<double>& utmP);

    const UtmDataVector& getUTMPoints() const { return m_utmPoints; }
    const UtmDataVector& getNormalizedUTMPoints() const
    { return m_normalizedUtmPoints; }
    const UtmDataVector& getNormalizedUTMPointsGlobal() const
    { return m_normalizedUtmPointsGlobal; }

	const std::string& getUser() const { return m_user; }
    const GpsDataIndexVector& getIndices() const { return m_indices; }

    //--------------------------------------------------------------------------

    void calculateUtmPoints();
    void calculateUtmPointsWithIndex();

    //--------------------------------------------------------------------------

    static void setGlobalValues(const ofxPoint<double>& minXY,
                                const ofxPoint<double>& maxXY);

    //--------------------------------------------------------------------------

    void normalizeUtmPointsGlobal();

private:

    typedef boost::function<double(const GpsPoint&)> tFnGetGpsData;
    double getData(size_t segmentIndex, size_t pointIndex,
                   const tFnGetGpsData& fnGetGpsData) const;

    tFnGetGpsData tGetLongitudeFn;
    tFnGetGpsData tGetLatitudeFn;
    tFnGetGpsData tGetElevationFn;

    typedef boost::function<double(const UtmPoint&)> tFnGetUtmData;
    double getUtmData(size_t segmentIndex, size_t pointIndex,
                      const UtmDataVector& utmVec,
                      const tFnGetUtmData& fnGetUtmData) const;

    tFnGetUtmData tGetUtmXFn;
    tFnGetUtmData tGetUtmYFn;

    //--------------------------------------------------------------------------

    void normalizeUtmPoints();
    void normalizeUtmPoints(UtmDataVector& utmDataVec);

    //--------------------------------------------------------------------------

    void setMinMaxRatioUTM();
	void setGlobalMinMaxRatioUTM();
	void setMinMaxValuesUTM();

    //--------------------------------------------------------------------------
    static ofxPoint<double> drawMaxima;
    static ofxPoint<double> drawMinima;

    static double m_lon0Global;

    const AppSettings& m_settings;
    int m_gpsDataId;
    GpsSegmentVector m_segments;
	std::string m_user;

    ofxPoint<double> m_minLonLat;
    ofxPoint<double> m_maxLonLat;
    ofxPoint<double> m_minUtm;
    ofxPoint<double> m_maxUtm;

    UtmDataVector m_utmPoints;
    UtmDataVector m_normalizedUtmPoints;
    UtmDataVector m_normalizedUtmPointsGlobal;

    GpsDataIndexVector m_indices;
};

#endif // _GPSDATA_H_
