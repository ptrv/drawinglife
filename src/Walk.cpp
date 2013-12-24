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

Walk::Walk(const AppSettings& settings, ofColor dotColor, bool magicBoxEnabled)
:
m_settings(settings),
m_gpsData(0),
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
m_magicBox(0),
m_currentPointIsImage(false),
m_interactiveMode(false),
m_drawTraced(true),
m_imageAlpha(255)
{
    m_maxPointsToDraw = m_settings.getWalkLength();
    //	m_dotColor.a = 127;
//	m_dotColor.a = m_dotAlpha;
	m_dotColor.a = dotColor.a;
	m_dotColor.r = dotColor.r;
	m_dotColor.g = dotColor.g;
	m_dotColor.b = dotColor.b;

    m_interactiveMode = m_settings.isInteractiveMode();
    m_drawTraced = m_settings.drawTraced();

    m_currentSegColor.r = m_settings.getColorInteractiveSegR();
    m_currentSegColor.g = m_settings.getColorInteractiveSegG();
    m_currentSegColor.b = m_settings.getColorInteractiveSegB();
    m_currentSegColor.a = m_settings.getColorInteractiveSegA();
}

Walk::~Walk()
{
    if(m_gpsData)
        m_gpsData = 0;
    if(m_magicBox)
        m_magicBox = 0;
	m_image.clear();

    ofLogVerbose("Walk", "destroying");
}

// -----------------------------------------------------------------------------
// Counting through GpsSegments and GpsPoints
// -----------------------------------------------------------------------------
void Walk::update()
{
        if ((unsigned int)m_currentGpsSegment < m_gpsData->getNormalizedUTMPoints().size())
        {
            if ((unsigned int)m_currentGpsPoint < m_gpsData->getNormalizedUTMPoints()[m_currentGpsSegment].size()-1)
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
            if ((unsigned int)m_currentGpsPoint < m_gpsData->getNormalizedUTMPoints()[m_currentGpsSegment].size()-1)
            {
                ++m_currentGpsPoint;
            }
            else//	void setMinMaxRatio();

            {
                std::cout << "I'm the last gps point" << std::endl;
                m_currentGpsPoint = 0;
                m_currentGpsSegment = 0;
                m_currentPoint = -1;
            }
        }
        ++m_currentPoint;
}

