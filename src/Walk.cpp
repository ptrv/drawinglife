/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#include "DrawingLifeIncludes.h"

#include "Walk.h"

float Walk::m_dotSize = 2.0;
int Walk::m_dotAlpha = 127;

Walk::Walk(const AppSettings& settings, ofColor dotColor, bool magicBoxEnabled)
:
DrawingLifeDrawable(),
m_settings(settings),
m_currentGpsPoint(0),
m_currentGpsSegment(0),
m_currentPoint(-1),
m_firstPoint(true),
m_currentGpsPointInfoDebug(""),
m_currentGpsPointInfo(""),
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
	m_image.clear();

    ofLogVerbose("Walk", "destroying");
}

// -----------------------------------------------------------------------------
// Counting through GpsSegments and GpsPoints
// -----------------------------------------------------------------------------
void Walk::update()
{
    const GpsDataPtr gpsData = m_gpsData.lock();
    if (!gpsData)
    {
        return;
    }
    const UtmDataVector& utmData= gpsData->getNormalizedUTMPoints();
    const UtmSegment& utmSegment = utmData[m_currentGpsSegment];

    if (m_currentGpsSegment < static_cast<int>(utmData.size()))
    {
        if (m_currentGpsPoint < static_cast<int>(utmSegment.size()) - 1)
        {
            if (!m_firstPoint)
            {
                ++m_currentGpsPoint;
            }
            else
            {
                m_firstPoint = false;
            }
        }
        else
        {
            ++m_currentGpsSegment;
            m_currentGpsPoint = 0;
        }
    }
    else
    {
        if (m_currentGpsPoint < static_cast<int>(utmSegment.size()) - 1)
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
    updateToSegment(false);
}
void Walk::updateToPreviousSegment()
{
    updateToSegment(true);
}

void Walk::updateToSegment(bool prev)
{
    const GpsDataPtr gpsData = m_gpsData.lock();
    if (!gpsData)
    {
        return;
    }

    const UtmDataVector& utmData = gpsData->getNormalizedUTMPoints();

    if (gpsData->getTotalGpsPoints() > 0
            && static_cast<int>(utmData.size()) > 0)
    {
        if (!prev && m_currentGpsSegment < static_cast<int>(utmData.size()) - 1)
        {
            ++m_currentGpsSegment;
            const UtmSegment& utmSegment = utmData[m_currentGpsSegment];
            m_currentGpsPoint = static_cast<int>(utmSegment.size()) - 1;

        }
        else if (prev && m_currentGpsSegment > 0)
        {
            --m_currentGpsSegment;
            const UtmSegment& utmSegment = utmData[m_currentGpsSegment];
            m_currentGpsPoint = static_cast<int>(utmSegment.size()) - 1;
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
    const GpsDataPtr gpsData = m_gpsData.lock();
    const MagicBoxPtr magicBox = m_magicBox.lock();
    if (!gpsData || !magicBox)
    {
        return;
    }

    if (gpsData->getNormalizedUTMPoints().size() > 0
            && gpsData->getNormalizedUTMPointsGlobal()[m_currentGpsSegment].size() > 0)
	{
		// -----------------------------------------------------------------------------
		// Draw Gps data
		// -----------------------------------------------------------------------------

        const UtmPoint& currentUtm =
                gpsData->getUTMPoints()[m_currentGpsSegment][m_currentGpsPoint];

        if (!m_interactiveMode && !m_settings.isMultiMode()
                && !m_settings.isBoundingBoxFixed())
        {
            magicBox->updateBoxIfNeeded(currentUtm);
        }

        int startSeg, startPoint;
        if (m_maxPointsToDraw > 0 && m_maxPointsToDraw - m_currentPoint <= 0)
        {
            int startIndex = m_currentPoint-m_maxPointsToDraw;
            const GpsDataIndex& gpsDataIndex =
                    gpsData->getIndices()[startIndex];
            startSeg = gpsDataIndex.gpsSegment;
            startPoint = gpsDataIndex.gpsPoint;
        }
        else
        {
            startSeg = 0;
            startPoint = 0;
        }
        if (m_interactiveMode && !m_drawTraced)
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
                if (m_interactiveMode && m_drawTraced)
                    ofSetColor(m_currentSegColor.r,
                               m_currentSegColor.g,
                               m_currentSegColor.b,
                               m_currentSegColor.a);
            }
            else
            {
                pointEnd = (int)gpsData->getNormalizedUTMPointsGlobal()[i].size()-1;
            }

            for (int j = startPoint; j <= pointEnd; ++j)
            {
                const UtmPoint& utm = gpsData->getUTMPoints()[i][j];
                bool isInBox = true;
                if (m_settings.isBoundingBoxAuto() && !m_settings.isMultiMode())
                {
                    isInBox = magicBox->isInBox(utm);
                    if (!isInBox)
                    {
                        glEnd();
                        glBegin(GL_LINE_STRIP);
                    }
                }
//                else
//                    isInBox = true;
                if (m_settings.useSpeed())
                {
                    if (utm.speed > m_settings.getSpeedThreshold())
                    {
                        const ofColor tmpColor = m_settings.getSpeedColorAbove();
                        ofSetColor(tmpColor);
                        if (tmpColor.a == 0.0)
                        {
                            isInBox = false;
                            glEnd();
                            glBegin(GL_LINE_STRIP);
                        }
                    }
                    else
                    {
                        const ofColor tmpColor = m_settings.getSpeedColorUnder();
                        ofSetColor(tmpColor);
                        if (tmpColor.a == 0.0)
                        {
                            isInBox = false;
                            glEnd();
                            glBegin(GL_LINE_STRIP);
                        }
                    }
                }
                if (isInBox)
                {
//                    ofSetColor(255, 0,0);
                    const ofxPoint<double> tmp = magicBox->getDrawablePoint(utm);
                    glVertex2d(getScaledUtmX(tmp.x),
                               getScaledUtmY(tmp.y));
                }

            }
            glEnd();

            startPoint = 0;
        }
//        ofDisableSmoothing();

        const ofxPoint<double> currentDrawablePoint =
                magicBox->getDrawablePoint(currentUtm);

		if (m_currentPointIsImage)
		{
			ofSetColor(255, 255, 255, m_imageAlpha);
            m_image.draw(getScaledUtmX(currentDrawablePoint.x),
                         getScaledUtmY(currentDrawablePoint.y));

		}
        else if (!m_interactiveMode)
		{
			ofFill();
            bool skipDrawing = false;
            if (m_settings.useSpeed())
			{
                if (currentUtm.speed > m_settings.getSpeedThreshold())
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
                ofSetColor(m_dotColor);
                ofCircle(getScaledUtmX(currentDrawablePoint.x),
                         getScaledUtmY(currentDrawablePoint.y),
                         m_dotSize);
			}
		}
	}

    // draw borders of bounding boxes.
    if (EXTRA_DEBUG_MODE)
    {
        ofNoFill();
        ofSetColor(255,0,0);

        const ofxRectangle<double>& normalizedBox =
                magicBox->getNormalizedBox();
        double x = getScaledUtmX(normalizedBox.getX());
        double y = getScaledUtmY(normalizedBox.getY());
        double w = getScaledUtmX(normalizedBox.getWidth()) - x;
        double h = getScaledUtmY(normalizedBox.getHeight()) - y;

        ofRect(x, y , w, h);

        ofNoFill();
        ofSetColor(0,255,0);

        const ofxRectangle<double>& normalizedPaddedBox =
                magicBox->getNormalizedPaddedBox();
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
    const GpsDataPtr gpsData = m_gpsData.lock();
    const MagicBoxPtr magicBox = m_magicBox.lock();
    if (!gpsData || !magicBox)
    {
        return;
    }

	ofNoFill();
    BOOST_FOREACH(const UtmSegment& utmSegment, gpsData->getUTMPoints())
    {
        glBegin(GL_LINE_STRIP);
        BOOST_FOREACH(const UtmPoint& utmPoint, utmSegment)
        {
            bool isInBox = true;
            if (m_settings.isBoundingBoxAuto() && !m_settings.isMultiMode())
            {
                isInBox = magicBox->isInBox(utmPoint);
            }
            if (isInBox)
            {
                const ofxPoint<double>& tmp =
                        magicBox->getDrawablePoint(utmPoint);
                glVertex2d(getScaledUtmX(tmp.x),
                           getScaledUtmY(tmp.y));
            }
        }
        glEnd();
    }
}

// -----------------------------------------------------------------------------

const std::string Walk::getGpsLocationCurrent() const
{
    const GpsDataPtr gpsData = m_gpsData.lock();
    if (!gpsData)
    {
        return std::string();
    }

    return gpsData->getGpsLocation(m_currentGpsSegment, m_currentGpsPoint);
}

int Walk::getCurrentSegmentNum() const
{
    const GpsDataPtr gpsData = m_gpsData.lock();
    if (!gpsData)
    {
        return 0;
    }

    try
    {
        const GpsSegmentVector& segments = gpsData->getSegments();
        return segments.at(m_currentGpsSegment).getSegmentNum();
    }
    catch (const std::out_of_range&)
    {
        return 0;
    }
}

int Walk::getCurrentPointNum() const
{
    return m_currentPoint;
}

std::string Walk::getCurrentTimestamp() const
{
    const GpsDataPtr gpsData = m_gpsData.lock();
    if (!gpsData)
    {
        return std::string();
    }

    try
    {
        const GpsSegmentVector& segments = gpsData->getSegments();
        const GpsPointVector& points =
                segments.at(m_currentGpsSegment).getPoints();
        return points.at(m_currentGpsPoint).getTimestamp();
    }
    catch (const std::out_of_range&)
    {
        return std::string();
    }
}

double Walk::getCurrentThing(const tFuncGetCurrentDouble& fnGetCurrentDouble) const
{
    if (const GpsDataPtr gpsData = m_gpsData.lock())
    {
        return fnGetCurrentDouble(gpsData, m_currentGpsSegment, m_currentGpsPoint);
    }
    else
    {
        return 0.0;
    }
}

double Walk::getCurrentLongitude() const
{
    return getCurrentThing(boost::bind(&GpsData::getLongitude, _1, _2, _3));
}
double Walk::getCurrentLatitude() const
{
    return getCurrentThing(boost::bind(&GpsData::getLatitude, _1, _2, _3));
}
double Walk::getCurrentElevation() const
{
    return getCurrentThing(boost::bind(&GpsData::getElevation, _1, _2, _3));
}
double Walk::getCurrentUtmX() const
{
    return getCurrentThing(boost::bind(&GpsData::getUtmX, _1, _2, _3));
}
double Walk::getCurrentUtmY() const
{
    return getCurrentThing(boost::bind(&GpsData::getUtmY, _1, _2, _3));
}

// -----------------------------------------------------------------------------

const std::string& Walk::getCurrentGpsInfoDebug()
{
    const GpsDataPtr gpsData = m_gpsData.lock();
    const MagicBoxPtr magicBox = m_magicBox.lock();
    if (gpsData && magicBox)
    {
        GpsPoint boxCenter = magicBox->getCenterGps();
        m_currentGpsPointInfoDebug  =	"Longitude         : " + ofToString(getCurrentLongitude(), 7) + "\n" +
        "Latitude          : " + ofToString(getCurrentLatitude(), 7) + "\n" +
        "Elevation         : " + ofToString(getCurrentElevation(), 7) + "\n" +
        "UTM X             : " + ofToString(getCurrentUtmX(), 7) + "\n" +
        "UTM Y             : " + ofToString(getCurrentUtmY(), 7) + "\n" +
        "Time              : " + getCurrentTimestamp() + "\n" +
        "Location          : " + getGpsLocationCurrent() + "\n" +
        "Central meridian  : " + ofToString(gpsData->getLon0(), 7) + "\n" +
        "Meridian global   : " + ofToString(GpsData::getLon0Glogal(), 7) + "\n" +
        "Min/Max latitude  : " + ofToString(gpsData->getMinLat(), 7) + " / " + ofToString(gpsData->getMaxLat(), 7) + "\n" +
        "Min/Max longitude : " + ofToString(gpsData->getMinLon(), 7) + " / " + ofToString(gpsData->getMaxLon(), 7) + "\n" +
        "Min/Max UTM X     : " + ofToString(gpsData->getMinUtmX(), 7) + " / " + ofToString(gpsData->getMaxUtmX(), 7) + "\n" +
        "Min/Max UTM Y     : " + ofToString(gpsData->getMinUtmY(), 7) + " / " + ofToString(gpsData->getMaxUtmY(), 7) + "\n" +
        "Currrent pt.      : " + ofToString(getCurrentPointNum()) + "\n" +
        "Segment nr.       : " + ofToString(getCurrentSegmentNum()) + "\n" +
        "Total pts.        : " + ofToString(gpsData->getTotalGpsPoints()) + "\n" +
    //	"Viewbox center    : " + ofToString(m_magicBox->getCenter().x,7) + " / " + ofToString(m_magicBox->getCenter().y, 7) + "\n" +
        "Viewbox center    : " + ofToString(boxCenter.getLatitude(),7) + " / " + ofToString(boxCenter.getLongitude(), 7) + "\n" +
        "Viewbox size      : " + ofToString(magicBox->getSize(),7) + "\n" +
        "Person            : " + gpsData->getUser();
    }
    else
    {
        m_currentGpsPointInfoDebug = std::string();
    }


    return m_currentGpsPointInfoDebug;
}

const std::string& Walk::getCurrentGpsInfo()
{
    const GpsDataPtr gpsData = m_gpsData.lock();
    if (gpsData && gpsData->getTotalGpsPoints() != 0)
	{
		std::string timeString = getCurrentTimestamp();
		int year, month, day, hour, min, sec;
//        sscanf(timeString.c_str(), "%d-%d-%dT%d:%d:%dZ",
//               &year, &month, &day, &hour, &min, &sec);
        sscanf(timeString.c_str(), "%d-%d-%d %d:%d:%d",
               &year, &month, &day, &hour, &min, &sec);
		char buf[25];
        sprintf(buf, "%02d.%02d.%d %02d:%02d:%02d",
                day, month, year, hour, min, sec);
		m_currentGpsPointInfo = getGpsLocationCurrent() + " " + string(buf);
    }
    else
    {
        m_currentGpsPointInfo = std::string();
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

void Walk::setGpsData(const GpsDataWeak gpsDataWeak)
{
    m_gpsData = gpsDataWeak;
}


void Walk::setMagicBox(MagicBoxWeak magicBoxWeak)
{
    m_magicBox = magicBoxWeak;
    reset();
    GpsDataPtr gpsData = m_gpsData.lock();
    MagicBoxPtr magicBox = m_magicBox.lock();
    if (gpsData && magicBox)
    {
        magicBox->setupBox(gpsData->getUtm(0, 0), GpsData::getLon0Glogal());
    }
}

void Walk::setMagicBoxStatic(MagicBoxWeak magicBoxWeak,
                             const double lat,
                             const double lon)
{
    m_magicBox = magicBoxWeak;
    reset();
    UtmPoint utmP = GpsData::getUtmPointWithRegion(lat, lon, m_settings);
    if (MagicBoxPtr magicBox = m_magicBox.lock())
    {
        magicBox->setupBox(utmP, GpsData::getLon0Glogal());
    }

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
