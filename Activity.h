#pragma once
#include "STRUCT_OP.h"
#include "View.h"
#include "NPTimer.h"
#include "Processable.h"
#include "ActivityContext.h"
#include "Log.h"

class AppContext;//�v���g�^�C�v�錾

/// <summary>
/// �A�N�e�B�r�e�B�̏��
/// </summary>
enum class ActivityState
{
	Waiting, //�ҋ@��
	Running, //���s��
	Pausing, //��~��
	Finished //�I���ς�
};


/// <summary>
/// �A�N�e�B�r�e�B�N���X
/// �p�����Ċe��ʂ����܂�
/// </summary>
class Activity : public Processable
{
public:
	ActivityState activityState = ActivityState::Waiting;

	virtual ~Activity() {};
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

	/// <summary>
	/// ��ʂ��J�n����Ƃ��ɐe��ʂ���Ă�
	/// </summary>
	void start() {
		initLogic();
		initViewHandlers();
		initButton();

		initTimer();
		onStart();
	};

	// Processable ����Čp������܂���
	virtual void process() override {
		timerProcess();
		buttonProcess();
		logic();
		draw();
	}

	/// <summary>
	/// ���̃A�N�e�B�r�e�B�ɑJ��
	/// </summary>
	/// <param name="activity">�J�ڐ�A�N�e�B�r�e�B</param>
	void startActivity(Activity* activity) {
		appContext->startActivity(activity);
	};

	/// <summary>
	/// ���̃A�N�e�B�r�e�B�̏I��
	/// </summary>
	void finish() {
		activityState = ActivityState::Finished;
	};

protected:
	AppContext* appContext;
	ActivityContext activityContext;
	double GAME_start_time = 0;
	double GAME_passed_time = 0;//�P�ʂ�ms �o�ߎ���

	double LOOP_passed_time = 1;//1���[�v�ɂ�����������(us)
	double time_cash = 0;//LOOP_passed_time���Z�o���邽�߂̋L���ϐ�


	virtual void initLogic() {};//���W�b�N�����̏���
	virtual void initViewHandlers() {};//�eView�̃n���h���̐ݒ�

	/// <summary>
	/// �^�C�}�[�̏�����
	/// </summary>
	virtual void initTimer() {
		GAME_start_time = activityContext.getTime();
	};
	virtual void initButton();//�{�^�������̏�����

	/// <summary>
	/// �^�C�}�[�̌v�Z����
	/// </summary>
	virtual void timerProcess() {
		GAME_passed_time = activityContext.getTime() - GAME_start_time;//�o�ߎ��Ԍv�Z
		LOOP_passed_time = GAME_passed_time - time_cash;//1���[�v�ɂ����������Ԃ��Z�o
		time_cash = GAME_passed_time;
	};

	virtual void buttonProcess();//�{�^���̏���
	virtual void logic();//���t���[���̃��W�b�N����
	virtual void draw();//�`�揇�Ԃ��w��

private:
	Asset asset = Asset();
};

