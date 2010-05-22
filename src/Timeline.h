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


class Timeline
{
public:
    Timeline();
    ~Timeline();

    void setTimeline(vector<GpsData*> gpsDatas);

    int getNext();

    const vector<TimelineObject>& getTimeline() const { return m_timeline; }

private:
    // -----------------------------------------------------------------------------
    time_t makeTimeObject(string timeString);

    void sort();
    // -----------------------------------------------------------------------------
    vector<TimelineObject> m_timeline;

    unsigned int m_counter;
};

#endif // _TIMELINE_H_
