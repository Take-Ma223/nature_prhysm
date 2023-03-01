#pragma once
#include "STRUCT_OP.h"
#include "View.h"
#include "NPTimer.h"
#include "Processable.h"
#include "ActivityContext.h"
#include "Log.h"

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
		Log(L"onCreate\n");
		ActivityState activityState = ActivityState::Waiting;
	};
	virtual void onStart() {
		Log(L"onStart\n");
		ActivityState activityState = ActivityState::Running;
	};
	virtual void onPause() {
		Log(L"onPause\n");
		ActivityState activityState = ActivityState::Pausing;
	};
	virtual void onDestroy() {
		Log(L"onDestroy\n");
		//asset�������
	};

	void start();//��ʂ��J�n����Ƃ��ɐe��ʂ���Ă�

	// Processable ����Čp������܂���
	virtual void process() override {
		timerProccess();
		buttonProccess();
		logic();
		draw();
	}

	ActivityState activityState = ActivityState::Waiting;

	void startActivity(Activity* activity) {
		appContext->startActivity(activity);
	};
	void finish() {
		activityState = ActivityState::Finished;
	};

};

