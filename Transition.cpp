#include "Transition.h"
#include <math.h>


Transition::Transition(double* time) {
	nowTime = time;
}

void Transition::clearEvent() {
	std::vector<Event*> empty;
	
	//���������
	for (const auto& e : transition) {
		delete[] e;
	}

	std::swap(transition, empty);



	//transition.clear();
	//playStartTime = 0;
}

void Transition::push(Event* input)
{
	transition.push_back(input);
	return;
}

int Transition::getLastIndex(){
	return transition.size() - 1;
}


//�v���[���[�֌W
void Transition::play()
{
	if (!transition.empty()) {
		if (!isReverse) {
			playStartTime = 0;
			playedTimeOfCaller = getNowTime();
			Transition::isPlay = TRUE;
			durationOffset = 0;
			eventIndex = 0;

			onPlay(isReverse);
			//transition[eventIndex].determinValueFrom(value, isReverse);
			//transition[eventIndex].handler();
		}
		else {
			playStartTime = transition.back()->getEndTime();
			playedTimeOfCaller = getNowTime();
			Transition::isPlay = TRUE;
			durationOffset = 0;
			eventIndex = getLastIndex();

			onPlay(isReverse);
			//transition[eventIndex].determinValueFrom(value, isReverse);
			//transition[eventIndex].handler();
		}
	}
}

void Transition::stop()
{
	if (Transition::isPlay) {
		Transition::isPlay = FALSE;
	}
}

void Transition::resume()
{
	if (!Transition::isPlay) {
		playStartTime = playingTime;
		playedTimeOfCaller = getNowTime();
		durationOffset = 0;
		Transition::isPlay = TRUE;
	}
}

void Transition::reverse()
{
	if (Transition::isReverse) {
		Transition::isReverse = FALSE;
		if (!transition.empty()) {
			eventIndex = 0;

			onReverse(isReverse);
			//transition[eventIndex].determinValueFrom(value, isReverse);
		}
	}
	else {
		Transition::isReverse = TRUE;
		if (!transition.empty()) {
			eventIndex = getLastIndex();

			onReverse(isReverse);
			//transition[eventIndex].determinValueFrom(value, isReverse);
		}
	}
	playStartTime = playingTime;
	playedTimeOfCaller = getNowTime();
	durationOffset = 0;
}

void Transition::setReverse(BOOL isReverse)
{
	Transition::isReverse = isReverse;

	if (!Transition::isReverse) {
		if (!transition.empty()) {
			eventIndex = 0;

			onReverse(isReverse);
			//transition[eventIndex].determinValueFrom(value, isReverse);
		}
	}
	else {
		if (!transition.empty()) {
			eventIndex = getLastIndex();

			onReverse(isReverse);
			//transition[eventIndex].determinValueFrom(value, isReverse);
		}
	}

	playStartTime = playingTime;
	playedTimeOfCaller = getNowTime();
	durationOffset = 0;
}

void Transition::setLoop(BOOL isLoop)
{
	if (!transition.empty()) {
		if (transition.back()->getEndTime() - 0 > 0) {//�g�����W�V�����ɒ���������Ƃ������ύX����
			Transition::isLoop = isLoop;
		}
		else {
			Transition::isLoop = FALSE;
		}
	}
}

void Transition::setPlaySpeed(double playSpeed)
{
	Transition::playSpeed = playSpeed;
	playStartTime = playingTime;
	playedTimeOfCaller = getNowTime();
	durationOffset = 0;
}

void Transition::setStartTimeFromTime(int startTime)
{
	Transition::playStartTime = startTime;
	playingTime = playStartTime;
	playedTimeOfCaller = getNowTime();
	durationOffset = 0;
}

void Transition::setStartTimeFromRange(double startTimeRange)
{
	Transition::playStartTime = startTimeRange * transition.back()->getEndTime();
	playingTime = playStartTime;
	playedTimeOfCaller = getNowTime();
	durationOffset = 0;
}

void Transition::process()
{
	update();
}


//�g�����W�V��������
void Transition::update()
{
	if (!transition.empty()) {
		if (isPlay) {
			calculateWhereToPlay();
			decideWhichEventToProcess();
			onUpdate();
			//value = calculateVal(transition[eventIndex]);
		}
	}
	return;
}

