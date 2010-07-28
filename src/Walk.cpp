/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#include "Walk.h"

Walk::Walk()
{
}

Walk::~Walk()
{
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
	
//    m_lon0Global = lon0;
    //normalizeUtmPointsGlobal();
}


