/*
 *  TimedCounter.cpp
 *  openFrameworks
 *
 *  Created by Pat Long on 08/07/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "TimedCounter.h"

TimedCounter::TimedCounter(int countStart, int countEnd, int countDelay){
	this->countStart = countStart;
	this->countEnd = countEnd;
	this->countDelay = countDelay;
	this->countInc = (this->countEnd < this->countStart)?-1:1;
	this->resetCurrentCount();
}

TimedCounter::~TimedCounter(){
}

void TimedCounter::resetCurrentCount(){
	this->isActive = false;
	this->cCount = this->countStart;
	this->lastCountTime = ofGetElapsedTimeMillis();
	this->countComplete = false;
	this->changeFlag = true;
	this->cCountPercent = 0.0;
}

int TimedCounter::getCurrentCount(){
	return this->cCount;
}

float TimedCounter::getCurrentCountPercent(){
	return this->cCountPercent;
}

bool TimedCounter::isCounting(){
	return this->isActive;
}

bool TimedCounter::isCountComplete(){
	return this->countComplete;
}

bool TimedCounter::hasChanged(){
	bool check = this->changeFlag;
	this->changeFlag = false;
	return check;
}


void TimedCounter::startCount(){
	this->resetCurrentCount();
	this->isActive = true;
}

void TimedCounter::nextCount(){
	this->cCount += this->countInc;
	this->cCountPercent = 0.0f;
	this->changeFlag = true;
}

void TimedCounter::endCount(){
	this->countComplete = true;
	this->isActive = false;
}

void TimedCounter::update(){
	if(this->isActive){
		int cTime = ofGetElapsedTimeMillis();
		this->cCountPercent = (float)(cTime - this->lastCountTime) / (float)this->countDelay;
		if(this->cCountPercent >= 1.0){
			this->nextCount();
			this->lastCountTime = cTime;
			if(this->cCount == this->countEnd)
				this->endCount();
		}
	}
}
