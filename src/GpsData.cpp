/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#include "DrawingLifeIncludes.h"
#include "GpsData.h"
#include "GeographicLib/TransverseMercatorExact.hpp"

#if defined (WIN32)
#undef max
#undef min
#endif
#include <limits>

using namespace GeographicLib;

double GpsData::maxDrawX = -numeric_limits<double>::max();
double GpsData::minDrawX = numeric_limits<double>::max();
double GpsData::maxDrawY = -numeric_limits<double>::max();
double GpsData::minDrawY = numeric_limits<double>::max();
double GpsData::m_lon0Global = 0.0;
float GpsData::m_dotSize = 3.0;

GpsData::GpsData()
:
m_gpsDataId(0),
m_user(""),
m_minLon(0.0),
m_maxLon(0.0),
m_minLat(0.0),
m_maxLat(0.0),
m_minUtmX(0.0),
m_maxUtmX(0.0),
m_minUtmY(0.0),
m_maxUtmY(0.0),
m_currentGpsPoint(0),
m_currentGpsSegment(0),
m_currentPoint(-1),
m_firstPoint(true),
m_screenWidth(0),
m_screenHeight(0),
m_viewXOffset(0.0),
m_viewYOffset(0.0),
m_viewMinDimension(0.0),
m_viewPadding(0.0),
m_lon0(0.0),
m_currentGpsPointInfoDebug(""),
m_currentGpsPointInfo("")
{
	m_segments.reserve(1000); // TODO good amount.
}

GpsData::~GpsData()
{
}


// -----------------------------------------------------------------------------
// Set Gps data.
// -----------------------------------------------------------------------------
void GpsData::setGpsData(const std::vector<GpsSegment>& segments,
						 double minLon,
						 double maxLon,
						 double minLat,
						 double maxLat,
						 const std::string& user)
{
	++m_gpsDataId;
	m_segments.clear();
	m_segments = segments;
	m_minLon = minLon;
	m_maxLon = maxLon;
	m_minLat = minLat;
	m_maxLat = maxLat;
    const TransverseMercatorExact& TMS = TransverseMercatorExact::UTM;
    Math::real minGamma, minK, maxGamma, maxK;
    // -----------------------------------------------------------------------------
    // Calculating central meridian for projection.
    m_lon0 = m_minLon + (m_maxLon-m_minLon)/2;
    // -----------------------------------------------------------------------------
    TMS.Forward(Math::real(m_lon0), m_minLat, m_minLon, m_minUtmX, m_minUtmY, minGamma, minK);
    TMS.Forward(Math::real(m_lon0), m_maxLat, m_maxLon, m_maxUtmX, m_maxUtmY, maxGamma, maxK);
	m_user = user;
	calculateUtmPoints();
    normalizeUtmPoints();
}
// -----------------------------------------------------------------------------
void GpsData::clear()
{
	m_gpsDataId = 0;
	m_segments.clear();
	m_minLon = 0.0;
	m_maxLon = 0.0;
	m_minLat = 0.0;
	m_maxLat = 0.0;
	m_minUtmX = 0.0;
	m_maxUtmX = 0.0;
	m_minUtmY = 0.0;
	m_maxUtmY = 0.0;
	m_user = "";
}
// -----------------------------------------------------------------------------
// Counting through GpsSegments and GpsPoints
// -----------------------------------------------------------------------------
void GpsData::update()
{
    if (getTotalGpsPoints() > 0)
    {
        if (m_normalizedUtmPoints.size() > 0)
        {
            if ((unsigned int)m_currentGpsSegment < m_normalizedUtmPoints.size()-1)
            {
                if ((unsigned int)m_currentGpsPoint < m_normalizedUtmPoints[m_currentGpsSegment].size() - 1)
                {
                    if (!m_firstPoint)
                        ++m_currentGpsPoint;
                    else
                        m_firstPoint = false;
                }
                else
                {
                    ++m_currentGpsSegment;
                    m_currentGpsPoint = 0;
                }
            }
            else
            {
                if ((unsigned int)m_currentGpsPoint < m_normalizedUtmPoints[m_currentGpsSegment].size() - 1)
                {
                    ++m_currentGpsPoint;
                }
                else//	void setMinMaxRatio();

                {
                    m_currentGpsPoint = 0;
                    m_currentGpsSegment = 0;
                    m_currentPoint = -1;
                }
            }
            ++m_currentPoint;
        }

    }
}

void GpsData::reset()
{
    m_currentGpsPoint = 0;
    m_currentGpsSegment = 0;
    m_currentPoint = -1;
    m_firstPoint = true;
}
// -----------------------------------------------------------------------------