void Walk::updateToNextSegment()
{
    if (m_gpsData->getTotalGpsPoints() > 0
            && m_gpsData->getNormalizedUTMPoints().size() > 0)
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
void Walk::updateToPreviousSegment()
{
    if (m_gpsData->getTotalGpsPoints() > 0
            && m_gpsData->getNormalizedUTMPoints().size() > 0)
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
    if (m_gpsData->getNormalizedUTMPoints().size() > 0
            && m_gpsData->getNormalizedUTMPointsGlobal()[m_currentGpsSegment].size() > 0)
	{
		// -----------------------------------------------------------------------------
		// Draw Gps data
		// -----------------------------------------------------------------------------

        const UtmPoint& currentUtm =
                m_gpsData->getUTMPoints()[m_currentGpsSegment][m_currentGpsPoint];

        if(!m_interactiveMode && !m_settings.isMultiMode()
                && !m_settings.isBoundingBoxFixed())
        {
            m_magicBox->updateBoxIfNeeded(currentUtm);
        }

        int startSeg, startPoint;
        if(m_maxPointsToDraw > 0)
        {
            if(m_maxPointsToDraw - m_currentPoint <= 0)
            {
                int startIndex = m_currentPoint-m_maxPointsToDraw;
                const GpsDataIndex& gpsDataIndex =
                        m_gpsData->getIndices()[startIndex];
                startSeg = gpsDataIndex.gpsSegment;
                startPoint = gpsDataIndex.gpsPoint;
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
                const UtmPoint& utm = m_gpsData->getUTMPoints()[i][j];
                bool isInBox = true;
                if(m_settings.isBoundingBoxAuto() && !m_settings.isMultiMode())
                {
                    isInBox = m_magicBox->isInBox(utm);
                    if(!isInBox)
                    {
                        glEnd();
                        glBegin(GL_LINE_STRIP);
                    }
                }
//                else
//                    isInBox = true;
                if(m_settings.useSpeed())
                {
                    //DBG_VAL(utm.speed);
                    if(utm.speed > m_settings.getSpeedThreshold())
                    {
                        ofColor tmpColor = m_settings.getSpeedColorAbove();
                        ofSetColor(tmpColor.r, tmpColor.g, tmpColor.b, tmpColor.a);
                        if(tmpColor.a == 0.0)
                        {
                            isInBox = false;
                            glEnd();
                            glBegin(GL_LINE_STRIP);
                        }
                    }
                    else
                    {
                        ofColor tmpColor = m_settings.getSpeedColorUnder();
                        ofSetColor(tmpColor.r, tmpColor.g, tmpColor.b, tmpColor.a);
                        if(tmpColor.a == 0.0)
                        {
                            isInBox = false;
                            glEnd();
                            glBegin(GL_LINE_STRIP);
                        }
                    }
                }
                if(isInBox)
                {
//                    ofSetColor(255, 0,0);
                    ofxPoint<double> tmp = m_magicBox->getDrawablePoint(utm);
                    glVertex2d(getScaledUtmX(tmp.x),
                               getScaledUtmY(tmp.y));
                }

            }
            glEnd();

            startPoint = 0;
        }
//        ofDisableSmoothing();

        ofxPoint<double> currentDrawablePoint =
                m_magicBox->getDrawablePoint(currentUtm);

		if (m_currentPointIsImage)
		{
			ofSetColor(255, 255, 255, m_imageAlpha);
            m_image.draw(getScaledUtmX(currentDrawablePoint.x),
                         getScaledUtmY(currentDrawablePoint.y));

		}
		else if(!m_interactiveMode)
		{
			ofFill();
            bool skipDrawing = false;
            if(m_settings.useSpeed())
			{
                if(currentUtm.speed > m_settings.getSpeedThreshold())
			    {
                    skipDrawing = m_settings.getSpeedColorAbove().a == 0.0;
                }
                else
                {
                    skipDrawing = m_settings.getSpeedColorUnder().a == 0.0;
                }
			}
            if (!skipDrawing)
			{
			    ofSetColor(m_dotColor.r, m_dotColor.g, m_dotColor.b, m_dotColor.a);
                ofCircle(getScaledUtmX(currentDrawablePoint.x),
                         getScaledUtmY(currentDrawablePoint.y),
                         m_dotSize);
			}
		}
	}

    // draw borders of bounding boxes.
    if(EXTRA_DEBUG_MODE)
    {
        ofNoFill();
        ofSetColor(255,0,0);

        const ofxRectangle<double>& normalizedBox = m_magicBox->getNormalizedBox();
        double x = getScaledUtmX(normalizedBox.getX());
        double y = getScaledUtmY(normalizedBox.getY());
        double w = getScaledUtmX(normalizedBox.getWidth()) - x;
        double h = getScaledUtmY(normalizedBox.getHeight()) - y;

        ofRect(x, y , w, h);

        ofNoFill();
        ofSetColor(0,255,0);

        const ofxRectangle<double>& normalizedPaddedBox =
                m_magicBox->getNormalizedPaddedBox();
        double xp = getScaledUtmX(normalizedPaddedBox.getX());
        double yp = getScaledUtmY(normalizedPaddedBox.getY());
        double wp = getScaledUtmX(normalizedPaddedBox.getWidth()) - xp;
        double hp = getScaledUtmY(normalizedPaddedBox.getHeight()) - yp;

        ofRect(xp, yp , wp, hp);
    }

}

// -----------------------------------------------------------------------------
// Draw all Gps data
// -----------------------------------------------------------------------------
void Walk::drawAll()
{
	ofNoFill();
    BOOST_FOREACH(const UtmSegment& utmSegment, m_gpsData->getUTMPoints())
    {
        glBegin(GL_LINE_STRIP);
        BOOST_FOREACH(const UtmPoint& utmPoint, utmSegment)
        {
            bool isInBox = true;
            if(m_settings.isBoundingBoxAuto() && !m_settings.isMultiMode())
            {
                isInBox = m_magicBox->isInBox(utmPoint);
            }
            if (isInBox)
            {
                const ofxPoint<double>& tmp = m_magicBox->getDrawablePoint(utmPoint);
                glVertex2d(getScaledUtmX(tmp.x),
                           getScaledUtmY(tmp.y));
            }
        }
        glEnd();
    }
}
// -----------------------------------------------------------------------------
// Set view bounds.
// -----------------------------------------------------------------------------
void Walk::setViewBounds(const int screenWidth,
                         const int screenHeight,
                         const double viewXOffset,
                         const double viewYOffset,
                         const double viewMinDimension,
                         const double viewPadding)
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
    try
    {
        const GpsSegmentVec& segments = m_gpsData->getSegments();
        return segments.at(m_currentGpsSegment).getSegmentNum();
    }
    catch (const std::out_of_range&)
    {
        return 0;
    }
}

