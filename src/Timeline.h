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

    void setData(const GpsDataVector& gpsDatas);

    void countUp();

    int getCurrentId() const;

    bool isLast() const;
    bool isFirst() const;
    bool isTimelineIndex(unsigned int index);
    bool isNextReady();

    const TimelineObjectVec& getTimeline() const { return m_timeline; }
    inline unsigned int getNumberToUpdate() const;
    std::string getCurrentTime() const;
    unsigned int getCurrentCount() const;
    unsigned int getAllCount() const;
    const TimelineObject& getCurrentTimelineObj() const;

private:
    // -------------------------------------------------------------------------
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
    // -------------------------------------------------------------------------
    TimelineObjectVec m_timeline;

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

//------------------------------------------------------------------------------

unsigned int Timeline::getNumberToUpdate() const
{
    return m_counter - m_lastUpdatedTimelineId;
}

//------------------------------------------------------------------------------

#endif // _TIMELINE_H_