void GpsData::draw(bool animated)
{
    if (animated)
    {
        if (m_normalizedUtmPoints.size() > 0 && m_normalizedUtmPointsGlobal[m_currentGpsSegment].size() > 0)
        {
            // -----------------------------------------------------------------------------
            // Draw Gps data
            // -----------------------------------------------------------------------------
            for (int i = 0; i <= m_currentGpsSegment; ++i)
            {
                glBegin(GL_LINE_STRIP);
                int pointEnd;
                if (i == m_currentGpsSegment)
                    pointEnd = m_currentGpsPoint;
                else
                    pointEnd = (int)m_normalizedUtmPointsGlobal[i].size()-1;
                for (int j = 0; j <= pointEnd; ++j)
                {
                    glVertex2d(getScaledUtmX(m_normalizedUtmPointsGlobal[i][j].x),
                               getScaledUtmY(m_normalizedUtmPointsGlobal[i][j].y));
                }
                glEnd();
            }
            ofFill();
            ofSetColor(0, 255, 0, 127);
            ofCircle(getScaledUtmX(m_normalizedUtmPointsGlobal[m_currentGpsSegment][m_currentGpsPoint].x),
					 getScaledUtmY(m_normalizedUtmPointsGlobal[m_currentGpsSegment][m_currentGpsPoint].y), 5);
        }
    }
    else
    {
        // -----------------------------------------------------------------------------
        // Draw Gps data
        // -----------------------------------------------------------------------------
        ofNoFill();
        for (unsigned int i = 0; i < m_normalizedUtmPoints.size(); ++i)
        {
            glBegin(GL_LINE_STRIP);
            for (unsigned int j = 0; j < m_normalizedUtmPointsGlobal[i].size(); ++j)
            {
				glVertex2d(getScaledUtmX(m_normalizedUtmPointsGlobal[i][j].x),
						   getScaledUtmY(m_normalizedUtmPointsGlobal[i][j].y));
            }
            glEnd();
        }
    }
}
// -----------------------------------------------------------------------------
// Set view bounds.
// -----------------------------------------------------------------------------
void GpsData::setViewBounds(int screenWidth,
                            int screenHeight,
                            double viewXOffset,
                            double viewYOffset,
                            double viewMinDimension,
                            double viewPadding)
{
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;
    m_viewXOffset = viewXOffset;
    m_viewYOffset = viewYOffset;
    m_viewMinDimension = viewMinDimension;
    m_viewPadding = viewPadding;
}


const std::string GpsData::getGpsLocationCurrent()
{
    return getGpsLocation(m_currentGpsSegment, m_currentGpsPoint);
}
int GpsData::getCurrentSegmentNum()
{
    int segmentNum = 0;
    if(m_currentGpsSegment < (int)m_segments.size())
    {
        if(m_currentGpsPoint < (int)m_segments[m_currentGpsSegment].getPoints().size())
        {
            segmentNum = m_segments[m_currentGpsSegment].getSegmentNum();
        }
    }
    return segmentNum;
}
int GpsData::getCurrentPointNum()
{
    return m_currentPoint;
}
std::string GpsData::getCurrentTimestamp()
{
    std::string timestamp = "";
	if (m_currentGpsSegment < (int)m_segments.size())
	{
		if (m_currentGpsPoint < (int)m_segments[m_currentGpsSegment].getPoints().size())
		{
			timestamp = m_segments[m_currentGpsSegment].getPoints()[m_currentGpsPoint].getTimestamp();
		}
	}
	return timestamp;

}
double GpsData::getCurrentLongitude()
{
    return getLongitude(m_currentGpsSegment, m_currentGpsPoint);
}
double GpsData::getCurrentLatitude()
{
    return getLatitude(m_currentGpsSegment, m_currentGpsPoint);
}
double GpsData::getCurrentElevation()
{
    return getElevation(m_currentGpsSegment, m_currentGpsPoint);
}
double GpsData::getCurrentUtmX()
{
    return getUtmX(m_currentGpsSegment, m_currentGpsPoint);
}
double GpsData::getCurrentUtmY()
{
    return getUtmY(m_currentGpsSegment, m_currentGpsPoint);
}

// -----------------------------------------------------------------------------
// Get Gps point.
// -----------------------------------------------------------------------------
double GpsData::getLongitude(int segmentIndex, int pointIndex)
{
	double longitude = -1000.0;
	if (segmentIndex < (int)m_segments.size())
	{
		if (pointIndex < (int)m_segments[segmentIndex].getPoints().size())
		{
			longitude = m_segments[segmentIndex].getPoints()[pointIndex].getLongitude();
		}
	}
	return longitude;
}

