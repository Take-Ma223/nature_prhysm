#pragma once
#include "Asset.h"
#include "STRUCT_OP.h"
#include "Drawable.h"

/** 
* ���W�N���X
*/
class Cordinate {
public:
	int x = 0;
	int y = 0;

	Cordinate(int x, int y);
};

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

public:
	/**
	* �p����ŃR���X�g���N�^���I�[�o�[���C�h����ۂɁA�g�p����Image,View�̃C���X�^���X��p�ӂ��鏈�����L�q���Ă�������
	*/
	View(ViewContext* vc, Cordinate cordinate);
	
	virtual void draw() override = 0;//View�̕`�揈�� ���t���[���Ăт܂� �p����ŕK���������Ă�������

};

