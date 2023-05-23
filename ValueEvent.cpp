#include "ValueEvent.h"

Converter::Converter(ConvertMode mode, double paramA)
{
	Converter::mode = mode;
	Converter::paramA = paramA;
}

ConvertMode Converter::getMode()
{
	return mode;
}

double Converter::getParamA()
{
	return paramA;
}


Point::Point(int val, Specify specify) {
	Point::val = val;
	Point::different = val;
	Point::specify = specify;
}

int Point::updateVal()
{
	return val;
}

Specify Point::getSpecify()
{
	return specify;
}

ValueEvent::ValueEvent(Point start, Point end, Converter converter, double timeStart, double timeEnd) : Event(timeStart, timeEnd)
{
	ValueEvent::eventType = determinEventType(start, end);

	ValueEvent::converter = converter;

}

EventType ValueEvent::determinEventType(Point p1, Point p2)
{
	Specify s1 = p1.getSpecify();
	Specify s2 = p2.getSpecify();

	if (s1 == Abs && s2 == Abs) {
		startValue = p1.updateVal();
		endValue = p2.updateVal();
		return EventType(AbsAbs);
	}
	if (s1 == Abs && s2 == Rel) {
		startValue = p1.updateVal();
		endRelativeValue = p2.updateVal();
		return EventType(AbsRel);
	}
	if (s1 == Rel && s2 == Abs) {
		startRelativeValue = p1.updateVal();
		endValue = p2.updateVal();
		return EventType(RelAbs);
	}
	if (s1 == Rel && s2 == Rel) {
		startRelativeValue = p1.updateVal();
		endRelativeValue = p2.updateVal();
		return EventType(RelRel);
	}

}

void ValueEvent::determinValueFrom(int nowVal, BOOL isReverse)
{
	if (!isReverse) {
		switch (eventType)
		{
		case AbsAbs:
			startValue = startValue;
			endValue = endValue;
			break;
		case AbsRel:
			startValue = startValue;
			endValue = nowVal + endRelativeValue;
			break;
		case RelAbs:
			startValue = nowVal + startRelativeValue;
			endValue = endValue;
			break;
		case RelRel:
			startValue = nowVal + startRelativeValue;
			endValue = nowVal + endRelativeValue;
			break;
		default:
			break;
		}
	}
	else {
		switch (eventType)
		{
		case AbsAbs:
			startValue = startValue;
			endValue = endValue;
			break;
		case AbsRel:
			startValue = startValue;
			endValue = nowVal;
			break;
		case RelAbs:
			startValue = endValue;//逆再生不可能なので終了値から動かないようにする
			endValue = endValue;
			break;
		case RelRel:
			startValue = nowVal - endRelativeValue;
			endValue = nowVal - startRelativeValue;
			break;
		default:
			break;
		}
	}


}

double ValueEvent::easing(double input) {
	double output = 0;

	double startExpY = 1;
	double endExpY = 4;
	double base = converter.getParamA();

	if (input <= 0)return 0;
	else if (input >= 1)return 1;

	switch (converter.getMode()) {
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
	case ConvertMode::ExponentialSlider:
		output = 0;

		auto expSliderFunc = [&base](double x) {
			if (x <= 0) {
				return 0.0;
			}else if (x <= 0.5) {
				return pow(base, x) - 1;
			}
			else if (x < 1) {
				return -pow(base, -x + 1) + 2 * (pow(base, 0.5)) - 1;
			}
			else {
				return -pow(base, -1 + 1) + 2 * (pow(base, 0.5)) - 1;
			}
		};

		if (base == 1 || base <= 0) {
			output = input;
		}
		else if (base > 1) {
			startExpY = 0;
			endExpY = expSliderFunc(1);
			output = expSliderFunc(input) / endExpY;
		}
		else if (base < 1) {
			startExpY = 0;
			endExpY = -expSliderFunc(1);
			output = -expSliderFunc(input) / endExpY;
		}
		break;
	}
	return output;
}

int ValueEvent::getStartVal() { return startValue; }
int ValueEvent::getEndVal() { return endValue; }
int ValueEvent::getStartRelVal() { return startRelativeValue; }
int ValueEvent::getEndRelVal() { return endRelativeValue; }