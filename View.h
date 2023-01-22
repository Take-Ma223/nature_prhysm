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
class View : public Drawable
{
private:
	void deleteGraph();

protected:
	ViewContext* vc;
	Cordinate cordinate = Cordinate(0,0);
	
	void makeScreen(Size s);//����View��`�悷��X�N���[���̗p�� �p����̃R���X�g���N�^�ŌĂяo���Ă������� s:�p�ӂ���T�C�Y 
	virtual void prepareScreen(int drawScreen) = 0;//draw���Ă΂�邽�тɂ���View�̃X�N���[����p�ӂ��鏈�� �p����ŕK���������Ă�������

public:
	/**
	* �p����ŃR���X�g���N�^���I�[�o�[���C�h����ۂɁA�g�p����Image,View�̃C���X�^���X��p�ӂ��鏈�����L�q���Ă�������
	*/
	View(ViewContext* vc, DrawableInitParam param = DrawableInitParam()) : Drawable(vc->time, param) {
		View::vc = vc;
		View::cordinate = cordinate;
	}
	~View() {
		deleteGraph();
	}

	void draw(int drawScreen) override;//View�̕`�揈���̊J�n ���t���[���Ăт܂� 
};