void Transition::calculateWhereToPlay() {
	//�ǂ����Đ�����ׂ����v�Z
	
	double duration = 0;
	auto calculateDuration = [&] {
		duration = getNowTime() - (playedTimeOfCaller + durationOffset);
		duration *= playSpeed;
	};

	if (!isReverse) {
		//playStartTime����̑��Ύ���
		calculateDuration();
		playingTime = playStartTime + duration;//playingTime�͕K���g�����W�V�����̎��ԓ����w��

		while (playingTime >= transition.back()->getEndTime()) {//�g�����W�V�����̍Ō�܂œ��B
			if (isLoop) {//���[�v�Đ���
				durationOffset += transition.back()->getEndTime() / playSpeed;
				calculateDuration();
				eventIndex = 0;

				onLoop(isReverse);
				//transition[eventIndex].determinValueFrom(transition.back().getEndVal(), isReverse);

				playingTime = playStartTime + duration;//playingTime�͕K���g�����W�V�����̎��ԓ����w��
			}
			else {//�����V���b�g��
				//stop();//��~

				//�g�����W�V�����̍Ō���w���悤�ɂ���
				playingTime = transition.back()->getEndTime();
				break;
			}
		}
	}
	else {
		//playStartTime����̑��Ύ���
		calculateDuration();
		playingTime = playStartTime - duration;//playingTime�͕K���g�����W�V�����̎��ԓ����w��

		while (playingTime <= 0) {//�g�����W�V�����̍ŏ��܂œ��B
			if (isLoop) {//���[�v�Đ���
				durationOffset += transition.back()->getEndTime() / playSpeed;
				calculateDuration();
				eventIndex = getLastIndex();

				onLoop(isReverse);
				//transition[eventIndex].determinValueFrom(transition.front().getStartVal(), isReverse);

				playingTime = playStartTime - duration;//playingTime�͕K���g�����W�V�����̎��ԓ����w��
			}
			else {//�����V���b�g��
				//stop();//��~

				//�g�����W�V�����̍ŏ����w���悤�ɂ���
				playingTime = transition.front()->getStartTime();
				break;
			}
		}
	}
}

void Transition::decideWhichEventToProcess()
{
	//��������ׂ��C�x���g�ԍ������߂�

	if (!isReverse) {
		//���̃C�x���g�ԍ����K�؂��ǂ���
		while (1) {
			if ((transition[eventIndex]->getStartTime() <= playingTime) && (playingTime < transition[eventIndex]->getEndTime())
				&& ((transition[eventIndex]->getStartTime()- transition[eventIndex]->getEndTime()) != 0)) {//������0�ł͂Ȃ��C�x���g�̊J�n�I�����Ԃ͈͓̔�



				break;
			}

			eventIndex++;//���̃C�x���g������
			if (eventIndex == transition.size()) {//������������  �Ō�܂ŒT�� �Ō�̃C�x���g���w��
				eventIndex--;
				break;
			}

			onThrough(isReverse);
			//transition[eventIndex].determinValueFrom(transition[eventIndex - 1].getEndVal(), isReverse);//���̃C�x���g��ʂ�̂őJ�ڒl�����߂�

			if (playingTime < transition[eventIndex]->getStartTime()) {//���̃C�x���g�̊J�n���ԂɒB���Ă��Ȃ��@�C�x���g�̖�����Ԃɂ���ꍇ�͒��O�̃C�x���g���Q��
				eventIndex--;
				break;
			}

			onNext(isReverse);
		}
	}
	else {
		//���̃C�x���g�ԍ����K�؂��ǂ���
		while (1) {
			if ((transition[eventIndex]->getStartTime() < playingTime) && (playingTime <= transition[eventIndex]->getEndTime())
				&& ((transition[eventIndex]->getStartTime() - transition[eventIndex]->getEndTime()) != 0)) {//������0�ł͂Ȃ��C�x���g�̊J�n�I�����Ԃ͈͓̔�
				break;
			}

			eventIndex--;//���̃C�x���g������
			if (eventIndex == -1) {//������������  �Ō�܂ŒT�� �Ō�̃C�x���g���w��
				eventIndex++;
				break;
			}

			onThrough(isReverse);
			//transition[eventIndex].determinValueFrom(transition[eventIndex + 1].getStartVal(), isReverse);//���̃C�x���g��ʂ�̂őJ�ڒl�����߂�

			if (playingTime > transition[eventIndex]->getEndTime()) {//���̃C�x���g�̊J�n���ԂɒB���Ă��Ȃ��@�C�x���g�̖�����Ԃɂ���ꍇ�͒��O�̃C�x���g���Q��
				eventIndex++;
				break;
			}

			onNext(isReverse);
		}
	}
}



double Transition::getNowTime()
{
	return *nowTime;
}

