#pragma once
#include <functional>
#include <vector>

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
	KeyInput();
};


struct KeyBehavior
{

};



/**
* �L�[�R���t�B�O�╡���̃L�[����̃L�[�Ƃ��Ĉ������߂̉��z�L�[���̓N���X
*/
class VirtualKeyInput {
	function<void(void)> onTapHandler = [] {};
	function<void(void)> onReleaseHandler = [] {};

public:
	KeyState keyState = Release;
	bool isJustPressed = false;//�������u�Ԃ��ǂ���
	bool isJustReleased = false;//�������u�Ԃ��ǂ���

	void setOnTapHandler(function<void(void)> handler);
	void setOnReleaseHandler(function<void(void)> handler);

	void onTap();
	void onRelease();

	void update();

	function<bool(void)> isPress;//���̉��z�L�[�������Ă���Ƃ݂Ȃ�����
};


class Controller
{
	char keyInputBuffer[256];
	KeyInput keyInput[256];

	//�p�����VirtualKeyInput��K�v�Ȃ����錾����
public:
	//�p�����VirtualKeyInput��isPress��ݒ肷��
	Controller();

	void updateInput();//���t���[���Ă�

	/**
	* ���z�L�[���͂𐧌䂷�鏈�����p����ŃI�[�o�[���C�h����
	*/
	virtual void updateVirtualInput() = 0;

	//�p����ŉ��z�L�[�̃n���h���Z�b�g���\�b�h��K�v�Ȃ����p�ӂ���

};

