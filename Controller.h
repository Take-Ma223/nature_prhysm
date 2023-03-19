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
public:
	KeyState keyState = Release;
	bool isJustPressed = false;//�������u�Ԃ��ǂ���
	bool isJustReleased = false;//�������u�Ԃ��ǂ���

	void setIsPressCondition(function<bool(void)> condition);
	void setOnTapHandler(function<void(void)> handler);
	void setOnReleaseHandler(function<void(void)> handler);

	virtual void process() override {
		update();
	};
private:
	function<bool(void)> isPress;//���̉��z�L�[�������Ă���Ƃ݂Ȃ�����
	function<void(void)> onTapHandler = [] {return; };
	function<void(void)> onReleaseHandler = [] {return; };

	void update();
};


class Controller : public Processable
{
public:
	/// <summary>
	/// �p����ŉ��z�L�[��p�ӂ��A�ǉ����Ă�������
	/// </summary>
	Controller() {};

	virtual void process() override {
		updateInput();
	};

protected:
	KeyInput keyInput[256];

	/// <summary>
	/// ���z�L�[�̒ǉ�
	/// </summary>
	/// <param name="k">���z�L�[</param>
	void addVirtualKeyInput(VirtualKeyInput* k) {
		virtualKeyInputs.push_back(k);
	}

private:
	char keyInputBuffer[256];

	void updateInput();//�S�L�[�̍X�V
	void updateVirtualInput();//���z�L�[�̍X�V
	std::vector<VirtualKeyInput*> virtualKeyInputs;
};

