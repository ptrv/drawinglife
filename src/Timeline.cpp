/*=======================================================
 Copyright (c) avp::ptr, 2010
 =======================================================*/

#include "Timeline.h"
#include <algorithm>


Timeline::Timeline()
:
m_current(0),
m_last(0),
m_counter(0),
m_indexToUpdate(0),
m_lastUpdatedTimelineId(0),
m_currentCountWasUpdated(false)
{
    //ctor
}

Timeline::~Timeline()
{
    ofLogVerbose("Timeline", "destroying");
}

void Timeline::sortGpsDataVecs(const std::vector<GpsData*>& gpsDatas)
{
}

void Timeline::setTimeline(const GpsDataVector& gpsDatas)
{
    m_counter = 0;
    m_timeline.clear();
    int i = 0;
    BOOST_FOREACH(const GpsDataPtr gpsData, gpsDatas)
    {
        BOOST_FOREACH(const GpsSegment& gpsSegment, gpsData->getSegments())
        {
            BOOST_FOREACH(const GpsPoint& gpsPoint, gpsSegment.getPoints())
            {
                std::string timeString = gpsPoint.getTimestamp();
                TimelineObject tmObj;
                tmObj.timeString = timeString;
                tmObj.secs = makeTimeObject(timeString);
                tmObj.id = i;
                tmObj.gpsid = gpsPoint.getGpsPointId();
                m_timeline.push_back(tmObj);
//                ofLog(OF_LOG_VERBOSE, "%s : %d : %li\n", tmObj.timeString.c_str(), tmObj.id, tmObj.secs);
            }
        }
        ++i;
    }

    sortTimeline();
    if (m_timeline.size() > 0)
    {
        m_current = &m_timeline[0];
    }
}

int Timeline::getNext() const
{
    if (m_timeline.size() > 0)
    {
        int id = m_timeline[m_counter].id;
//        ++m_counter;
//        m_counter %= m_timeline.size();

        return id;
    }
    else
    {
        return -1;
    }
}

bool Timeline::isLast() const
{
    return m_counter == m_timeline.size()-1;
}

bool Timeline::isFirst() const
{
    return m_counter == 0;
}

bool Timeline::isTimelineIndex(unsigned int index)
{
    return m_counter == index;
}

bool Timeline::isNextReady()
{
//    bool ready = true;
    m_current = &m_timeline[m_counter];

    bool ready = false;
    if (m_last != 0)
    {
        time_t current = m_current->secs;
        time_t last = m_last->secs;

        time_t diff = current - last;

        if (diff > 10)
        {
            m_last = m_current;
            m_currentCountWasUpdated = true;
            ready = true;
        }
        else
        {
            m_currentCountWasUpdated = false;
            ready = false;
        }
    }
    else
    {
        m_last = m_current;
        m_currentCountWasUpdated = true;
        ready = true;
    }
    return ready;
}

void Timeline::countUp()
{
    if (m_currentCountWasUpdated)
    {
        m_lastUpdatedTimelineId = m_counter;
    }
    ++m_counter;
    m_counter %= m_timeline.size();
}
time_t Timeline::makeTimeObject(const std::string& timeString)
{
    struct tm tm;
    time_t t;
    time(&t);
    int year, month, day, hour, min, sec;
    std::string str = timeString;
//    sscanf(str.c_str(), "%d-%d-%dT%d:%d:%dZ", &year, &month, &day, &hour, &min, &sec);
    sscanf(str.c_str(), "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &min, &sec);
    tm = *localtime(&t);
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
//    tm.tm_mon = month;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = min;
    tm.tm_sec = sec;
    tm.tm_isdst = -1;
    t = mktime(&tm);
    return t;
}

void Timeline::sortTimeline()
{
    std::sort(m_timeline.begin(), m_timeline.end(), TimelineObject());
}

std::string Timeline::getCurrentTime()
{
    std::string timeString = m_timeline[m_counter].timeString;
    int year, month, day, hour, min, sec;
//    sscanf(timeString.c_str(), "%d-%d-%dT%d:%d:%dZ", &year, &month, &day, &hour, &min, &sec);
    sscanf(timeString.c_str(), "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &min, &sec);
    char buf[25];
    sprintf(buf, "%02d:%02d:%02d", hour, min, sec);
    return string(buf);

}

unsigned int Timeline::getCurrentCount() const
{
	return m_counter;
}

unsigned int Timeline::getAllCount() const
{
	return m_timeline.size();
}

const TimelineObject& Timeline::getCurrentTimelineObj() const
{
	return m_timeline[m_counter];
}
