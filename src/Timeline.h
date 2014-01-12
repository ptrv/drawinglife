/*=======================================================
 Copyright (c) avp::ptr, 2010
 =======================================================*/

#ifndef _TIMELINE_H_
#define _TIMELINE_H_

#include "DrawingLifeIncludes.h"
#include <vector>
#include <string>
#include "GpsData.h"

/**
 * \brief Class for managing the timeline.
 *
 */
class Timeline
{
public:
    Timeline();
    ~Timeline();

    /**
    * \brief Set timeline for gps data.
    * \param gpsDatas vector with gps data.
    */
    void setTimeline(const GpsDataVector& gpsDatas);

    /**
    * \brief Get next user id on the timeline.
    * \return int value for user id.
    */
    int getNext() const;

    /**
    * \brief Return true if count is last in timeline.
    * \return true if count is last.
    */
    bool isLast() const;

    /**
    * \brief Return true if count is last in timeline.
    * \return true if count is last.
    */
    bool isFirst() const;

    /**
    * \brief Return true if count is last in timeline.
    * \return true if count is last.
    */
    bool isTimelineIndex(unsigned int index);

    /**
    * \brief Get the timeline.
    * \return vector of TimelineObjects.
    */
    const std::vector<TimelineObject>& getTimeline() const { return m_timeline; }

    bool isNextReady();

    unsigned int getNumberToUpdate()
    {
        unsigned int tmp = m_counter - m_lastUpdatedTimelineId;
        return  tmp;
    }

    void countUp();

    std::string getCurrentTime();

    unsigned int getCurrentCount() const;
    unsigned int getAllCount() const;

    const TimelineObject& getCurrentTimelineObj() const;

private:
    // -----------------------------------------------------------------------------
    /**
    * \brief Get unix time for a given time string.
    * \param timeString time string.
    * \return time_t value.
    */
    time_t makeTimeObject(const std::string& timeString);

    /**
    * \brief Sort timeline objects.
    */
    void sortTimeline();
    // -----------------------------------------------------------------------------
    std::vector<TimelineObject> m_timeline;

    TimelineObject* m_current;
    TimelineObject* m_last;

    unsigned int m_counter;

    unsigned int m_indexToUpdate;

    unsigned int m_lastUpdatedTimelineId;

    bool m_currentCountWasUpdated;

    // (prevent copy constructor and operator= being generated..)
    Timeline (const Timeline&);
    const Timeline& operator= (const Timeline&);

};

#endif // _TIMELINE_H_
