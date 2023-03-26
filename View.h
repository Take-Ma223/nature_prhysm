#pragma once
#include "Asset.h"
#include "STRUCT_OP.h"
#include "Drawable.h"
#include "Geometry.h"
#include <stack>
#include "ActivityContext.h"

/// <summary>
/// �r���[�N���X �S�Ẵr���[�̊��N���X
/// </summary>
class View : public Drawable
{
private:
	void deleteGraph();
	std::vector<Drawable*> drawables;

protected:
	void addDrawable(Drawable* d) {
		drawables.push_back(d);
	}

	/// <summary>
	/// ����View��`�悷��X�N���[���̗p�� �p����̃R���X�g���N�^�ŌĂяo���Ă�������
	/// </summary>
	/// <param name="s">�p�ӂ���T�C�Y </param>
	void makeScreen(Size s);

	/// <summary>
	/// draw���Ă΂�邽�тɂ���View�̃X�N���[����p�ӂ��鏈�� �p����ŕK���������Ă�������
	/// </summary>
	/// <param name="drawScreen">�`���</param>
	virtual void beforeDrawProcess(int drawScreen) = 0;

public:
	View() : Drawable() {}

	/// <summary>
	/// �p����ŃR���X�g���N�^���I�[�o�[���C�h����ۂɁA�g�p����Image,View�̃C���X�^���X��p�ӂ��鏈�����L�q���Ă�������
	/// </summary>
	/// <param name="c"></param>
	/// <param name="param"></param>
	View(ActivityContext* c, DrawableInitParam param = DrawableInitParam()) : Drawable(c, param) {
	}
	View(Drawable* parent, ActivityContext* c, DrawableInitParam param = DrawableInitParam()) : Drawable(parent, c, param) {
	}
	~View() {
		deleteGraph();
		drawables.clear();
	}

	/// <summary>
	/// View�̕`�揈���̊J�n ���t���[���Ăт܂�
	/// </summary>
	void draw() override;

	/// <summary>
	/// View�̕`�揈���̊J�n ���t���[���Ăт܂� 
	/// </summary>
	/// <param name="drawScreen">�`���</param>
	void draw(int drawScreen) override;
};

