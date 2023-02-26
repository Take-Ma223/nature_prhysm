#pragma once
#include "STRUCT_OP.h"
#include "View.h"
#include "NPTimer.h"
#include "Processable.h"
#include "ActivityContext.h"

class AppContext;//�v���g�^�C�v�錾

enum class ActivityState
{
	Waiting, //�ҋ@��
	Running, //���s��
	Pausing, //��~��
	Finished //�I���ς�
};


/**
* ��ʃN���X
* �p�����Ďg���܂�
*/
class Activity : public Processable
{
	AppContext* appContext;

	Asset asset = Asset();
protected:
	ActivityContext activityContext;
	double GAME_start_time = 0;
	double GAME_passed_time = 0;//�P�ʂ�ms �o�ߎ���

	double LOOP_passed_time = 1;//1���[�v�ɂ�����������(us)
	double time_cash = 0;//LOOP_passed_time���Z�o���邽�߂̋L���ϐ�


	virtual void initLogic() {};//���W�b�N�����̏���
	virtual void initViewHandlers() {};//�eView�̃n���h���̐ݒ�
	virtual void initTimer();//�^�C�}�[�̏�����
	virtual void initButton();//�{�^�������̏�����

	virtual void timerProccess();//�^�C�}�[�̌v�Z����
	virtual void buttonProccess();//�{�^���̏���
	virtual void logic() {};//���t���[���̃��W�b�N����
	virtual void draw() {};//�`�揇�Ԃ��w��
public:
	Activity(AppContext* appContext) {
		Activity::appContext = appContext;
		Activity::activityContext = ActivityContext(appContext, &asset);
	};

	virtual void onCreate() {
		ActivityState activityState = ActivityState::Waiting;
	};
	virtual void onStart() {
		ActivityState activityState = ActivityState::Running;
	};
	virtual void onPause() {
		ActivityState activityState = ActivityState::Pausing;
	};
	virtual void onDestroy() {
		//asset�������
	};

	void start();//��ʂ��J�n����Ƃ��ɐe��ʂ���Ă�

	// Processable ����Čp������܂���
	virtual void process() override;

	ActivityState activityState = ActivityState::Waiting;

	void startActivity(Activity* activity) {
		appContext->startActivity(activity);
	};
	void finish() {
		activityState = ActivityState::Finished;
	};

};

