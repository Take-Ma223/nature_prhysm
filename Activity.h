#pragma once
#include "STRUCT_OP.h"
#include "View.h"
#include "NPTimer.h"

/**
* �A�N�e�B�r�e�B�N���X�̑O��ƂȂ�R���e�L�X�g�N���X
*/
class ActivityContext {
public:
	Option* option;
	Config* config;

	ActivityContext(Option* option = NULL, Config* config = NULL);
};


/**
* ��ʃN���X
* �p�����Ďg���܂�
*/
class Activity
{
	DrawableContext dContext;
	NPTimer timer;

protected:
	ActivityContext* ac;
	double GAME_start_time = 0;
	double GAME_passed_time = 0;//�P�ʂ�ms �o�ߎ���

	double LOOP_passed_time = 1;//1���[�v�ɂ�����������(ms)
	double time_cash = 0;//LOOP_passed_time���Z�o���邽�߂̋L���ϐ�


	virtual void initLogic() = 0;//���W�b�N�����̏���
	virtual void initViewHandlers() = 0;//�eView�̃n���h���̐ݒ�
	virtual void onStart() = 0;//��ʊJ�n����u�Ԃ̏���(���܂�d�������͏����Ȃ�)

	virtual void initTimer();//�^�C�}�[�̏�����


	virtual void initButton();//�{�^�������̏�����

	void loop();//���C�����[�v
	virtual void timerProccess();//�^�C�}�[�̌v�Z����
	virtual void buttonProccess();//�{�^���̏���
	virtual void logic() = 0;//���t���[���̃��W�b�N����
	virtual void draw() = 0;//�`�揇�Ԃ��w��
public:
	Activity(ActivityContext* ac);
	void start();//��ʂ��J�n����Ƃ��ɐe��ʂ���Ă�





};

