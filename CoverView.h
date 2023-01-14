#pragma once
#include "Image.h"
#include "TransValue.h"
#include "View.h"

enum CoverViewPhase{
	Start,
	Play,
	Finish
};

class CoverView : public View
{
	int XLeftCoverOpen = -320;
	int XLeftCoverClose = 0;
	int YLeftCoverOpen = 0;
	int YLeftCoverClose = 0;

	int XMiddleCoverOpen = 320;
	int XMiddleCoverClose = 320;
	int YMiddleCoverOpen = -720;
	int YMiddleCoverClose = 0;

	int XRightCoverOpen = 960;
	int XRightCoverClose = 1280;
	int YRightCoverOpen = 0;
	int YRightCoverClose = 0;

	int transitionDurationTime = 833;

	int transitionDurationTimeWindbreak = 750;

	const double functionXStart = 0.09384164222873900293255131964809;
	const double functionXRange = 0.90615835777126099706744868035191;
	const double functionYstart = 0.00880625381618665130158839363266;
	const double functionYRange = 0.99119374618381334869841160636734;


	const int CoverMoveRange = 618;

	int CoverFirstRatio = 0;//�ŏ��ɕ��銄��

	int easing(int x);//0~1000��0~618�ɓ񎟊֐��ŕϊ�
	int easingInvert(int x);//easing�̋t�ϊ�

	CoverViewPhase phase = Start;

	void setMoveSpeedAnimation();

	void drawLeftCover(int drawScreen);
	void drawMiddleCover(int drawScreen);
	void drawRightCover(int drawScreen);

	int sizeX = 1280;
	int sizeY = 720;
public:
	TransValue CloseRatio;//�J�o�[���Ă闦(1000����)
	TransValue MoveSpeed;//�J�o�[�J��1000�����X�s�[�h(�L�[�{�[�h���쎞)�������u�Ԃ����x�������悤�ɂ���


	Image leftCover;
	Image middleCover;
	Image rightCover;

	CoverView::CoverView(ViewContext* vc, DrawableInitParam param = DrawableInitParam()) : View(vc, param)
	{
		makeScreen(Size(sizeX, sizeY));

		wstring themeStr1(L"img/themes/");
		wstring themeStr2(vc->option->theme[vc->option->op.theme]);

		ImageHandle leftCoverHandle = ImageHandle();
		ImageHandle  middleCoverHandle = vc->asset->img((themeStr1 + themeStr2 + wstring(L"/cover_middle.png")).c_str());
		ImageHandle  rightCoverHandle = ImageHandle();

		DrawableInitParam p1;
		p1.cordinate = Cordinate(XLeftCoverClose, YLeftCoverClose);
		DrawableInitParam p2;
		p2.cordinate = Cordinate(XMiddleCoverClose, YMiddleCoverClose);
		DrawableInitParam p3;
		p3.cordinate = Cordinate(XRightCoverClose, YRightCoverClose);


		leftCover = Image(leftCoverHandle, vc->time, p1);
		middleCover = Image(middleCoverHandle, vc->time, p2);
		rightCover = Image(rightCoverHandle, vc->time, p3);

		CloseRatio = TransValue(vc->time);
		MoveSpeed = TransValue(vc->time);
		setMoveSpeedAnimation();
	}

	void prepareScreen(int drawScreen) override;


	void closeAll();
	void openAll();

	void startMiddleCover();//�J�n���ɃJ�o�[���グ�鏈��
	void finishMiddleCover();//�I�����Ɍ��ݒl����J�o�[�������鏈��

	void startWindbreak();//���t���ɃJ�o�[�グ�������鏈���̋���
	void setOpenRange(int openRatio);//���t���̌��݂̃J�o�[�ʒu�̎w��(1000�����w��)
	void setFirstOpenRange(int openRatio);//���t�J�n���̃J�o�[�ʒu�̎w��(1000�����w��)


	void openMiddleCover();//���t���ɃJ�o�[���グ�鏈��
	void closeMiddleCover();//���t���ɃJ�o�[����鏈��
	void stopMiddleCover();//���t���ɃJ�o�[�̓������~�߂鏈��
	void setMiddleCover();//(�R���g���[���p)//���t���ɃJ�o�[�̈ʒu���w��

	int getMiddleCoverBottomY();

	double getSpeedRatio();//�̊��n�C�X�s�{�������߂�


};

