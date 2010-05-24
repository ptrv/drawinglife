/*=======================================================
 Copyright (c) avp::ptr, 2010
 =======================================================*/

#ifndef _TIMELINE_H_
#define _TIMELINE_H_

#include "DrawingLifeIncludes.h"
#include <vector>
#include <string>
#include "GpsData.h"

using namespace std;

/**
 * \brief Represents an entry on the timeline with time and id.
 *
 */
struct TimelineObject
{
    string timeString;
    time_t secs;
    int id;
    bool operator() (TimelineObject lhs, TimelineObject rhs)
    {
        return lhs.secs < rhs.secs;
    }
};

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
    void setTimeline(vector<GpsData*> gpsDatas);

    /**
    * \brief Get next user id on the timeline.
    * \return int value for user id.
    */
    int getNext();

    /**
    * \brief Get the timeline.
    * \return vector of TimelineObjects.
    */
    const vector<TimelineObject>& getTimeline() const { return m_timeline; }

private:
    // -----------------------------------------------------------------------------
    /**
    * \brief Get unix time for a given time string.
    * \param timeString time string.
    * \return time_t value.
    */
    time_t makeTimeObject(string timeString);

    /**
    * \brief Sort timeline objects.
    */
    void sort();
    // -----------------------------------------------------------------------------
    vector<TimelineObject> m_timeline;

    unsigned int m_counter;

    // (prevent copy constructor and operator= being generated..)
    Timeline (const Timeline&);
    const Timeline& operator= (const Timeline&);

};

#endif // _TIMELINE_H_
