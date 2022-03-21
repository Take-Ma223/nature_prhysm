#include "Animation.h"
#include <math.h>

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


point::point(int val, Specify specify) {
	point::val = val;
	point::different = val;
	point::specify = specify;
}

int point::getVal()
{
	return val;
}

Specify point::getSpecify()
{
	return specify;
}


event::event(point start, point end, Converter converter, double timeStart, double  timeEnd) {
	event::eventType = determinEventType(start, end);

	event::converter = converter;

	event::timeStart = timeStart;
	event::timeEnd = timeEnd;
}

EventType event::determinEventType(point p1, point p2)
{
	Specify s1 = p1.getSpecify();
	Specify s2 = p2.getSpecify();

	if (s1 == Abs && s2 == Abs) { 
		event::startValue = p1.getVal();
		event::endValue = p2.getVal();
		return EventType(AbsAbs); 
	}
	if (s1 == Abs && s2 == Rel) { 
		event::startValue = p1.getVal();
		event::endRelativeValue = p2.getVal();
		return EventType(AbsRel); 
	}
	if (s1 == Rel && s2 == Abs) { 
		event::startRelativeValue = p1.getVal();
		event::endValue = p2.getVal();
		return EventType(RelAbs); 
	}
	if (s1 == Rel && s2 == Rel) { 
		event::startRelativeValue = p1.getVal();
		event::endRelativeValue = p2.getVal();
		return EventType(RelRel); 
	}

}

void event::determinValueFrom(int nowVal, BOOL isReverse)
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
			startValue = endValue;//�t�Đ��s�\�Ȃ̂ŏI���l���瓮���Ȃ��悤�ɂ���
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