double GpsData::getLatitude(int segmentIndex, int pointIndex)
{
	double latitude = -1000.0;
	if (segmentIndex < (int)m_segments.size())
	{
		if (pointIndex < (int)m_segments[segmentIndex].getPoints().size())
		{
			latitude = m_segments[segmentIndex].getPoints()[pointIndex].getLatitude();
		}
	}
	return latitude;
}

double GpsData::getElevation(int segmentIndex, int pointIndex)
{
	double elevation = -1000.0;
	if (segmentIndex < (int)m_segments.size())
	{
		if (pointIndex < (int)m_segments[segmentIndex].getPoints().size())
		{
			elevation = m_segments[segmentIndex].getPoints()[pointIndex].getElevation();
		}
	}
	return elevation;
}
// -----------------------------------------------------------------------------

double GpsData::getUtmX(int segmentIndex, int pointIndex)
{
	double utmX = 0.0;
	if (segmentIndex < (int)m_utmPoints.size())
	{
		if (pointIndex < (int)m_utmPoints[segmentIndex].size())
		{
			utmX = m_utmPoints[segmentIndex][pointIndex].x;
		}
	}
	return utmX;
}

double GpsData::getUtmY(int segmentIndex, int pointIndex)
{
	double utmY = 0.0;
	if (segmentIndex < (int)m_utmPoints.size())
	{
		if (pointIndex < (int)m_utmPoints[segmentIndex].size())
		{
			utmY = m_utmPoints[segmentIndex][pointIndex].y;
		}
	}
	return utmY;
}

double GpsData::getNormalizedUtmX(int segmentIndex, int pointIndex)
{
	double utmX = 0.0;
	if (segmentIndex < (int)m_normalizedUtmPoints.size())
	{
		if (pointIndex < (int)m_normalizedUtmPoints[segmentIndex].size())
		{
			utmX = m_normalizedUtmPoints[segmentIndex][pointIndex].x;
		}
	}
	return utmX;
}

double GpsData::getNormalizedUtmY(int segmentIndex, int pointIndex)
{
	double utmY = 0.0;
	if (segmentIndex < (int)m_normalizedUtmPoints.size())
	{
		if (pointIndex < (int)m_normalizedUtmPoints[segmentIndex].size())
		{
			utmY = m_normalizedUtmPoints[segmentIndex][pointIndex].y;
		}
	}
	return utmY;
}

// -----------------------------------------------------------------------------
// Scale to screen
// -----------------------------------------------------------------------------
double GpsData::getScaledUtmX(double normalizedUtmX)
{
    return ( normalizedUtmX * (m_viewMinDimension - 2.0 * m_viewPadding) + m_viewXOffset);
}

double GpsData::getScaledUtmY(double normalizedUtmY)
{
	//    return ( (lat - m_minUtmY) / (m_maxUtmY - m_minUtmY) * (m_viewMinDimension - 2.0 * m_viewPadding) + m_viewYOffset);
    // Flip y coordinates ??
    return m_screenHeight - ( normalizedUtmY * (m_viewMinDimension - 2.0 * m_viewPadding) + m_viewYOffset);
}

//---------------------------------------------------------------------------
const std::string GpsData::getGpsLocation(int segmentIndex, int pointIndex)
{
	std::string loc = "";
	if (segmentIndex < (int)m_segments.size())
	{
		if (pointIndex < (int)m_segments[segmentIndex].getPoints().size())
		{
			loc = m_segments[segmentIndex].getPoints()[pointIndex].getLocation();
		}
	}
	return loc;
}
// -----------------------------------------------------------------------------
int GpsData::getTotalGpsPoints()
{
	int num = 0;
	for (unsigned int i = 0; i < m_segments.size(); ++i) {
		for (unsigned int j = 0; j < m_segments[i].getPoints().size(); ++j) {
			++num;
		}
	}
	return num;
}

