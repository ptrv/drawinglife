/*
 *  TimedCounter.h
 *  openFrameworks
 *
 *  Created by Pat Long on 08/07/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _TIMED_COUNTER_H
#define _TIMED_COUNTER_H

#include "ofMain.h"

#define DEFAULT_COUNT_START 3
#define DEFAULT_COUNT_END	0
#define DEFAULT_COUNT_DELAY	1000

class TimedCounter{
	protected:
		bool isActive, countComplete, changeFlag;
		int countStart, countEnd, countDelay, countInc;
		int cCount, lastCountTime;
		float cCountPercent;
	
		virtual void resetCurrentCount();
		
	public:
		TimedCounter(int countStart=DEFAULT_COUNT_START, int countEnd=DEFAULT_COUNT_END, int countDelay=DEFAULT_COUNT_DELAY);
		~TimedCounter();
	
		void update();
	
		int getCurrentCount();
		float getCurrentCountPercent();
		bool isCounting();
		bool isCountComplete();
		bool hasChanged();
	
		virtual void startCount();
		virtual void nextCount();
		virtual void endCount();
};

#endif