double event::easing(double input) {
	double output = 0;
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
		double startExpY = 1;
		double endExpY = 4;
		double base = converter.getParamA();

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
void event::setTime(int start, int end) {
	timeStart = start;
	timeEnd = end;
}

//getter
int event::getStartVal() { return startValue; }
int event::getEndVal() { return endValue; }
int event::getStartRelVal() { return startRelativeValue; }
int event::getEndRelVal() { return endRelativeValue; }
double event::getStartTime() { return timeStart; }
double event::getEndTime() { return timeEnd; }


//Animation�N���X

Animation::Animation(double* time) {
	nowTime = time;
}

int Animation::getValue() {
	return value;
}

void Animation::setValue(int input)
{
	value = input;
	return;
}

//�C�x���g�}��

void Animation::eChange(point start, point end, Converter converter, double startTime, double endTimeInput) {
	double endTime = endTimeInput;

	if (endTime < startTime) {
		endTime = startTime;
	}
	
	push(
		event(start, end, converter, startTime, endTime)
	);
}
void Animation::eChangeTo(point end, Converter converter, double startTime, double endTimeInput) {
	double endTime = endTimeInput;

	if (endTime < startTime) {
		endTime = startTime;
	}

	push(
		event(point(0, Rel), end, converter, startTime, endTime)
	);
}
void Animation::eSet(point val, double startTime) {
	push(
		event(val, val, Converter(Teleportation, 1), startTime, startTime)
	);
}
void Animation::eON(double startTime) {
	push(
		event(1, 1, Converter(Teleportation, 1), startTime, startTime)
	);
}
void Animation::eOFF(double startTime) {
	push(
		event(0, 0, Converter(Teleportation, 1), startTime, startTime)
	);
}


void Animation::clearEvent() {
	std::vector<event> empty;
	std::swap(transition, empty);
	//transition.clear();
	//playStartTime = 0;
}

void Animation::push(event input)
{
	transition.push_back(input);
	return;
}

int Animation::getLastIndex(){
	return transition.size() - 1;
}


//�v���[���[�֌W
void Animation::play()
{
	if (!transition.empty()) {
		if (!isReverse) {
			playStartTime = 0;
			playedTimeOfCaller = getNowTime();
			Animation::isPlay = TRUE;
			durationOffset = 0;
			eventIndex = 0;
			transition[eventIndex].determinValueFrom(value, isReverse);
		}
		else {
			playStartTime = transition.back().getEndTime();
			playedTimeOfCaller = getNowTime();
			Animation::isPlay = TRUE;
			durationOffset = 0;
			eventIndex = getLastIndex();
			transition[eventIndex].determinValueFrom(value, isReverse);
		}
	}
}

void Animation::stop()
{
	if (Animation::isPlay) {
		Animation::isPlay = FALSE;
	}
}

void Animation::resume()
{
	if (!Animation::isPlay) {
		playStartTime = playingTime;
		playedTimeOfCaller = getNowTime();
		durationOffset = 0;
		Animation::isPlay = TRUE;
	}
}

void Animation::reverse()
{
	if (Animation::isReverse) {
		Animation::isReverse = FALSE;
		if (!transition.empty()) {
			eventIndex = 0;
			transition[eventIndex].determinValueFrom(value, isReverse);
		}
	}
	else {
		Animation::isReverse = TRUE;
		if (!transition.empty()) {
			eventIndex = getLastIndex();
			transition[eventIndex].determinValueFrom(value, isReverse);
		}
	}
	playStartTime = playingTime;
	playedTimeOfCaller = getNowTime();
	durationOffset = 0;
}

void Animation::setReverse(BOOL isReverse)
{
	Animation::isReverse = isReverse;

	if (!Animation::isReverse) {
		if (!transition.empty()) {
			eventIndex = 0;
			transition[eventIndex].determinValueFrom(value, isReverse);
		}
	}
	else {
		if (!transition.empty()) {
			eventIndex = getLastIndex();
			transition[eventIndex].determinValueFrom(value, isReverse);
		}
	}

	playStartTime = playingTime;
	playedTimeOfCaller = getNowTime();
	durationOffset = 0;
}

void Animation::setLoop(BOOL isLoop)
{
	if (!transition.empty()) {
		if (transition.back().getEndTime() - 0 > 0) {//�A�j���[�V�����ɒ���������Ƃ������ύX����
			Animation::isLoop = isLoop;
		}
		else {
			Animation::isLoop = FALSE;
		}
	}
}

void Animation::setPlaySpeed(double playSpeed)
{
	Animation::playSpeed = playSpeed;
	playStartTime = playingTime;
	playedTimeOfCaller = getNowTime();
	durationOffset = 0;
}

void Animation::setStartTimeFromTime(int startTime)
{
	Animation::playStartTime = startTime;
	playingTime = playStartTime;
	playedTimeOfCaller = getNowTime();
	durationOffset = 0;
}

void Animation::setStartTimeFromRange(double startTimeRange)
{
	Animation::playStartTime = startTimeRange * transition.back().getEndTime();
	playingTime = playStartTime;
	playedTimeOfCaller = getNowTime();
	durationOffset = 0;
}


//�A�j���[�V��������
void Animation::update()
{
	if (!transition.empty()) {
		if (isPlay) {
			calculateWhereToPlay();
			decideWhichEventToProcess();
			value = calculateVal(transition[eventIndex]);
		}
	}
	return;
}

void Animation::calculateWhereToPlay() {
	//�ǂ����Đ�����ׂ����v�Z
	
	double duration = 0;
	auto calculateDuration = [&] {
		duration = getNowTime() - (playedTimeOfCaller + durationOffset);
		duration *= playSpeed;
	};

	if (!isReverse) {
		//playStartTime����̑��Ύ���
		calculateDuration();
		playingTime = playStartTime + duration;//playingTime�͕K���A�j���[�V�����̎��ԓ����w��

		while (playingTime > transition.back().getEndTime()) {//�A�j���[�V�����̍Ō�܂œ��B
			if (isLoop) {//���[�v�Đ���
				durationOffset += transition.back().getEndTime() / playSpeed;
				calculateDuration();
				eventIndex = 0;
				transition[eventIndex].determinValueFrom(transition.back().getEndVal(), isReverse);
				playingTime = playStartTime + duration;//playingTime�͕K���A�j���[�V�����̎��ԓ����w��
			}
			else {//�����V���b�g��
				//stop();//��~

				//�A�j���[�V�����̍Ō���w���悤�ɂ���
				playingTime = transition.back().getEndTime();
				break;
			}
		}
	}
	else {
		//playStartTime����̑��Ύ���
		calculateDuration();
		playingTime = playStartTime - duration;//playingTime�͕K���A�j���[�V�����̎��ԓ����w��

		while (playingTime < 0) {//�A�j���[�V�����̍ŏ��܂œ��B
			if (isLoop) {//���[�v�Đ���
				durationOffset += transition.back().getEndTime() / playSpeed;
				calculateDuration();
				eventIndex = getLastIndex();
				transition[eventIndex].determinValueFrom(transition.front().getStartVal(), isReverse);
				playingTime = playStartTime - duration;//playingTime�͕K���A�j���[�V�����̎��ԓ����w��
			}
			else {//�����V���b�g��
				//stop();//��~

				//�A�j���[�V�����̍ŏ����w���悤�ɂ���
				playingTime = transition.front().getStartTime();
				break;
			}
		}
	}
}

void Animation::decideWhichEventToProcess()
{
	//��������ׂ��C�x���g�ԍ������߂�

	if (!isReverse) {
		//���̃C�x���g�ԍ����K�؂��ǂ���
		while (1) {
			if ((transition[eventIndex].getStartTime() <= playingTime) && (playingTime < transition[eventIndex].getEndTime()) 
				&& ((transition[eventIndex].getStartTime()- transition[eventIndex].getEndTime()) != 0)) {//������0�ł͂Ȃ��C�x���g�̊J�n�I�����Ԃ͈͓̔�



				break;
			}

			eventIndex++;//���̃C�x���g������
			if (eventIndex == transition.size()) {//������������  �Ō�܂ŒT�� �Ō�̃C�x���g���w��
				eventIndex--;
				break;
			}
			transition[eventIndex].determinValueFrom(transition[eventIndex - 1].getEndVal(), isReverse);//���̃C�x���g��ʂ�̂őJ�ڒl�����߂�

			if (playingTime < transition[eventIndex].getStartTime()) {//���̃C�x���g�̊J�n���ԂɒB���Ă��Ȃ��@�C�x���g�̖�����Ԃɂ���ꍇ�͒��O�̃C�x���g���Q��
				eventIndex--;
				break;
			}
		}
	}
	else {
		//���̃C�x���g�ԍ����K�؂��ǂ���
		while (1) {
			if ((transition[eventIndex].getStartTime() < playingTime) && (playingTime <= transition[eventIndex].getEndTime())
				&& ((transition[eventIndex].getStartTime() - transition[eventIndex].getEndTime()) != 0)) {//������0�ł͂Ȃ��C�x���g�̊J�n�I�����Ԃ͈͓̔�
				break;
			}

			eventIndex--;//���̃C�x���g������
			if (eventIndex == -1) {//������������  �Ō�܂ŒT�� �Ō�̃C�x���g���w��
				eventIndex++;
				break;
			}
			transition[eventIndex].determinValueFrom(transition[eventIndex + 1].getStartVal(), isReverse);//���̃C�x���g��ʂ�̂őJ�ڒl�����߂�

			if (playingTime > transition[eventIndex].getEndTime()) {//���̃C�x���g�̊J�n���ԂɒB���Ă��Ȃ��@�C�x���g�̖�����Ԃɂ���ꍇ�͒��O�̃C�x���g���Q��
				eventIndex++;
				break;
			}
		}
	}
}

int Animation::calculateVal(event event) {
	//�J�ڎ��Ԍo�ߊ���
	double timeRatio = event.calculateTimeRatio(playingTime);

	//�J�ڗʂɂ�����l(���̒l�̕ω���őJ�ڂ̎d�����ς��)
	double multipleRatio = event.easing(timeRatio);

	double transitionAmount = (double)event.getEndVal() - event.getStartVal();

	return (int)((double)event.getStartVal() + transitionAmount * multipleRatio);
}

double Animation::getNowTime()
{
	return *nowTime;
}

