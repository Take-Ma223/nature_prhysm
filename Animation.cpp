#include "Animation.h"
#include <math.h>

point::point(int val, Specify specify) {
	point::val = val;
	point::different = val;
	point::specify = specify;
}

void point::determinValueFrom(int nowVal, BOOL isReverse)
{
	if (specify == Rel) {
		if (isReverse) {
			val = nowVal - different;
		}
		else {
			val = nowVal + different;
		}

	}
}

int point::getVal()
{
	return val;
}


event::event(point start, point end, ConvertMode mode, double base, double timeStart, double  timeEnd) {
	event::start = start;
	event::end = end;

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

double event::easing(double input) {
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
void event::setMode(ConvertMode mode, double base) {
	event::mode = mode;
	event::base = base;
}
void event::setTime(int start, int end) {
	timeStart = start;
	timeEnd = end;
}

void event::determinValueFrom(int nowVal, BOOL isReverse)
{
	start.determinValueFrom(nowVal, isReverse);
	end.determinValueFrom(nowVal, isReverse);
}

//getter
int event::getStartVal() { return start.getVal(); }
int event::getEndVal() { return end.getVal(); }
ConvertMode event::getMode() { return mode; }
double event::getBase() { return base; }
double event::getTimeStart() { return timeStart; }
double event::getTimeEnd() { return timeEnd; }


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

void Animation::eChange(point start, point end, ConvertMode mode, double durationTime, double startTime, double base) {
	push(
		event(start, end, mode, base, startTime, startTime + durationTime)
	);
}
void Animation::eChangeTo(point end, ConvertMode mode, double durationTime, double startTime, double base) {
	push(
		event(point(0, Rel), end, mode, base, startTime, startTime + durationTime)
	);
}
void Animation::eSet(point val, double startTime) {
	push(
		event(val, val, Teleportation, 1, startTime, startTime)
	);
}
void Animation::eON(double startTime) {
	push(
		event(0, 1, Teleportation, 1, startTime, startTime)
	);
}
void Animation::eOFF(double startTime) {
	push(
		event(1, 0, Teleportation, 1, startTime, startTime)
	);
}


void Animation::clearEvent() {
	std::vector<event> empty;
	std::swap(transition, empty);
	//transition.clear();
	playStartTime = 0;
	isLoop = FALSE;
}

void Animation::push(event input)
{
	transition.push_back(input);
	return;
}


//�v���[���[�֌W


void Animation::play()
{
	if (!transition.empty()) {
		playStartTime = 0;
		playedTimeOfCaller = getNowTime();
		Animation::isPlay = TRUE;
		eventIndex = 0;
		durationBuf = 0;
		transition[eventIndex].determinValueFrom(value, isReverse);
	}
}

void Animation::stop()
{
	Animation::isPlay = FALSE;
}

void Animation::resume()
{
	playStartTime = playingTime;
	Animation::isPlay = TRUE;
}

void Animation::reverse()
{
	if (Animation::isReverse) {
		Animation::isReverse = FALSE;
	}
	else {
		Animation::isReverse = TRUE;
	}
	playStartTime = playingTime;
	playedTimeOfCaller = getNowTime();
}

void Animation::setReverse(BOOL isReverse)
{
	Animation::isReverse = isReverse;
	playStartTime = playingTime;
	playedTimeOfCaller = getNowTime();
}

void Animation::setLoop(BOOL isLoop)
{
	if (!transition.empty()) {
		if (transition.back().getTimeEnd() - transition.front().getTimeStart() > 0) {//�A�j���[�V�����ɒ���������Ƃ������ύX����
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
}


//�A�j���[�V��������
void Animation::update()
{
	if (!transition.empty()) {

		if (isPlay) {
			//�ǂ����Đ�����ׂ����v�Z
			//playStartTime����̑��Ύ���
			double duration = getNowTime() - playedTimeOfCaller - durationBuf;
			playingTime = playStartTime + duration;//playingTime�͕K���A�j���[�V�����̎��ԓ����w��

			while (playStartTime + duration > transition.back().getTimeEnd()) {//�A�j���[�V�����̍Ō�܂œ��B
				if (isLoop) {//���[�v�Đ���
					durationBuf += transition.back().getTimeEnd();
					duration = getNowTime() - playedTimeOfCaller - durationBuf;

					
					eventIndex = 0;

					transition[eventIndex].determinValueFrom(transition.back().getEndVal(), isReverse);

					playingTime = playStartTime + duration;//playingTime�͕K���A�j���[�V�����̎��ԓ����w��
				}
				else {//�����V���b�g��
					stop();//��~

					//�A�j���[�V�����̍Ō���w���悤�ɂ���
					playingTime = transition.back().getTimeEnd();
					break;
				}
			}
			
			

			//��������ׂ��C�x���g�ԍ������߂�
			//���̃C�x���g�ԍ����K�؂��ǂ���
			while (1) {
				if ((transition[eventIndex].getTimeStart() <= playingTime) && (playingTime <= transition[eventIndex].getTimeEnd())) {//�C�x���g�̊J�n�I�����Ԃ͈͓̔�
					break;
				}

				eventIndex++;//���̃C�x���g������
				if (eventIndex == transition.size()) {//������������  �Ō�܂ŒT�� �Ō�̃C�x���g���w��
					eventIndex--;
					break;
				}
				transition[eventIndex].determinValueFrom(transition[eventIndex - 1].getEndVal(), isReverse);

				if (playingTime < transition[eventIndex].getTimeStart()) {//���̃C�x���g�̊J�n���ԂɒB���Ă��Ȃ��@�C�x���g�̖�����Ԃɂ���ꍇ�͒��O�̃C�x���g���Q��
					eventIndex--;
					break;
				}

			}
		
		}
		value = calculateVal(transition[eventIndex]);
	}
	return;
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




