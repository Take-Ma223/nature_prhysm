#pragma once
#include "Asset.h"
#include "STRUCT_OP.h"

/** 
* ���W�N���X
*/
class Cordinate {
public:
	int x = 0;
	int y = 0;

	Cordinate(int x = 0, int y = 0);
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
class View
{
protected:
	ViewContext* vc;
	Cordinate cordinate;

public:
	View(ViewContext* vc, Cordinate cordinate);
	
	virtual void draw() = 0;//View�̕`�揈�� ���t���[���Ăт܂� �p����ŕK���������Ă�������

};

