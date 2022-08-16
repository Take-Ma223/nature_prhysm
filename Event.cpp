#include "Event.h"

Event::Event(double timeStart, double timeEnd) {


	Event::timeStart = timeStart;
	Event::timeEnd = timeEnd;
}

double Event::calculateTimeRatio(double now) {
	double ratio = 0;
	if (timeEnd == timeStart) {
		ratio = 1;
	}
	else {
		ratio = (now - timeStart) / (timeEnd - timeStart);
	}
	if (ratio < 0) {
		ratio = 0;
	}
	else if (ratio > 1) {
		ratio = 1;
	}
	return ratio;
}


//setter
void Event::setTime(int start, int end) {
	timeStart = start;
	timeEnd = end;
}

//getter
double Event::getStartTime() { return timeStart; }
double Event::getEndTime() { return timeEnd; }
