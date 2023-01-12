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
private:
	int screenHandle = 0;//����View�̉摜�n���h��
	Size size = Size(1, 1);//����View�̃T�C�Y makeScreen���s���ɕύX����܂�
	void deleteGraph();

protected:
	ViewContext* vc;
	Cordinate cordinate = Cordinate(0,0);
	
	void makeScreen(Size s);//����View��`�悷��X�N���[���̗p�� �p����̃R���X�g���N�^�ŌĂяo���Ă������� s:�p�ӂ���T�C�Y 

public:
	/**
	* �p����ŃR���X�g���N�^���I�[�o�[���C�h����ۂɁA�g�p����Image,View�̃C���X�^���X��p�ӂ��鏈�����L�q���Ă�������
	*/
	View(ViewContext* vc, Cordinate cordinate);
	~View();

	void draw(int drawScreen) override;//View�̕`�揈���̊J�n ���t���[���Ăт܂� 
	void setScreen(int drawScreen) override;//�`���X�N���[���̎w��
	virtual void putContents(int drawScreen) override = 0;//�`�揈�� �p����ŕK���������Ă�������
};

