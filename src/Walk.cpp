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

float Walk::m_dotSize = 2.0;
int Walk::m_dotAlpha = 127;

Walk::Walk(int maxPointsToDraw, ofColor dotColor, bool magicBoxEnabled)
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
m_currentGpsPointInfo(""),
m_magicBox(NULL),
m_maxPointsToDraw(maxPointsToDraw),
m_currentPointIsImage(false),
m_imgOffsetX(0),
m_imgOffsetY(0),
m_magicBoxEnabled(true),
m_interactiveMode(false),
m_drawTraced(true)
{
    //	m_dotColor.a = 127;
//	m_dotColor.a = m_dotAlpha;
	m_dotColor.a = dotColor.a;
	m_dotColor.r = dotColor.r;
	m_dotColor.g = dotColor.g;
	m_dotColor.b = dotColor.b;

	m_interactiveMode = AppSettings::instance().isInteractiveMode();
	m_drawTraced = AppSettings::instance().drawTraced();

    m_currentSegColor.r = AppSettings::instance().getColorInteractiveSegR();
    m_currentSegColor.g = AppSettings::instance().getColorInteractiveSegG();
    m_currentSegColor.b = AppSettings::instance().getColorInteractiveSegB();
    m_currentSegColor.a = AppSettings::instance().getColorInteractiveSegA();
}