// -----------------------------------------------------------------------------
// Set min/max aspect ratio with UTM values.
// -----------------------------------------------------------------------------
void GpsData::setMinMaxRatioUTM()
{
	setMinMaxValuesUTM();

	double minLon = this->getMinUtmX();
	double maxLon = this->getMaxUtmX();
	double minLat = this->getMinUtmY();
	double maxLat = this->getMaxUtmY();

	// Calculate horizontal and vertical range.
	double deltaLon = maxLon - minLon;
	double deltaLat = maxLat - minLat;

	// Aspect ratio is: width < height.
	if (deltaLon <	deltaLat)
	{
		m_minUtmX = minLon - (deltaLat - deltaLon)/2.0;
		m_maxUtmX = maxLon + (deltaLat - deltaLon)/2.0;
		m_minUtmY = minLat;
		m_maxUtmY = maxLat;
	}
	// Aspect ratio is: width > height.
	else if (deltaLon > deltaLat)
	{
		m_minUtmX = minLon;
		m_maxUtmX = maxLon;
		m_minUtmY = minLat - (deltaLon - deltaLat)/2.0;
		m_maxUtmY = maxLat + (deltaLon - deltaLat)/2.0;
	}
	// Aspect ratio is: height == width.
	else
	{
		m_minUtmX = minLon;
		m_maxUtmX = maxLon;
		m_minUtmY = minLat;
		m_maxUtmY = maxLat;
	}
}
void GpsData::setGlobalMinMaxRatioUTM()
{
	double minLon = minDrawX;
	double maxLon = maxDrawX;
	double minLat = minDrawY;
	double maxLat = maxDrawY;

	// Calculate horizontal and vertical range.
	double deltaLon = maxLon - minLon;
	double deltaLat = maxLat - minLat;

	// Aspect ratio is: width < height.
	if (deltaLon <	deltaLat)
	{
		minDrawX = minLon - (deltaLat - deltaLon)/2.0;
		maxDrawX = maxLon + (deltaLat - deltaLon)/2.0;
		minDrawY = minLat;
		maxDrawY = maxLat;
	}
	// Aspect ratio is: width > height.
	else if (deltaLon > deltaLat)
	{
		minDrawX = minLon;
		maxDrawX = maxLon;
		minDrawY = minLat - (deltaLon - deltaLat)/2.0;
		maxDrawY = maxLat + (deltaLon - deltaLat)/2.0;
	}
	// Aspect ratio is: height == width.
	else
	{
		minDrawX = minLon;
		maxDrawX = maxLon;
		minDrawY = minLat;
		maxDrawY = maxLat;
	}
}
// -----------------------------------------------------------------------------
// Normalize Utm points.
// -----------------------------------------------------------------------------
void GpsData::normalizeUtmPoints()
{
	setMinMaxRatioUTM();
	m_normalizedUtmPoints = m_utmPoints;
	for (unsigned int i = 0; i < m_utmPoints.size(); ++i) {
		for (unsigned int j = 0; j < m_utmPoints[i].size(); ++j)
		{
			double x = m_utmPoints[i][j].x;
			double y = m_utmPoints[i][j].y;
			x = (x - m_minUtmX) / (m_maxUtmX - m_minUtmX);
			y = (y - m_minUtmY) / (m_maxUtmY - m_minUtmY);
			m_normalizedUtmPoints[i][j].x = x;
			m_normalizedUtmPoints[i][j].y = y;
		}
	}
}
void GpsData::normalizeUtmPointsGlobal()
{
	setGlobalMinMaxRatioUTM();
	m_normalizedUtmPointsGlobal = m_utmPoints;
	for (unsigned int i = 0; i < m_utmPoints.size(); ++i) {
		for (unsigned int j = 0; j < m_utmPoints[i].size(); ++j)
		{
			double x = m_utmPoints[i][j].x;
			double y = m_utmPoints[i][j].y;
			x = (x - minDrawX) / (maxDrawX - minDrawX);
			y = (y - minDrawY) / (maxDrawY- minDrawY);
			m_normalizedUtmPointsGlobal[i][j].x = x;
			m_normalizedUtmPointsGlobal[i][j].y = y;
		}
	}
}

void GpsData::setMinMaxValuesUTM()
{
	double minX = numeric_limits<double>::max();
	double minY = numeric_limits<double>::max();
	// using -std::numeric_limits<double>::max() for smallest double
	// std::numeric_limits<double>::min() yields smallest positiv number.
	double maxX = -numeric_limits<double>::max();
	double maxY = -numeric_limits<double>::max();

	for (unsigned int i = 0; i < m_utmPoints.size(); ++i) {
		for (unsigned int j = 0; j < m_utmPoints[i].size(); ++j)
		{
			double x = m_utmPoints[i][j].x;
			double y = m_utmPoints[i][j].y;
			if (x < minX) minX = x;
			if (x > maxX) maxX = x;
			if (y < minY) minY = y;
			if (y > maxY) maxY = y;
		}
	}

    m_minUtmX = minX;
    m_minUtmY = minY;
    m_maxUtmX = maxX;
    m_maxUtmY = maxY;
}

