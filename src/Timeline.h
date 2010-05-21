/*=======================================================
 Copyright (c) avp::ptr, 2010
 =======================================================*/

#ifndef _TIMELINE_H_
#define _TIMELINE_H_

#include "DrawingLifeIncludes.h"
#include <vector>
#include <string>
#include "GpsData.h"
//#include "Time.h"

struct TimelineObject
{
    std::string timeString;
    time_t secs;
    int id;
};

using namespace std;

class Timeline
{
public:
    Timeline();
    ~Timeline();

    void setTimeline(std::vector<GpsData*> gpsDatas);
    int getNext();

    const std::vector<TimelineObject>& getTimeline() const { return m_timeline; }
private:
    time_t makeTimeObject(std::string timeString);
    void sort();

    std::vector<TimelineObject> m_timeline;

    unsigned int m_counter;
};

#endif // _TIMELINE_H_