Walk::~Walk()
{
    if(m_gpsData)
        m_gpsData = 0;
    if(m_magicBox)
        m_magicBox = 0;
	m_image.clear();
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

void Walk::updateToNextSegment()
{
    if (m_gpsData->getTotalGpsPoints() > 0)
    {
        if (m_gpsData->getNormalizedUTMPoints().size() > 0)
        {
            if ((unsigned int)m_currentGpsSegment < m_gpsData->getNormalizedUTMPoints().size()-1)
            {
                ++m_currentGpsSegment;
                m_currentGpsPoint = m_gpsData->getNormalizedUTMPoints()[m_currentGpsSegment].size()-1;
            }
            else
            {
                m_currentGpsPoint = 0;
                m_currentGpsSegment = 0;
                m_currentPoint = -1;
            }
        }
    }
}
void Walk::updateToPreviousSegment()
{
    if (m_gpsData->getTotalGpsPoints() > 0)
    {
        if (m_gpsData->getNormalizedUTMPoints().size() > 0)
        {
            if ((unsigned int)m_currentGpsSegment > 0)
            {
                --m_currentGpsSegment;
                m_currentGpsPoint = m_gpsData->getNormalizedUTMPoints()[m_currentGpsSegment].size()-1;
            }
            else
            {
                m_currentGpsPoint = 0;
                m_currentGpsSegment = 0;
                m_currentPoint = -1;
            }
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

void Walk::draw()
{
	if (m_gpsData->getNormalizedUTMPoints().size() > 0 && m_gpsData->getNormalizedUTMPointsGlobal()[m_currentGpsSegment].size() > 0)
	{
		// -----------------------------------------------------------------------------
		// Draw Gps data
		// -----------------------------------------------------------------------------

        if(!m_interactiveMode)
        {
            m_magicBox->updateBoxIfNeeded(ofxPointd(m_gpsData->getUTMPoints()[m_currentGpsSegment][m_currentGpsPoint].x,
                                                    m_gpsData->getUTMPoints()[m_currentGpsSegment][m_currentGpsPoint].y));
        }

        int startSeg, startPoint;
        if(m_maxPointsToDraw > 0)
        {
            if(m_maxPointsToDraw - m_currentPoint <= 0)
            {
                int startIndex = m_currentPoint-m_maxPointsToDraw;
                startSeg = m_gpsData->getIndices()[startIndex].gpsSegment;
                startPoint = m_gpsData->getIndices()[startIndex].gpsPoint;
            }
            else
            {
                startSeg = 0;
                startPoint = 0;
            }
        }
        else
        {
            startSeg = 0;
            startPoint = 0;
        }
        if(m_interactiveMode && !m_drawTraced)
        {
            startSeg = m_currentGpsSegment;
        }

//        ofSetLineWidth(3.0);
//        ofEnableSmoothing();
        for (int i = startSeg; i <= m_currentGpsSegment; ++i)
        {
            glBegin(GL_LINE_STRIP);
            int pointEnd;
            if (i == m_currentGpsSegment)
            {
                pointEnd = m_currentGpsPoint;
                if(m_interactiveMode && m_drawTraced)
                    ofSetColor(m_currentSegColor.r,
                               m_currentSegColor.g,
                               m_currentSegColor.b,
                               m_currentSegColor.a);
            }
            else
            {
                pointEnd = (int)m_gpsData->getNormalizedUTMPointsGlobal()[i].size()-1;
            }

            for (int j = startPoint; j <= pointEnd; ++j)
            {
                bool isInBox = m_magicBox->isInBox(ofxPointd(m_gpsData->getUTMPoints()[i][j].x, m_gpsData->getUTMPoints()[i][j].y));
                if(isInBox)
                {
                    ofxPointd tmp = m_magicBox->getDrawablePoint(m_gpsData->getUTMPoints()[i][j]);
                    glVertex2d(getScaledUtmX(tmp.x),
                               getScaledUtmY(tmp.y));
                }
            }
            glEnd();

            startPoint = 0;
        }
//        ofDisableSmoothing();

		ofxPointd tmp = m_magicBox->getDrawablePoint(m_gpsData->getUTMPoints()[m_currentGpsSegment][m_currentGpsPoint]);

		if (m_currentPointIsImage)
		{
			ofSetColor(0xffffff);
			m_image.draw(getScaledUtmX(tmp.x)-m_imgOffsetX,getScaledUtmY(tmp.y)-m_imgOffsetY);

		}
		else if(!m_interactiveMode)
		{
			ofFill();
			ofSetColor(m_dotColor.r, m_dotColor.g, m_dotColor.b, m_dotColor.a);
			ofCircle(getScaledUtmX(tmp.x),
					 getScaledUtmY(tmp.y), m_dotSize);
		}
	}

    // draw borders of bounding boxes.
    if(EXTRA_DEBUG_MODE)
    {
        ofNoFill();
        ofSetColor(255,0,0);

        double x = getScaledUtmX(m_magicBox->getNormalizedBox().x);
        double y = getScaledUtmY(m_magicBox->getNormalizedBox().y);
        double w = getScaledUtmX(m_magicBox->getNormalizedBox().width)-x;
        double h = getScaledUtmY(m_magicBox->getNormalizedBox().height)-y;

        ofRect(x, y , w, h);

        ofNoFill();
        ofSetColor(0,255,0);

        double xp = getScaledUtmX(m_magicBox->getNormalizedPaddedBox().x);
        double yp = getScaledUtmY(m_magicBox->getNormalizedPaddedBox().y);
        double wp = getScaledUtmX(m_magicBox->getNormalizedPaddedBox().width)-xp;
        double hp = getScaledUtmY(m_magicBox->getNormalizedPaddedBox().height)-yp;

        ofRect(xp, yp , wp, hp);
    }

}

// -----------------------------------------------------------------------------
// Draw all Gps data
// -----------------------------------------------------------------------------
void Walk::drawAll()
{
	ofNoFill();
	for (unsigned int i = 0; i < m_gpsData->getNormalizedUTMPoints().size(); ++i)
	{
		glBegin(GL_LINE_STRIP);

		for (unsigned int j = 0; j < m_gpsData->getNormalizedUTMPointsGlobal()[i].size(); ++j)
		{
            bool isInBox = m_magicBox->isInBox(ofxPointd(m_gpsData->getUTMPoints()[i][j].x, m_gpsData->getUTMPoints()[i][j].y));
            if(isInBox)
            {
                const ofxPointd& tmp = m_magicBox->getDrawablePoint(m_gpsData->getUTMPoints()[i][j]);
                glVertex2d(getScaledUtmX(tmp.x),
                           getScaledUtmY(tmp.y));
            }

//			glVertex2d(getScaledUtmX(m_gpsData->getNormalizedUTMPointsGlobal()[i][j].x),
//					   getScaledUtmY(m_gpsData->getNormalizedUTMPointsGlobal()[i][j].y));
		}
		glEnd();
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
// -----------------------------------------------------------------------------
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
	"Central meridian  : " + ofToString(m_gpsData->getLon0(), 7) + "\n" +
	"Meridian global   : " + ofToString(GpsData::getLon0Glogal(), 7) + "\n" +
	"Min/Max latitude  : " + ofToString(m_gpsData->getMinLat(), 7) + " / " + ofToString(m_gpsData->getMaxLat(), 7) + "\n" +
	"Min/Max longitude : " + ofToString(m_gpsData->getMinLon(), 7) + " / " + ofToString(m_gpsData->getMaxLon(), 7) + "\n" +
	"Min/Max UTM X     : " + ofToString(m_gpsData->getMinUtmX(), 7) + " / " + ofToString(m_gpsData->getMaxUtmX(), 7) + "\n" +
	"Min/Max UTM Y     : " + ofToString(m_gpsData->getMinUtmY(), 7) + " / " + ofToString(m_gpsData->getMaxUtmY(), 7) + "\n" +
	"Currrent pt.      : " + ofToString(getCurrentPointNum()) + "\n" +
	"Segment nr.       : " + ofToString(getCurrentSegmentNum()) + "\n" +
	"Total pts.        : " + ofToString(m_gpsData->getTotalGpsPoints()) + "\n" +
	"Viewbox center    : " + ofToString(m_magicBox->getCenter().x,7) + " / " + ofToString(m_magicBox->getCenter().y, 7) + "\n" +
    "Viewbox size      : " + ofToString(m_magicBox->getSize(),7) + "\n" +
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
//void Walk::setDotColors()
//{
////   	m_dotColor.a = m_dotAlpha;
////	m_dotColor.r = (int)ofRandom(30,255);
////	m_dotColor.g = (int)ofRandom(30,255);
////	m_dotColor.b = (int)ofRandom(30,255);
//
//}

void Walk::setGpsData(GpsData* gpsData)
{
    m_gpsData = 0;
    m_gpsData = gpsData;
}


void Walk::setMagicBox(MagicBox* magicBox)
{
    m_magicBox = 0;
    m_magicBox = magicBox;
    reset();
    ofxPointd tmpCoord;
    tmpCoord.x = m_gpsData->getUtmX(0,0);
    tmpCoord.y = m_gpsData->getUtmY(0,0);
    m_magicBox->setupBox(tmpCoord, GpsData::getLon0Glogal());

}
void Walk::setMagicBoxStatic(MagicBox* magicBox)
{
    m_magicBox = 0;
    m_magicBox = magicBox;
    reset();
    ofxPointd tmpCoord;
    tmpCoord.x = m_gpsData->getMinUtmX() + ((m_gpsData->getMaxUtmX() - m_gpsData->getMinUtmX()) / 2.0);
    tmpCoord.y = m_gpsData->getMinUtmY() + ((m_gpsData->getMaxUtmY() - m_gpsData->getMinUtmY()) / 2.0);
    m_magicBox->setupBoxStatic(tmpCoord,
                               GpsData::getLon0Glogal(),
                               (m_gpsData->getMaxUtmX() - m_gpsData->getMinUtmX()),
                               (m_gpsData->getMaxUtmY() - m_gpsData->getMinUtmY()));

}

void Walk::setCurrentPointImage(ofImage img)
{
    m_image = img;
	m_imgOffsetX = m_image.getWidth()/2;
	m_imgOffsetY = m_image.getHeight()/2;
    m_currentPointIsImage = true;
}

void Walk::toggleTraced()
{
    m_drawTraced = !m_drawTraced;
}