void GpsData::calculateUtmPoints()
{
    const TransverseMercatorExact& TMS = TransverseMercatorExact::UTM;
    m_utmPoints.clear();
    m_utmPoints.reserve(m_segments.size());
    for(unsigned int i = 0; i < m_segments.size(); ++i)
    {
        std::vector<UtmPoint> utmVec;
        utmVec.reserve( m_segments[i].getPoints().size());
        for(unsigned int j = 0; j < m_segments[i].getPoints().size(); ++j)
        {
            Math::real gamma, k;
            UtmPoint utmP;
            TMS.Forward(Math::real(m_lon0),
                        m_segments[i].getPoints()[j].getLatitude(),
                        m_segments[i].getPoints()[j].getLongitude(),
                        utmP.x, utmP.y, gamma, k);
            utmVec.push_back(utmP);
        }
        m_utmPoints.push_back(utmVec);
    }
}
void GpsData::calculateUtmPointsGlobalLon()
{
    const TransverseMercatorExact& TMS = TransverseMercatorExact::UTM;
    m_utmPoints.clear();
    m_utmPoints.reserve(m_segments.size());
    for(unsigned int i = 0; i < m_segments.size(); ++i)
    {
        std::vector<UtmPoint> utmVec;
        utmVec.reserve( m_segments[i].getPoints().size());
        for(unsigned int j = 0; j < m_segments[i].getPoints().size(); ++j)
        {
            Math::real gamma, k;
            UtmPoint utmP;
            TMS.Forward(Math::real(m_lon0Global),
                        m_segments[i].getPoints()[j].getLatitude(),
                        m_segments[i].getPoints()[j].getLongitude(),
                        utmP.x, utmP.y, gamma, k);
            utmVec.push_back(utmP);
        }
        m_utmPoints.push_back(utmVec);
    }
}

const std::string& GpsData::getCurrentGpsInfoDebug()
{
    m_currentGpsPointInfoDebug  =	"Longitude         : " + ofToString(getCurrentLongitude(), 7) + "\n" +
                                    "Latitude          : " + ofToString(getCurrentLatitude(), 7) + "\n" +
                                    "Elevation         : " + ofToString(getCurrentElevation(), 7) + "\n" +
                                    "UTM X             : " + ofToString(getCurrentUtmX(), 7) + "\n" +
                                    "UTM Y             : " + ofToString(getCurrentUtmY(), 7) + "\n" +
                                    "Time              : " + getCurrentTimestamp() + "\n" +
                                    "Location          : " + getGpsLocationCurrent() + "\n" +
                                    "Central meridiam  : " + ofToString(m_lon0, 7) + "\n" +
                                    "Min/Max latitude  : " + ofToString(m_minLat, 7) + " / " + ofToString(m_maxLat, 7) + "\n" +
                                    "Min/Max longitude : " + ofToString(m_minLon, 7) + " / " + ofToString(m_maxLon, 7) + "\n" +
                                    "Min/Max UTM X     : " + ofToString(m_minUtmX, 7) + " / " + ofToString(m_maxUtmX, 7) + "\n" +
                                    "Min/Max UTM Y     : " + ofToString(m_minUtmY, 7) + " / " + ofToString(m_maxUtmY, 7) + "\n" +
                                    "Currrent pt.      : " + ofToString(getCurrentPointNum()) + "\n" +
                                    "Segment nr.       : " + ofToString(getCurrentSegmentNum()) + "\n" +
                                    "Total pts.        : " + ofToString(getTotalGpsPoints()) + "\n" +
                                    "Person            : " + m_user;

    return m_currentGpsPointInfoDebug;
}

const std::string& GpsData::getCurrentGpsInfo()
{
	if(getTotalGpsPoints() != 0)
	{
		std::string timeString = getCurrentTimestamp();
		int year, month, day, hour, min, sec;
		sscanf(timeString.c_str(), "%d-%d-%dT%d:%d:%dZ", &year, &month, &day, &hour, &min, &sec);
		char buf[25];
		sprintf(buf, "%02d.%02d.%d %02d:%02d:%02d", day, month, year, hour, min, sec);
		m_currentGpsPointInfo = getGpsLocationCurrent() + " " + string(buf);
    }
	return m_currentGpsPointInfo;


}

void GpsData::setGlobalValues(double minX,
                              double maxX,
                              double minY,
                              double maxY,
                              double lon0)
{
    minDrawX = minX;
    maxDrawX = maxX;
    minDrawY = minY;
    maxDrawY = maxY;

    m_lon0Global = lon0;
    //normalizeUtmPointsGlobal();
}