int Walk::getCurrentPointNum()
{
    return m_currentPoint;
}

std::string Walk::getCurrentTimestamp()
{
    try
    {
        const GpsSegmentVec& segments = m_gpsData->getSegments();
        const GpsPointVec& points = segments.at(m_currentGpsSegment).getPoints();
        return points.at(m_currentGpsPoint).getTimestamp();
    }
    catch (const std::out_of_range&)
    {
        return std::string();
    }
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
double Walk::getScaledUtmX(const double normalizedUtmX)
{
    return ( normalizedUtmX * (m_viewMinDimension - 2.0 * m_viewPadding) + m_viewXOffset);
}

double Walk::getScaledUtmY(const double normalizedUtmY)
{
    // Flip y coordinates ??
    return m_screenHeight - ( normalizedUtmY * (m_viewMinDimension - 2.0 * m_viewPadding) + m_viewYOffset);
}

UtmPoint Walk::getScaledUtm(const UtmPoint& normalizedUtmPoint)
{
    return UtmPoint(getScaledUtmX(normalizedUtmPoint.x),
                    getScaledUtmY(normalizedUtmPoint.y));
}

const std::string& Walk::getCurrentGpsInfoDebug()
{
    GpsPoint boxCenter = m_magicBox->getCenterGps();
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
//	"Viewbox center    : " + ofToString(m_magicBox->getCenter().x,7) + " / " + ofToString(m_magicBox->getCenter().y, 7) + "\n" +
	"Viewbox center    : " + ofToString(boxCenter.getLatitude(),7) + " / " + ofToString(boxCenter.getLongitude(), 7) + "\n" +
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
//		sscanf(timeString.c_str(), "%d-%d-%dT%d:%d:%dZ", &year, &month, &day, &hour, &min, &sec);
        sscanf(timeString.c_str(), "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &min, &sec);
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

void Walk::setGpsData(GpsData* const gpsData)
{
    m_gpsData = 0;
    m_gpsData = gpsData;
}


void Walk::setMagicBox(MagicBox* const magicBox)
{
    m_magicBox = 0;
    m_magicBox = magicBox;
    reset();
    m_magicBox->setupBox(m_gpsData->getUtm(0, 0), GpsData::getLon0Glogal());
}

void Walk::setMagicBoxStatic(MagicBox* const magicBox,
                             const double lat,
                             const double lon)
{

    m_magicBox = 0;
    m_magicBox = magicBox;
    reset();
    UtmPoint utmP = GpsData::getUtmPointWithRegion(lat, lon, m_settings);
    m_magicBox->setupBox(utmP, GpsData::getLon0Glogal());

}

void Walk::setCurrentPointImage(const ofImage& img, const int alpha)
{
    m_image = img;
    m_image.setAnchorPercent(0.5f, 0.5f);
    m_imageAlpha = alpha;
    m_currentPointIsImage = true;
}

void Walk::toggleTraced()
{
    m_drawTraced = !m_drawTraced;
}
