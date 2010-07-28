/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#include "DrawingLifeIncludes.h"
#include "Walk.h"

#if defined (WIN32)
#undef max
#undef min
#endif
#include <limits>


double Walk::maxDrawX = -std::numeric_limits<double>::max();
double Walk::minDrawX = std::numeric_limits<double>::max();
double Walk::maxDrawY = -std::numeric_limits<double>::max();
double Walk::minDrawY = std::numeric_limits<double>::max();

float Walk::m_dotSize = 3.0;

Walk::Walk()
:
m_gpsData(NULL),
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
m_currentGpsPointInfoDebug(""),
m_currentGpsPointInfo("")
{
}

Walk::~Walk()
{
}

// -----------------------------------------------------------------------------
// Counting through GpsSegments and GpsPoints
// -----------------------------------------------------------------------------
void Walk::update()
{
    if (m_gpsData->getTotalGpsPoints() > 0)
    {
        if (m_gpsData->getNormalizedUTMPoints().size() > 0)
        {
            if ((unsigned int)m_currentGpsSegment < m_gpsData->getNormalizedUTMPoints().size()-1)
            {
                if ((unsigned int)m_currentGpsPoint < m_gpsData->getNormalizedUTMPoints()[m_currentGpsSegment].size() - 1)
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
                if ((unsigned int)m_currentGpsPoint < m_gpsData->getNormalizedUTMPoints()[m_currentGpsSegment].size() - 1)
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

void Walk::reset()
{
    m_currentGpsPoint = 0;
    m_currentGpsSegment = 0;
    m_currentPoint = -1;
    m_firstPoint = true;
}
// -----------------------------------------------------------------------------

void Walk::draw(bool animated)
{
    if (animated)
    {
        if (m_gpsData->getNormalizedUTMPoints().size() > 0 && m_gpsData->getNormalizedUTMPointsGlobal()[m_currentGpsSegment].size() > 0)
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
                    pointEnd = (int)m_gpsData->getNormalizedUTMPointsGlobal()[i].size()-1;
                for (int j = 0; j <= pointEnd; ++j)
                {
                    glVertex2d(getScaledUtmX(m_gpsData->getNormalizedUTMPointsGlobal()[i][j].x),
                               getScaledUtmY(m_gpsData->getNormalizedUTMPointsGlobal()[i][j].y));
                }
                glEnd();
            }
            ofFill();
            ofSetColor(0, 255, 0, 127);
            ofCircle(getScaledUtmX(m_gpsData->getNormalizedUTMPointsGlobal()[m_currentGpsSegment][m_currentGpsPoint].x),
					 getScaledUtmY(m_gpsData->getNormalizedUTMPointsGlobal()[m_currentGpsSegment][m_currentGpsPoint].y), 5);
        }
    }
    else
    {
        // -----------------------------------------------------------------------------
        // Draw Gps data
        // -----------------------------------------------------------------------------
        ofNoFill();
        for (unsigned int i = 0; i < m_gpsData->getNormalizedUTMPoints().size(); ++i)
        {
            glBegin(GL_LINE_STRIP);
            for (unsigned int j = 0; j < m_gpsData->getNormalizedUTMPointsGlobal()[i].size(); ++j)
            {
				glVertex2d(getScaledUtmX(m_gpsData->getNormalizedUTMPointsGlobal()[i][j].x),
						   getScaledUtmY(m_gpsData->getNormalizedUTMPointsGlobal()[i][j].y));
            }
            glEnd();
        }
    }
}
// -----------------------------------------------------------------------------
// Set view bounds.
// -----------------------------------------------------------------------------
void Walk::setViewBounds(int screenWidth,
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


const std::string Walk::getGpsLocationCurrent()
{
    return m_gpsData->getGpsLocation(m_currentGpsSegment, m_currentGpsPoint);
}
int Walk::getCurrentSegmentNum()
{
    int segmentNum = 0;
    if(m_currentGpsSegment < (int)m_gpsData->getSegments().size())
    {
        if(m_currentGpsPoint < (int)m_gpsData->getSegments()[m_currentGpsSegment].getPoints().size())
        {
            segmentNum = m_gpsData->getSegments()[m_currentGpsSegment].getSegmentNum();
        }
    }
    return segmentNum;
}
int Walk::getCurrentPointNum()
{
    return m_currentPoint;
}
std::string Walk::getCurrentTimestamp()
{
    std::string timestamp = "";
	if (m_currentGpsSegment < (int)m_gpsData->getSegments().size())
	{
		if (m_currentGpsPoint < (int)m_gpsData->getSegments()[m_currentGpsSegment].getPoints().size())
		{
			timestamp = m_gpsData->getSegments()[m_currentGpsSegment].getPoints()[m_currentGpsPoint].getTimestamp();
		}
	}
	return timestamp;
	
}
double Walk::getCurrentLongitude()
{
    return m_gpsData->getLongitude(m_currentGpsSegment, m_currentGpsPoint);
}
double Walk::getCurrentLatitude()
{
    return m_gpsData->getLatitude(m_currentGpsSegment, m_currentGpsPoint);
}
double Walk::getCurrentElevation()
{
    return m_gpsData->getElevation(m_currentGpsSegment, m_currentGpsPoint);
}
double Walk::getCurrentUtmX()
{
    return m_gpsData->getUtmX(m_currentGpsSegment, m_currentGpsPoint);
}
double Walk::getCurrentUtmY()
{
    return m_gpsData->getUtmY(m_currentGpsSegment, m_currentGpsPoint);
}

// -----------------------------------------------------------------------------
// Scale to screen
// -----------------------------------------------------------------------------
double Walk::getScaledUtmX(double normalizedUtmX)
{
    return ( normalizedUtmX * (m_viewMinDimension - 2.0 * m_viewPadding) + m_viewXOffset);
}

double Walk::getScaledUtmY(double normalizedUtmY)
{
	//    return ( (lat - m_minUtmY) / (m_maxUtmY - m_minUtmY) * (m_viewMinDimension - 2.0 * m_viewPadding) + m_viewYOffset);
    // Flip y coordinates ??
    return m_screenHeight - ( normalizedUtmY * (m_viewMinDimension - 2.0 * m_viewPadding) + m_viewYOffset);
}

const std::string& Walk::getCurrentGpsInfoDebug()
{
    m_currentGpsPointInfoDebug  =	"Longitude         : " + ofToString(getCurrentLongitude(), 7) + "\n" +
	"Latitude          : " + ofToString(getCurrentLatitude(), 7) + "\n" +
	"Elevation         : " + ofToString(getCurrentElevation(), 7) + "\n" +
	"UTM X             : " + ofToString(getCurrentUtmX(), 7) + "\n" +
	"UTM Y             : " + ofToString(getCurrentUtmY(), 7) + "\n" +
	"Time              : " + getCurrentTimestamp() + "\n" +
	"Location          : " + getGpsLocationCurrent() + "\n" +
	"Central meridiam  : " + ofToString(m_gpsData->getLon0(), 7) + "\n" +
	"Min/Max latitude  : " + ofToString(m_gpsData->getMinLat(), 7) + " / " + ofToString(m_gpsData->getMaxLat(), 7) + "\n" +
	"Min/Max longitude : " + ofToString(m_gpsData->getMinLon(), 7) + " / " + ofToString(m_gpsData->getMaxLon(), 7) + "\n" +
	"Min/Max UTM X     : " + ofToString(m_gpsData->getMinUtmX(), 7) + " / " + ofToString(m_gpsData->getMaxUtmX(), 7) + "\n" +
	"Min/Max UTM Y     : " + ofToString(m_gpsData->getMinUtmY(), 7) + " / " + ofToString(m_gpsData->getMaxUtmY(), 7) + "\n" +
	"Currrent pt.      : " + ofToString(getCurrentPointNum()) + "\n" +
	"Segment nr.       : " + ofToString(getCurrentSegmentNum()) + "\n" +
	"Total pts.        : " + ofToString(m_gpsData->getTotalGpsPoints()) + "\n" +
	"Person            : " + m_gpsData->getUser();
	
    return m_currentGpsPointInfoDebug;
}

const std::string& Walk::getCurrentGpsInfo()
{
	if(m_gpsData->getTotalGpsPoints() != 0)
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

//void Walk::setGlobalValues(double minX,
//						   double maxX,
//						   double minY,
//						   double maxY,
//						   double lon0)
//{
//    minDrawX = minX;
//    maxDrawX = maxX;
//    minDrawY = minY;
//    maxDrawY = maxY;
//	
//	//    m_lon0Global = lon0;
//    //normalizeUtmPointsGlobal();
//}


