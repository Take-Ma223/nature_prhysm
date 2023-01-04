#pragma once
#include "Asset.h"
#include "STRUCT_OP.h"
#include "Drawable.h"
#include "Geometry.h"

/**
* �r���[�N���X�̑O��ƂȂ�R���e�L�X�g�N���X
* ��ʓ��ŋ��L����Ă������
*/
class ViewContext {
public:
	Asset* asset;
	Option* option;
	double* time;

	ViewContext(Asset* asset = NULL, Option* option = NULL, double* time = NULL);
};



/**
* �r���[�N���X �S�Ẵr���[�̊��N���X
*/
class View : Drawable
{
	
protected:
	ViewContext* vc;
	Cordinate cordinate = Cordinate(0,0);

	ImageHandle handle;//�\������摜�n���h�����

	virtual Size viewSize() = 0;//�p����ł���View�̃T�C�Y���w�肵�Ă�������
	void makeScreen(Size s);//����View��`�悷��X�N���[���̗p�� �p����̃R���X�g���N�^�ŌĂяo���Ă�������

public:
	/**
	* �p����ŃR���X�g���N�^���I�[�o�[���C�h����ۂɁA�g�p����Image,View�̃C���X�^���X��p�ӂ��鏈�����L�q���Ă�������
	*/
	View(ViewContext* vc, Cordinate cordinate);

	void draw(int drawScreen) override;//View�̕`�揈�� ���t���[���Ăт܂� �p����ŕK���������Ă�������
	void setScreen(int drawScreen) override;
	virtual void putContents(int drawScreen) override = 0;
};

