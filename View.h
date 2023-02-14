#pragma once
#include "Asset.h"
#include "STRUCT_OP.h"
#include "Drawable.h"
#include "Geometry.h"

/**
* �r���[�N���X �S�Ẵr���[�̊��N���X
*/
class View : public Drawable
{
private:
	void deleteGraph();

protected:
	void makeScreen(Size s);//����View��`�悷��X�N���[���̗p�� �p����̃R���X�g���N�^�ŌĂяo���Ă������� s:�p�ӂ���T�C�Y 
	virtual void prepareScreen(int drawScreen) = 0;//draw���Ă΂�邽�тɂ���View�̃X�N���[����p�ӂ��鏈�� �p����ŕK���������Ă�������

public:
	View() : Drawable() {}
	/**
	* �p����ŃR���X�g���N�^���I�[�o�[���C�h����ۂɁA�g�p����Image,View�̃C���X�^���X��p�ӂ��鏈�����L�q���Ă�������
	*/
	View(DrawableContext* dc, DrawableInitParam param = DrawableInitParam()) : Drawable(dc, param) {
	}
	View(Drawable* parent, DrawableContext* dc, DrawableInitParam param = DrawableInitParam()) : Drawable(parent, dc, param) {
	}
	~View() {
		deleteGraph();
	}

	void draw(int drawScreen) override;//View�̕`�揈���̊J�n ���t���[���Ăт܂� 
};

