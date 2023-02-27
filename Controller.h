#pragma once
#include "DxLib.h"
#include <functional>
#include <vector>
#include "Processable.h"

using namespace std;

enum KeyState
{
	Press,//�����Ă���
	Release,//�����Ă���
};

class KeyInput
{
public:
	KeyState keyState = Release;
};


/**
* �L�[�R���t�B�O�╡���̃L�[����̃L�[�Ƃ��Ĉ������߂̉��z�L�[���̓N���X
*/
class VirtualKeyInput : public Processable {
	function<bool(void)> isPress;//���̉��z�L�[�������Ă���Ƃ݂Ȃ�����
	function<void(void)> onTapHandler = [] {};
	function<void(void)> onReleaseHandler = [] {};

	void update();
public:
	KeyState keyState = Release;
	bool isJustPressed = false;//�������u�Ԃ��ǂ���
	bool isJustReleased = false;//�������u�Ԃ��ǂ���

	void setIsPressCondition(function<bool(void)> condition);
	void setOnTapHandler(function<void(void)> handler);
	void setOnReleaseHandler(function<void(void)> handler);

	virtual void process() override;

};


class Controller : public Processable
{
	char keyInputBuffer[256];
	
	void updateInput();//�S�L�[�̍X�V
	void updateVirtualInput();//���z�L�[�̍X�V
	std::vector<VirtualKeyInput*> virtualKeyInputs;
protected:
	KeyInput keyInput[256];

	/*
	* ���z�L�[�̒ǉ�
	*/
	void addVirtualKeyInput(VirtualKeyInput* k) {
		virtualKeyInputs.push_back(k);
	}

public:
	Controller();

	//�p����ŉ��z�L�[��p�ӂ��A�ǉ����Ă�������
	virtual void addKey() = 0;

	virtual void process() override;
};

