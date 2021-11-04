#include "Image.h"
#include "DxLib.h"
#include <math.h>

event::event(int startVal, int endVal, ConvertMode mode, double base, double timeStart, double  timeEnd) {
	event::startVal = startVal;
	event::endVal = endVal;

	event::mode = mode;
	event::base = base;

	event::timeStart = timeStart;
	event::timeEnd = timeEnd;
}

double event::calculateTimeRatio(double now) {
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

double event::convert(double input) {
	double output = 0;
	switch (mode) {
	case ConvertMode::Teleportation:
		if (input < 1) {
			output = 0;
		}
		else {
			output = input;
		}
		break;
	case ConvertMode::Linear:
		output = input;
		break;
	case ConvertMode::HalfCosine:
		output = (1.0 - cos(input * 3.14159265)) / 2;
		break;
	case ConvertMode::QuarterSine:
		output = sin(input * (3.14159265 / 2));
		break;
	case ConvertMode::QuarterCosine:
		output = 1.0 - cos(input * (3.14159265 / 2));
		break;
	case ConvertMode::Exponential:
		output = 0;
		double startExpY = 1;
		double endExpY = 4;

		if (base == 1 || base <= 0) {
			output = input;
		}
		else if (base > 1) {
			startExpY = 0;
			endExpY = pow(base, 1) - 1;
			output = (pow(base, input) - 1) / endExpY;
		}
		else if (base < 1) {
			startExpY = 0;
			endExpY = 1 - pow(base, 1);
			output = (1 - pow(base, input)) / endExpY;
		}
		break;
	}
	return output;
}

//setter
void event::setStartVal(int start) { startVal = start; }
void event::setEndVal(int end) { endVal = end; }

void event::setMode(ConvertMode mode, double base) {
	event::mode = mode;
	event::base = base;
}
void event::setTime(int start, int end) {
	timeStart = start;
	timeEnd = end;
}

//getter
int event::getStartVal() { return startVal; }
int event::getEndVal() { return endVal; }
ConvertMode event::getMode() { return mode; }
double event::getBase() { return base; }
double event::getTimeStart() { return timeStart; }
double event::getTimeEnd() { return timeEnd; }


Image::Image(wchar_t* path, int x, int y, BOOL visible, int alpha) {
	handle = LoadGraph(path);
	Image::visible.push(
		event(visible, visible)
	);

	Image::moveXEvent.push(
		event(x, x)
	);

	Image::moveYEvent.push(
		event(y, y)
	);

	Image::alpha.push(
		event(alpha, alpha)
	);
	Image::x = x;
	Image::y = y;
	visibleVal = visible;
	alphaVal = alpha;

}

Image::~Image() {
	DeleteGraph(handle);
}

void Image::reset() {
	clearEvent(visible);
	clearEvent(moveXEvent);
	clearEvent(moveYEvent);
	clearEvent(alpha);
}

void Image::clearEvent(std::queue<event> &eventQueue) {
	std::queue<event> empty;
	std::swap(eventQueue, empty);
}

void Image::move(int xStart, int yStart, int xEnd, int yEnd, Specify specify, ConvertMode mode, double nowTime, double time, double base) {//指定した座標間を移動		
	Image::moveXEvent.push(
		event(absRel(specify, x, xStart), absRel(specify, x, xEnd), mode, base, nowTime, nowTime + time)
	);

	Image::moveYEvent.push(
		event(absRel(specify, y, yStart), absRel(specify, y, yEnd), mode, base, nowTime, nowTime + time)
	);

}

void Image::moveTo(int x, int y,	Specify specify, ConvertMode mode, double nowTime, double time, double base){//現在の座標から移動
	Image::moveXEvent.push(
		event(Image::x, absRel(specify, Image::x, x), mode, base, nowTime, nowTime + time)
	);

	Image::moveYEvent.push(
		event(Image::y, absRel(specify, Image::y, y), mode, base, nowTime, nowTime + time)
	);
}


void Image::appear(double nowTime, double time) {
	Image::visible.push(
		event(0, 1, Teleportation, 1, nowTime, nowTime + time)
	);
}
void Image::disappear(double nowTime, double time) {
	Image::visible.push(
		event(1, 0, Teleportation, 1, nowTime, nowTime + time)
	);
}

void Image::changeAlpha(int startVal, int endVal, Specify specify, ConvertMode mode, double nowTime, double time, double base) {
	Image::alpha.push(
		event(absRel(specify, alphaVal, startVal), absRel(specify, alphaVal, endVal), mode, base, nowTime, nowTime + time)
	);
}
void Image::changeAlphaTo(int val, Specify specify, ConvertMode mode, double nowTime, double time, double base) {
	Image::alpha.push(
		event(alphaVal, absRel(specify, alphaVal,  val), mode, base, nowTime, nowTime + time)
	);
}


void Image::draw(double nowTime) {
	if (!visible.empty()) {
		visibleVal = calculateVal(nowTime, visible.front());
		pop(&visible, nowTime);
	}
	if (!moveXEvent.empty()) { 
		x = calculateVal(nowTime, moveXEvent.front());
		pop(&moveXEvent, nowTime);
	}
	if (!moveYEvent.empty()) { 
		y = calculateVal(nowTime, moveYEvent.front());
		pop(&moveYEvent, nowTime);
	}
	if (!alpha.empty()) {
		alphaVal = calculateVal(nowTime, alpha.front());
		pop(&alpha, nowTime);
	}

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alphaVal);
	if (visibleVal && alphaVal != 0) {
		DrawGraph(x, y, handle, TRUE);
	}
}

int Image::calculateVal(double nowTime, event event) {
	//移動時間経過割合
	double timeRatio = event.calculateTimeRatio(nowTime);

	//距離にかける値(この値の変化具合で移動の仕方が変わる)
	double multipleRatio = event.convert(timeRatio);

	double distance = (double)event.getEndVal() - event.getStartVal();

	return (int)((double)event.getStartVal() + distance * multipleRatio);
}

void Image::pop(std::queue<event>* eventQueue, double nowTime) {
	if (eventQueue->front().getTimeEnd() <= nowTime)eventQueue->pop();
}

int Image::absRel(Specify specify, int now, int target) {//値の絶対指定、相対指定判別関数
	if (specify == Specify::Abs) {
		return target;
	}
	else if (specify == Specify::Rel) {
		return now + target;
	}
}